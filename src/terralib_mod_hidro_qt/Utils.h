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
\file terrahidro/src/hidro_qt/Utils.h

\brief Utilitary function for hidro qt module
*/

#ifndef __THAPP_INTERNAL_HIDRO_QT_UTILS_H
#define __THAPP_INTERNAL_HIDRO_QT_UTILS_H

#include "Config.h"

// TerraLib
#include <terralib/dataaccess/datasource/DataSource.h>
#include <terralib/graph/core/AbstractGraph.h>
#include <terralib/maptools/AbstractLayer.h>

// STL
#include <string>

namespace te
{
  namespace hidro
  {
    TEHIDROQTEXPORT te::da::DataSourcePtr CreateOGRDataSource(std::string repository);

    TEHIDROQTEXPORT te::da::DataSourcePtr CreateOGRDataSource(std::string path, std::string dataSetName);

    TEHIDROQTEXPORT te::da::DataSourcePtr CreateGDALDataSource(std::string path, std::string dataSetName);

    TEHIDROQTEXPORT te::map::AbstractLayerPtr CreateLayer(te::da::DataSourcePtr ds, std::string dataSetName);

    TEHIDROQTEXPORT te::map::AbstractLayerPtr CreateGraphLayer(te::graph::AbstractGraph* graph, std::string dataSourceId);

  } // end namespace hidro
}   // end namespace te

#endif  // __THAPP_INTERNAL_HIDRO_QT_UTILS_H

