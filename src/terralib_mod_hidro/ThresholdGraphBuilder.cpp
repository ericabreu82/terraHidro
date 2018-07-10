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
  \file ThresholdGraphBuilder.cpp
*/

#include "ThresholdGraphBuilder.h"

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
#include <terralib/graph/iterator/AbstractIterator.h>
#include <terralib/graph/iterator/QueryIterator.h>
#include <terralib/graph/Config.h>
#include <terralib/graph/Exception.h>



te::hidro::ThresholdGraphBuilder::ThresholdGraphBuilder()
{
}

te::hidro::ThresholdGraphBuilder::~ThresholdGraphBuilder()
{
}

std::string te::hidro::ThresholdGraphBuilder::getErrorMessage()
{
  return m_errorMessage;
}

te::graph::AbstractGraph* te::hidro::ThresholdGraphBuilder::getGraph()
{
  return m_graph.release();
}

bool te::hidro::ThresholdGraphBuilder::build(te::graph::AbstractGraph* g, te::da::Expression* e, const std::string& dsInfo, const std::string& graphType, const std::map<std::string, std::string>& gInfo)
{
  //create output graph
  m_graph.reset(te::graph::AbstractGraphFactory::make(graphType, dsInfo, gInfo));

  m_graph->getMetadata()->setEnvelope(*g->getMetadata()->getEnvelope());
  m_graph->getMetadata()->setSRID(g->getMetadata()->getSRID());

  assert(m_graph.get());

  //copy attributes
  for (int t = 0; t < g->getEdgePropertySize(); ++t)
  {
    te::dt::Property* p = g->getMetadata()->getEdgeProperty(t)->clone();
    p->setParent(0);

    m_graph->addEdgeProperty(p);
  }

  for (int t = 0; t < g->getVertexPropertySize(); ++t)
  {
    te::dt::Property* p = g->getMetadata()->getVertexProperty(t)->clone();
    p->setParent(0);

    m_graph->addVertexProperty(p);
  }

  //create iterator
  te::graph::QueryIterator* it = new te::graph::QueryIterator(g, e);

  te::graph::Edge* edge = it->getFirstEdge();

  te::common::TaskProgress t;

  t.setTotalSteps(it->getEdgeInteratorCount());
  t.setMessage(TE_TR("Build Graph..."));
  t.useTimer(true);


  //copy elements
  while (it->isEdgeIteratorAfterEnd() == false)
  {
    if (edge)
    {
      te::graph::Vertex* vFrom = g->getVertex(edge->getIdFrom());
      te::graph::Vertex* vTo = g->getVertex(edge->getIdTo());

      if (vFrom && vTo)
      {
        te::graph::Vertex* vFromNew = new te::graph::Vertex(vFrom);
        te::graph::Vertex* vToNew = new te::graph::Vertex(vTo);

        te::graph::Edge* eNew = new te::graph::Edge(edge);

        m_graph->add(vFromNew);
        m_graph->add(vToNew);
        m_graph->add(eNew);
      }
    }

    edge = it->getNextEdge();

    t.pulse();
  }

  delete it;

  return true;
}
