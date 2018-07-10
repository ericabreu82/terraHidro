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
  \file LDDGraphBuilder.cpp

  \brief This class defines the LDD strategy to build a graph.

        This strategy is based on Serio Rosim method, using this
        "mask" is possible extract a graph from a LDD image.

                    -------------------
                    |  32 |  64 | 128 |
                    -------------------
                    |  16 |  *  |  1  |
                    -------------------
                    |  8  |  4  |  2  |
                    -------------------

*/

#include "LDDGraphBuilder.h"

// TerraLib
#include <terralib/core/translator/Translator.h>
#include <terralib/common/progress/TaskProgress.h>
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
#include <terralib/graph/Config.h>
#include <terralib/graph/Exception.h>
#include <terralib/raster/Grid.h>
#include <terralib/raster/Raster.h>



te::hidro::LDDGraphBuilder::LDDGraphBuilder() : m_raster(0), m_edgeId(0)
{
}

te::hidro::LDDGraphBuilder::~LDDGraphBuilder()
{
}

std::string te::hidro::LDDGraphBuilder::getErrorMessage()
{
  return m_errorMessage;
}

te::graph::AbstractGraph* te::hidro::LDDGraphBuilder::getGraph()
{
  return m_graph.release();
}

bool te::hidro::LDDGraphBuilder::build(te::rst::Raster* raster, const std::string& dsInfo, const std::string& graphType, const std::map<std::string, std::string>& gInfo)
{
  m_raster = raster;

  //create output graph
  m_graph.reset(te::graph::AbstractGraphFactory::make(graphType, dsInfo, gInfo));

  assert(m_graph.get());

  //create graph attrs
  te::gm::GeometryProperty* gProp = new te::gm::GeometryProperty("coords");
  gProp->setId(0);
  gProp->setGeometryType(te::gm::PointType);
  gProp->setSRID(raster->getSRID());

  m_graph->addVertexProperty(gProp);
  m_graph->getMetadata()->setEnvelope(te::gm::Envelope(*raster->getExtent()));
  m_graph->getMetadata()->setSRID(raster->getSRID());

  te::common::TaskProgress t;

  t.setTotalSteps(m_raster->getNumberOfRows());
  t.setMessage(TE_TR("LLD Graph Builder..."));
  t.useTimer(true);

  //extract graph
  for (std::size_t r = 1; r < m_raster->getNumberOfRows(); r++)
  {
    for (std::size_t c = 1; c < m_raster->getNumberOfColumns(); c++)
    {
      //get pixel value
      double val = 255.;
      m_raster->getValue(c, r, val);

      int value = (int)val;

      //value to be ignored
      if (value == 255 || value == 0)
      {
        continue;
      }

      //get vertex from id
      int vFromId;

      if (getVertexId(r, c, vFromId) == false)
      {
        continue;
      }

      //check vertex to pos
      int vToId;
      int rToPos = -1;
      int cToPos = -1;

      if (value & 1)
      {
        rToPos = r;
        cToPos = c + 1;
      }

      if (value & 2)
      {
        rToPos = r + 1;
        cToPos = c + 1;
      }

      if (value & 4)
      {
        rToPos = r + 1;
        cToPos = c;
      }

      if (value & 8)
      {
        rToPos = r + 1;
        cToPos = c - 1;
      }

      if (value & 16)
      {
        rToPos = r;
        cToPos = c - 1;
      }

      if (value & 32)
      {
        rToPos = r - 1;
        cToPos = c - 1;
      }

      if (value & 64)
      {
        rToPos = r - 1;
        cToPos = c;
      }

      if (value & 128)
      {
        rToPos = r - 1;
        cToPos = c + 1;
      }


      if (getVertexId(rToPos, cToPos, vToId))
      {
        //create vertexFrom
        if (!m_graph->getVertex(vFromId))
        {
          te::graph::Vertex* vFrom = new te::graph::Vertex(vFromId);
          vFrom->setAttributeVecSize(m_graph->getVertexPropertySize());

          te::gm::Coord2D coord = m_raster->getGrid()->gridToGeo(c, r);
          te::gm::Point* p = new te::gm::Point(coord.x, coord.y, gProp->getSRID());

          vFrom->addAttribute(0, p);

          m_graph->add(vFrom);
        }

        //create vertexTo
        if (!m_graph->getVertex(vToId))
        {
          te::graph::Vertex* vTo = new te::graph::Vertex(vToId);
          vTo->setAttributeVecSize(m_graph->getVertexPropertySize());

          te::gm::Coord2D coord = m_raster->getGrid()->gridToGeo(cToPos, rToPos);
          te::gm::Point* p = new te::gm::Point(coord.x, coord.y, gProp->getSRID());

          vTo->addAttribute(0, p);

          m_graph->add(vTo);
        }

        //create edge
        int edgeId = getEdgeId();

        te::graph::Edge* e = new te::graph::Edge(edgeId, vFromId, vToId);

        m_graph->add(e);
      }
    }

    t.pulse();
  }

  return true;
}

bool te::hidro::LDDGraphBuilder::getVertexId(int row, int col, int& id)
{
  if( row < 1 || row > (int)m_raster->getNumberOfRows() - 1)
  {
    return false;
  }

  if( col < 1 || row > (int)m_raster->getNumberOfColumns() - 1)
  {
    return false;
  }

  if(m_raster)
  {
    id = (row * m_raster->getNumberOfColumns()) + col;

    return true;
  }

  return false;
}

int  te::hidro::LDDGraphBuilder::getEdgeId()
{
  int id = m_edgeId;

  m_edgeId++;

  return id;
}