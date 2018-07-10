/*  Copyright (C) 2011-2012 National Institute For Space Research (INPE) - Brazil.

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
\file terrahidro/src/hidro/GraphExport.h

\brief This file defines the Graph Export class
*/

#ifndef __THAPP_INTERNAL_HIDRO_GRAPHEXPORT_H
#define __THAPP_INTERNAL_HIDRO_GRAPHEXPORT_H

#include "Config.h"

// TerraLib
#include <terralib/dataaccess/datasource/DataSource.h>
#include <terralib/graph/core/AbstractGraph.h>
#include <terralib/memory/DataSet.h>


// STL
#include <memory>

namespace te
{
  namespace hidro
  {
    enum GraphExportType
    {
      GRAPH_VERTEX_TYPE,
      GRAPH_EDGE_TYPE
    };

    /*!
    \class GraphExport

    \brief This class is used to export a graph to a datasource
    */
    class TEHIDRODEXPORT GraphExport
    {

      public:

        GraphExport();

        ~GraphExport();

      public:

        void exportGraph(te::da::DataSourcePtr ds, std::string dataSetName, te::graph::AbstractGraph* graph, GraphExportType exportType);

      protected:

        std::auto_ptr<te::da::DataSetType> createEdgeDataSetType(std::string dataSetName, te::graph::AbstractGraph* graph);

        std::auto_ptr<te::da::DataSetType> createVertexDataSetType(std::string dataSetName, te::graph::AbstractGraph* graph);

        std::auto_ptr<te::mem::DataSet> createEdgeDataSet(te::da::DataSetType* dsType, te::graph::AbstractGraph* graph);

        std::auto_ptr<te::mem::DataSet> createVertexDataSet(te::da::DataSetType* dsType, te::graph::AbstractGraph* graph);

        std::map<int, std::string> getEdgePropertyMap(te::graph::AbstractGraph* graph);

        std::map<int, std::string> getVertexPropertyMap(te::graph::AbstractGraph* graph);

        bool getGraphVerterxAttrIndex(te::graph::AbstractGraph* graph, std::string attrName, int& index);

    };
  }       // end namespace hidro
}         // end namespace te

#endif  // __THAPP_INTERNAL_HIDRO_GRAPHEXPORT_H

