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
\file terrahidro/src/hidro_qt/Utils.cpp

\brief Utilitary function for hidro qt module
*/

#include "Utils.h"

// TerraLib
#include <terralib/dataaccess/datasource/DataSource.h>
#include <terralib/dataaccess/datasource/DataSourceInfo.h>
#include <terralib/dataaccess/datasource/DataSourceInfoManager.h>
#include <terralib/dataaccess/datasource/DataSourceManager.h>
#include <terralib/graph/core/GraphMetadata.h>
#include <terralib/graph/maptools/Layer.h>
#include <terralib/graph/maptools/LayerRenderer.h>
#include <terralib/qt/widgets/layer/utils/DataSet2Layer.h>
#include <terralib/se.h>

// STL
#include <cassert>

// Boost
#include <boost/filesystem.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

te::da::DataSourcePtr te::hidro::CreateOGRDataSource(std::string repository)
{
  //create new data source
  boost::filesystem::path uri(repository);

  std::string connInfo("file://");
  connInfo += uri.string();

  boost::uuids::basic_random_generator<boost::mt19937> gen;
  boost::uuids::uuid u = gen();
  std::string id_ds = boost::uuids::to_string(u);

  te::da::DataSourceInfoPtr dsInfoPtr(new te::da::DataSourceInfo);
  dsInfoPtr->setConnInfo(connInfo);
  dsInfoPtr->setTitle(uri.stem().string());
  dsInfoPtr->setAccessDriver("OGR");
  dsInfoPtr->setType("OGR");
  dsInfoPtr->setDescription(uri.string());
  dsInfoPtr->setId(id_ds);

  te::da::DataSourceInfoManager::getInstance().add(dsInfoPtr);

  return te::da::DataSourceManager::getInstance().get(id_ds, "OGR", dsInfoPtr->getConnInfo());
}

te::da::DataSourcePtr te::hidro::CreateOGRDataSource(std::string path, std::string dataSetName)
{
  std::string name = path + "/" + dataSetName + ".shp";

  return CreateOGRDataSource(name);
}

te::da::DataSourcePtr te::hidro::CreateGDALDataSource(std::string path, std::string dataSetName)
{
  std::string name = path + "/" + dataSetName + ".tif";

  //create new data source
  boost::filesystem::path uri(name);

  std::string connInfo("file://");
  connInfo += uri.string();

  boost::uuids::basic_random_generator<boost::mt19937> gen;
  boost::uuids::uuid u = gen();
  std::string id_ds = boost::uuids::to_string(u);

  te::da::DataSourceInfoPtr dsInfoPtr(new te::da::DataSourceInfo);
  dsInfoPtr->setConnInfo(connInfo);
  dsInfoPtr->setTitle(uri.stem().string());
  dsInfoPtr->setAccessDriver("GDAL");
  dsInfoPtr->setType("GDAL");
  dsInfoPtr->setDescription(uri.string());
  dsInfoPtr->setId(id_ds);

  te::da::DataSourceInfoManager::getInstance().add(dsInfoPtr);

  return te::da::DataSourceManager::getInstance().get(id_ds, "GDAL", dsInfoPtr->getConnInfo());
}

te::map::AbstractLayerPtr te::hidro::CreateLayer(te::da::DataSourcePtr ds, std::string dataSetName)
{
  te::qt::widgets::DataSet2Layer converter(ds->getId());

  te::da::DataSetTypePtr dt(ds->getDataSetType(dataSetName).release());

  return converter(dt);
}

te::map::AbstractLayerPtr te::hidro::CreateGraphLayer(te::graph::AbstractGraph* graph, std::string dataSourceId)
{
  graph->getMetadata()->m_maxCacheSize = 10000;
  graph->getMetadata()->m_maxVecCacheSize = 5;

  // set visual
  te::se::Stroke* stroke = te::se::CreateStroke("#0000FF", "1.0");
  te::se::LineSymbolizer* lineSymbolizer = te::se::CreateLineSymbolizer(stroke);

  te::se::Fill* markFill = te::se::CreateFill("#009900", "1.0");
  te::se::Stroke* markStroke = te::se::CreateStroke("#000000", "1");
  te::se::Mark* mark = te::se::CreateMark("circle", markStroke, markFill);
  te::se::Graphic* graphic = te::se::CreateGraphic(mark, "4", "", "");
  te::se::PointSymbolizer* pointSymbolizer = te::se::CreatePointSymbolizer(graphic);

  te::se::Rule* rule = new te::se::Rule;
  rule->push_back(lineSymbolizer);
  rule->push_back(pointSymbolizer);

  te::se::FeatureTypeStyle* style = new te::se::FeatureTypeStyle;
  style->push_back(rule);

  // layer id
  boost::uuids::basic_random_generator<boost::mt19937> gen;
  boost::uuids::uuid u = gen();
  std::string id_layer = boost::uuids::to_string(u);

  // creates a layer
  te::graph::Layer* layer = new te::graph::Layer(id_layer, graph->getMetadata()->getName());
  layer->setDataSourceId(dataSourceId);
  layer->setVisibility(te::map::VISIBLE);
  layer->setStyle(style);
  layer->setGraph(graph);
  layer->setDataSetName(graph->getMetadata()->getVertexTableName());
  

  // creates a layer renderer
  te::graph::LayerRenderer* r = new te::graph::LayerRenderer();
  layer->setRenderer(r);

  return layer;
}