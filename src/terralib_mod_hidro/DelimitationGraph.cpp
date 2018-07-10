/*  Copyright (C) 2008 National Institute For Space Research (INPE) - Brazil.

    This file is part of the TerraLib - a Framework for building GIS enabled applications.

    TerraLib is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License,
    or (at your option) any later version.

    TerraLib is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with TerraLib. See COPYING. If not, write to
    TerraLib Team at <terralib-team@terralib.org>.
 */

/*!
\file DelimitationGraph.h

\brief This class defines the Delimitation Graph method.

*/

#include "DelimitationGraph.h"

// TerraLib
#include <terralib/core/translator/Translator.h>
#include <terralib/common/progress/TaskProgress.h>
#include <terralib/dataaccess/datasource/DataSource.h>
#include <terralib/dataaccess/query/DataSetName.h>
#include <terralib/dataaccess/query/EqualTo.h>
#include <terralib/dataaccess/query/Fields.h>
#include <terralib/dataaccess/query/Field.h>
#include <terralib/dataaccess/query/From.h>
#include <terralib/dataaccess/query/FromItem.h>
#include <terralib/dataaccess/query/LiteralInt32.h>
#include <terralib/dataaccess/query/Select.h>
#include <terralib/dataaccess/query/Where.h>
#include <terralib/geometry/Coord2D.h>
#include <terralib/geometry/GeometryProperty.h>
#include <terralib/geometry/Point.h>
#include <terralib/graph/core/AbstractGraph.h>
#include <terralib/graph/core/AbstractGraphFactory.h>
#include <terralib/graph/core/Edge.h>
#include <terralib/graph/core/GraphMetadata.h>
#include <terralib/graph/core/Vertex.h>
#include <terralib/graph/core/VertexProperty.h>
#include <terralib/graph/graphs/Graph.h>
#include <terralib/graph/graphs/BidirectionalGraph.h>
#include <terralib/graph/functions/GetSubGraph.h>
#include <terralib/graph/iterator/MemoryIterator.h>
#include <terralib/graph/Config.h>
#include <terralib/graph/Exception.h>
#include <terralib/graph/Globals.h>
#include <terralib/raster/BandProperty.h>
#include <terralib/raster/Grid.h>
#include <terralib/raster/Raster.h>
#include <terralib/raster/RasterFactory.h>
#include <terralib/raster/Utils.h>



te::hidro::DelimitationGraph::DelimitationGraph() : m_inputRaster(0), m_inputGraph(0), m_errorMessage("")
{
}

te::hidro::DelimitationGraph::~DelimitationGraph()
{
}

std::string te::hidro::DelimitationGraph::getErrorMessage()
{
  return m_errorMessage;
}

bool te::hidro::DelimitationGraph::execute(te::rst::Raster* inputRaster, te::graph::AbstractGraph* inputGraph, const std::string& endVertexId, const std::string& outputhPath)
{
  inputGraph->flush();

  m_inputRaster = inputRaster;
  m_inputGraph = inputGraph;

  //get vertex geom attribute
  int index = -1;
  for (int i = 0; i < inputGraph->getVertexPropertySize(); ++i)
  {
    if (inputGraph->getVertexProperty(i)->getType() == te::dt::GEOMETRY_TYPE)
    {
      index = i;
      break;
    }
  }

  te::graph::BidirectionalGraph* biGraph = dynamic_cast<te::graph::BidirectionalGraph*>(inputGraph);

  //create output graph
  te::rst::Grid* grid = new te::rst::Grid(*inputRaster->getGrid());

  std::vector<te::rst::BandProperty*> vecBandProp;
  te::rst::BandProperty* bProp = new te::rst::BandProperty(0, te::dt::UCHAR_TYPE, "");
  bProp->m_noDataValue = 255.;
  vecBandProp.push_back(bProp);

  std::map<std::string, std::string> conInfo;
  conInfo["URI"] = outputhPath;

  std::auto_ptr<te::rst::Raster> rst(te::rst::RasterFactory::make("GDAL", grid, vecBandProp, conInfo));

  te::rst::FillRaster(rst.get(), rst->getBand(0)->getProperty()->m_noDataValue);

  //set raster values using sequence iterator

  te::graph::Vertex* vEnd = biGraph->getVertex(atoi(endVertexId.c_str()));

  if (vEnd)
  {
    te::gm::Point* p = dynamic_cast<te::gm::Point*>(vEnd->getAttributes()[index]);

    if (p)
    {
      te::gm::Coord2D coord = rst->getGrid()->geoToGrid(p->getX(), p->getY());

      rst->setValue((int)coord.x, (int)coord.y, 0., 0);
    }

    std::set<int> vertexIdSet;

    vertexIdSet.insert(vEnd->getId());

    run(vEnd, biGraph, vertexIdSet, index, rst.get());
  }


  return true;
}

void te::hidro::DelimitationGraph::run(te::graph::Vertex* v, te::graph::BidirectionalGraph* g, std::set<int>& vertexIdSet, int attrIdx, te::rst::Raster* outRaster)
{
  std::set<int> predecessors = getPredecessors(v->getId(), g);

  if (predecessors.empty() == false)
  {
    std::set<int>::iterator it = predecessors.begin();

    while (it != predecessors.end())
    {
      te::graph::Vertex* vFrom = g->getVertex(*it);

      if (vFrom)
      {
        std::set<int>::iterator itSet = vertexIdSet.find(vFrom->getId());

        if (itSet == vertexIdSet.end())
        {
          te::gm::Point* p = dynamic_cast<te::gm::Point*>(vFrom->getAttributes()[attrIdx]);

          if (p)
          {
            te::gm::Coord2D coord = outRaster->getGrid()->geoToGrid(p->getX(), p->getY());

            outRaster->setValue((int)coord.x, (int)coord.y, 0., 0);
          }

          vertexIdSet.insert(vFrom->getId());

          run(vFrom, g, vertexIdSet, attrIdx, outRaster);
        }
      }

      ++it;
    }
  }
}

std::set<int> te::hidro::DelimitationGraph::getPredecessors(int vertexId, te::graph::AbstractGraph* inputGraph)
{
  //select vertex_from from asu_model_edge where vertex_to = 35001

  std::string edgeAttrTable = inputGraph->getMetadata()->getEdgeTableName();

  //Fields
  te::da::Fields* fields = new te::da::Fields;
  fields->push_back(new te::da::Field(te::graph::Globals::sm_tableEdgeModelAttrVFrom));

  //From
  te::da::From* from = new te::da::From;
  from->push_back(new te::da::DataSetName(edgeAttrTable, "edge"));

  //where
  te::da::Field* fieldVTo = new te::da::Field(te::graph::Globals::sm_tableEdgeModelAttrVTo);
  te::da::EqualTo* equalTo = new te::da::EqualTo(fieldVTo->getExpression(), new te::da::LiteralInt32(vertexId));

  te::da::Where* wh = new te::da::Where(equalTo);

  //select
  te::da::Select select(fields, from, wh);

  //query
  std::auto_ptr<te::da::DataSet> dataset = inputGraph->getMetadata()->getDataSource()->query(select);

  std::set<int> predecessors;

  while (dataset->moveNext())
  {
    int vFromId = dataset->getInt32(0);

    predecessors.insert(vFromId);
  }

  return predecessors;
}

/*
bool te::hidro::DelimitationGraph::execute(te::rst::Raster* inputRaster, te::graph::AbstractGraph* inputGraph, const std::string& endVertexId, const std::string& outputhPath)
{
m_inputRaster = inputRaster;
m_inputGraph = inputGraph;

//create memoryGraph
std::string graphType = te::graph::Globals::sm_factoryGraphTypeBidirectionalGraph;

std::string connInfo;

std::map<std::string, std::string> graphInfo;
graphInfo["GRAPH_DATA_SOURCE_TYPE"] = "MEM";
graphInfo["GRAPH_NAME"] = "DelimitationGraph";
graphInfo["GRAPH_DESCRIPTION"] = "Generated by Delimitation Graph.";

te::graph::AbstractGraph* memGraph = te::graph::AbstractGraphFactory::make(graphType, connInfo, graphInfo);

//get sub graph
te::graph::BidirectionalGraph* biGraph = dynamic_cast<te::graph::BidirectionalGraph*>(inputGraph);
te::graph::GetSubGraph gsg(biGraph, memGraph, atoi(endVertexId.c_str()));

int index = -1;
for (int i = 0; i < inputGraph->getVertexPropertySize(); ++i)
{
if (inputGraph->getVertexProperty(i)->getType() == te::dt::GEOMETRY_TYPE)
{
index = i;
break;
}
}

//create output graph
te::rst::Grid* grid = new te::rst::Grid(*inputRaster->getGrid());

std::vector<te::rst::BandProperty*> vecBandProp;
te::rst::BandProperty* bProp = new te::rst::BandProperty(0, te::dt::UCHAR_TYPE, "");
bProp->m_noDataValue = 255.;
vecBandProp.push_back(bProp);

std::map<std::string, std::string> conInfo;
conInfo["URI"] = outputhPath;

std::auto_ptr<te::rst::Raster> rst(te::rst::RasterFactory::make("GDAL", grid, vecBandProp, conInfo));

te::rst::FillRaster(rst.get(), rst->getBand(0)->getProperty()->m_noDataValue);

//set raster values using sequence iterator
te::graph::MemoryIterator* it = new te::graph::MemoryIterator(memGraph);

te::graph::Vertex* vertex = it->getFirstVertex();

te::common::TaskProgress task;

task.setTotalSteps(it->getVertexInteratorCount());
task.setMessage("Delimitation Graph Operation");

while (it->isVertexIteratorAfterEnd() == false)
{
if (vertex)
{
te::gm::Point* p = dynamic_cast<te::gm::Point*>(vertex->getAttributes()[index]);

if (p)
{
te::gm::Coord2D coord = rst->getGrid()->geoToGrid(p->getX(), p->getY());

rst->setValue((int)coord.x, (int)coord.y, 0., 0);
}
}

vertex = it->getNextVertex();

task.pulse();
}

delete it;

delete memGraph;

return true;
}
*/