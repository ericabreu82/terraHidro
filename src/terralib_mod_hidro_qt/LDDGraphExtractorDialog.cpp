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
\file terrahidro/src/hidro_qt/LDDGraphExtractorDialog.cpp

\brief This file defines the UI for LDD Graph extractor.
*/

#include "../terralib_mod_hidro/GraphExport.h"
#include "../terralib_mod_hidro/LDDGraphBuilder.h"
#include "LDDGraphExtractorDialog.h"
#include "Utils.h"
#include "ui_LDDGraphExtractorDialogForm.h"

// TerraLib
#include <terralib/core/logger/Logger.h>
#include <terralib/common/progress/ProgressManager.h>
#include <terralib/core/translator/Translator.h>
#include <terralib/common/STLUtils.h>
#include <terralib/dataaccess/utils/Utils.h>
#include <terralib/dataaccess/datasource/DataSource.h>
#include <terralib/geometry/GeometryProperty.h>
#include <terralib/graph/core/AbstractGraph.h>
#include <terralib/graph/functions/AddDeepAttribute.h>
#include <terralib/graph/graphs/BidirectionalGraph.h>
#include <terralib/graph/Globals.h>
#include <terralib/maptools/DataSetLayer.h>
#include <terralib/raster/Grid.h>
#include <terralib/qt/widgets/datasource/selector/DataSourceSelectorDialog.h>
#include <terralib/qt/widgets/progress/ProgressViewerDialog.h>

// Qt
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QValidator>

// STL
#include <memory>

// Boost
#include <boost/filesystem.hpp>

Q_DECLARE_METATYPE(te::map::AbstractLayerPtr);

te::hidro::LDDGraphExtractorDialog::LDDGraphExtractorDialog(QWidget* parent, Qt::WindowFlags f)
  : QDialog(parent, f),
    m_ui(new Ui::LDDGraphExtractorDialogForm)
{
// add controls
  m_ui->setupUi(this);

// add icons
  m_ui->m_imgLabel->setPixmap(QIcon(":/images/hidro-lddextractor-hint").pixmap(112,48));
  m_ui->m_targetDatasourceToolButton->setIcon(QIcon::fromTheme("datasource"));

// connectors
  connect(m_ui->m_okPushButton, SIGNAL(clicked()), this, SLOT(onOkPushButtonClicked()));
  connect(m_ui->m_targetDatasourceToolButton, SIGNAL(pressed()), this, SLOT(onTargetDatasourceToolButtonPressed()));
  connect(m_ui->m_targetFileToolButton, SIGNAL(pressed()), this,  SLOT(onTargetFileToolButtonPressed()));

// help info
}

te::hidro::LDDGraphExtractorDialog::~LDDGraphExtractorDialog()
{
}

void te::hidro::LDDGraphExtractorDialog::setLayers(std::list<te::map::AbstractLayerPtr> layers)
{
  std::list<te::map::AbstractLayerPtr>::iterator it = layers.begin();

  while(it != layers.end())
  {
    te::map::AbstractLayerPtr l = *it;

    if(l->isValid())
    {
      std::auto_ptr<te::da::DataSetType> dsType = l->getSchema();

      te::map::DataSetLayer* dsLayer = dynamic_cast<te::map::DataSetLayer*>(l.get());

      if(dsLayer && dsType->hasRaster())
        m_ui->m_inputLayerComboBox->addItem(it->get()->getTitle().c_str(), QVariant::fromValue(l));
    }

    ++it;
  }
}

te::map::AbstractLayerPtr te::hidro::LDDGraphExtractorDialog::getOutputLayer()
{
  return m_outputLayer;
}

void te::hidro::LDDGraphExtractorDialog::onOkPushButtonClicked()
{
  // check input parameters
  if(m_ui->m_repositoryLineEdit->text().isEmpty())
  {
    QMessageBox::information(this, tr("Warning"), tr("Define a repository for the result."));
    return;
  }
       
  if(m_ui->m_newLayerNameLineEdit->text().isEmpty())
  {
    QMessageBox::information(this, tr("Warning"), tr("Define a name for the resulting layer."));
    return;
  }

  if (m_ui->m_contribAreaCheckBox->isChecked())
  {
    if (m_ui->m_contribAreaAttrNameLineEdit->text().isEmpty())
    {
      QMessageBox::information(this, tr("Warning"), tr("Define a name for Contribution Area Attribute."));
      return;
    }
  }

//get selected layer
  QVariant varLayer = m_ui->m_inputLayerComboBox->itemData(m_ui->m_inputLayerComboBox->currentIndex(), Qt::UserRole);
  te::map::AbstractLayerPtr l = varLayer.value<te::map::AbstractLayerPtr>();

  std::auto_ptr<te::da::DataSet> ds = l->getData();
  std::size_t rpos = te::da::GetFirstPropertyPos(ds.get(), te::dt::RASTER_TYPE);
  std::auto_ptr<te::rst::Raster> inputRst = ds->getRaster(rpos);
  inputRst->getGrid()->setSRID(l->getSRID());

//create datasource to save the output information
  std::string dataSetName = m_ui->m_newLayerNameLineEdit->text().toStdString();

  std::size_t idx = dataSetName.find(".");
  if (idx != std::string::npos)
        dataSetName=dataSetName.substr(0,idx);

  te::da::DataSourcePtr outputDataSource;

  if(m_toFile)
  {
    outputDataSource = te::hidro::CreateOGRDataSource(m_ui->m_repositoryLineEdit->text().toStdString());
  }
  else
  {
    outputDataSource = te::da::GetDataSource(m_outputDatasource->getId());
  }

  //operation parameters

  // graph type
  std::string graphType = te::graph::Globals::sm_factoryGraphTypeBidirectionalGraph;

  //// connection info
  //std::map<std::string, std::string> connInfo;

  //// graph information
  //std::map<std::string, std::string> graphInfo;
  //graphInfo["GRAPH_DATA_SOURCE_TYPE"] = "MEM";
  //graphInfo["GRAPH_NAME"] = dataSetName;
  //graphInfo["GRAPH_DESCRIPTION"] = "Generated by LDD Builder.";

  // data source information
  std::string uri = outputDataSource->getConnectionInfo().uri();
  //connInfo["PG_HOST"] = "localhost";
  //connInfo["PG_USER"] = "postgres";
  //connInfo["PG_PASSWORD"] = "tdk696";
  //connInfo["PG_DB_NAME"] = "t5graph";
  //connInfo["PG_CONNECT_TIMEOUT"] = "4";
  //connInfo["PG_CLIENT_ENCODING"] = "LATIN1";

  // graph information
  std::map<std::string, std::string> graphInfo;
  graphInfo["GRAPH_DATA_SOURCE_TYPE"] = outputDataSource->getType();
  graphInfo["GRAPH_NAME"] = dataSetName;
  graphInfo["GRAPH_DESCRIPTION"] = "Generated by LDD Builder.";
  graphInfo["GRAPH_STORAGE_MODE"] = te::graph::Globals::sm_edgeStorageMode;
  graphInfo["GRAPH_STRATEGY_LOADER"] = te::graph::Globals::sm_factoryLoaderStrategyTypeSequence;
  graphInfo["GRAPH_CACHE_POLICY"] = "FIFO";

  //progress
  te::qt::widgets::ProgressViewerDialog v(this);
  int id = te::common::ProgressManager::getInstance().addViewer(&v);

  QApplication::setOverrideCursor(Qt::WaitCursor);

  te::graph::AbstractGraph* graph;

  try
  {
    te::hidro::LDDGraphBuilder builder;
    if (!builder.build(inputRst.get(), uri, graphType, graphInfo))
    {
      QMessageBox::warning(this, tr("Warning"), builder.getErrorMessage().c_str());

      QApplication::restoreOverrideCursor();

      te::common::ProgressManager::getInstance().removeViewer(id);

      return;
    }

    graph = builder.getGraph();

    if (m_ui->m_contribAreaCheckBox->isChecked())
    {
      te::graph::BidirectionalGraph* biGraph = dynamic_cast<te::graph::BidirectionalGraph*>(graph);

      std::string attrName = m_ui->m_contribAreaAttrNameLineEdit->text().toUtf8().constData();

      if (biGraph)
      {
        te::graph::AddDeepAttribute(biGraph, attrName);
      }
    }

    graph->flush();

    //te::hidro::GraphExport ge;

    //ge.exportGraph(outputDataSource, dataSetName, graph.get(), te::hidro::GRAPH_EDGE_TYPE);
  }
  catch(const std::exception& e)
  {
    QMessageBox::warning(this, tr("Warning"), e.what());

    QApplication::restoreOverrideCursor();

    te::common::ProgressManager::getInstance().removeViewer(id);

    return;
  }
  catch(...)
  {
    QMessageBox::warning(this, tr("Warning"), tr("Internal Error."));

    QApplication::restoreOverrideCursor();

    te::common::ProgressManager::getInstance().removeViewer(id);

    return;
  }

  QApplication::restoreOverrideCursor();

  te::common::ProgressManager::getInstance().removeViewer(id);

  //create layer
  m_outputLayer = te::hidro::CreateGraphLayer(graph, outputDataSource->getId());

  te::map::DataSetLayer* dsLayer = dynamic_cast<te::map::DataSetLayer*>(m_outputLayer.get());
  if (dsLayer != 0)
    dsLayer->setRendererType("HIDROGRAPH_LAYER_RENDERER");

  accept();
}

void te::hidro::LDDGraphExtractorDialog::onTargetDatasourceToolButtonPressed()
{
  m_ui->m_newLayerNameLineEdit->clear();
  m_ui->m_newLayerNameLineEdit->setEnabled(true);

  te::qt::widgets::DataSourceSelectorDialog dlg(this);
  dlg.exec();

  std::list<te::da::DataSourceInfoPtr> dsPtrList = dlg.getSelecteds();

  if(dsPtrList.empty())
    return;

  std::list<te::da::DataSourceInfoPtr>::iterator it = dsPtrList.begin();

  m_ui->m_repositoryLineEdit->setText(QString(it->get()->getTitle().c_str()));

  m_outputDatasource = *it;
  
  m_toFile = false;
}

void te::hidro::LDDGraphExtractorDialog::onTargetFileToolButtonPressed()
{
  m_ui->m_newLayerNameLineEdit->clear();
  m_ui->m_repositoryLineEdit->clear();
  
  QString fileName = QFileDialog::getSaveFileName(this, tr("Save as..."), QString(), tr("Shapefile (*.shp *.SHP);;"),0, QFileDialog::DontConfirmOverwrite);
  
  if (fileName.isEmpty())
    return;
  
  boost::filesystem::path outfile(fileName.toStdString());

  m_ui->m_repositoryLineEdit->setText(outfile.string().c_str());

  m_ui->m_newLayerNameLineEdit->setText(outfile.leaf().string().c_str());

  m_ui->m_newLayerNameLineEdit->setEnabled(false);
  
  m_toFile = true;
}
