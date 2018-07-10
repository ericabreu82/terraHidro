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
\file terrahidro/src/hidro_qt/DelimitationGraphDialog.h

\brief This file defines the UI for Delimitation Graph.
*/

#include "../terralib_mod_hidro/DelimitationGraph.h"
#include "DelimitationGraphDialog.h"
#include "Utils.h"
#include "ui_DelimitationGraphDialogForm.h"

// TerraLib
#include <terralib/core/logger/Logger.h>
#include <terralib/common/progress/ProgressManager.h>
#include <terralib/core/translator/Translator.h>
#include <terralib/common/STLUtils.h>
#include <terralib/dataaccess/utils/Utils.h>
#include <terralib/geometry/GeometryProperty.h>
#include <terralib/graph/core/AbstractGraph.h>
#include <terralib/graph/core/Vertex.h>
#include <terralib/graph/iterator/BoxIterator.h>
#include <terralib/graph/maptools/Layer.h>
#include <terralib/graph/Globals.h>
#include <terralib/maptools/Utils.h>
#include <terralib/raster/Grid.h>
#include <terralib/qt/widgets/progress/ProgressViewerDialog.h>
#include <terralib/qt/widgets/tools/ExtentAcquire.h>
#include <terralib/qt/widgets/Utils.h>

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

te::hidro::DelimitationGraphDialog::DelimitationGraphDialog(QWidget* parent, Qt::WindowFlags f)
  : QDialog(parent, f),
  m_ui(new Ui::DelimitationGraphDialogForm)
{
// add controls
  m_ui->setupUi(this);

  setAttribute(Qt::WA_DeleteOnClose, true);

// connectors
  connect(m_ui->m_okPushButton, SIGNAL(clicked()), this, SLOT(onOkPushButtonClicked()));
  connect(m_ui->m_targetFileToolButton, SIGNAL(pressed()), this,  SLOT(onTargetFileToolButtonPressed()));
  connect(m_ui->m_vertexPickToolButton, SIGNAL(pressed()), this, SLOT(onVertexPickToolButtonClicked()));
  m_mapDisplay = 0;
}

te::hidro::DelimitationGraphDialog::~DelimitationGraphDialog()
{
}

void te::hidro::DelimitationGraphDialog::setLayers(std::list<te::map::AbstractLayerPtr> layers)
{
  std::list<te::map::AbstractLayerPtr>::iterator it = layers.begin();

  while(it != layers.end())
  {
    te::map::AbstractLayerPtr l = *it;

    if(l->isValid())
    {
      te::graph::Layer* graphLayer = dynamic_cast<te::graph::Layer*>(l.get());

      if (graphLayer)
        m_ui->m_inputGraphLayerComboBox->addItem(it->get()->getTitle().c_str(), QVariant::fromValue(l));
      else
      {
        std::auto_ptr<te::da::DataSetType> dsType = l->getSchema();

        if (dsType.get() && dsType->hasRaster())
        {
          m_ui->m_inputRasterLayerComboBox->addItem(it->get()->getTitle().c_str(), QVariant::fromValue(l));
        }
      }
    }

    ++it;
  }
}

te::map::AbstractLayerPtr te::hidro::DelimitationGraphDialog::getOutputLayer()
{
  return m_outputLayer;
}

void te::hidro::DelimitationGraphDialog::setMapDisplay(te::qt::widgets::MapDisplay* mapDisplay)
{
  m_mapDisplay = mapDisplay;
}

void te::hidro::DelimitationGraphDialog::onOkPushButtonClicked()
{
  // check input parameters
  if (m_ui->m_inputGraphLayerComboBox->currentText().isEmpty())
  {
    QMessageBox::information(this, tr("Warning"), tr("Define the Graph Layer."));
    return;
  }

  if (m_ui->m_inputRasterLayerComboBox->currentText().isEmpty())
  {
    QMessageBox::information(this, tr("Warning"), tr("Define the Raster Layer."));
    return;
  }

  if(m_ui->m_repositoryLineEdit->text().isEmpty())
  {
    QMessageBox::information(this, tr("Warning"), tr("Define a repository for the result."));
    return;
  }
  std::string path = m_ui->m_repositoryLineEdit->text().toUtf8().data();

  if (m_ui->m_endVertexLineEdit->text().isEmpty())
  {
    QMessageBox::information(this, tr("Warning"), tr("Define the End Vertex Value."));
    return;
  }
  std::string endVertexId = m_ui->m_endVertexLineEdit->text().toUtf8().data();

//get selected raster layer
  QVariant varRasterLayer = m_ui->m_inputRasterLayerComboBox->itemData(m_ui->m_inputRasterLayerComboBox->currentIndex(), Qt::UserRole);
  te::map::AbstractLayerPtr rasterLayer = varRasterLayer.value<te::map::AbstractLayerPtr>();

  te::rst::Raster* raster = te::map::GetRaster(rasterLayer.get());

  if (!raster)
  {
    QMessageBox::information(this, tr("Warning"), tr("Error getting Raster from Layer."));
    return;
  }

//get selected graph layer
  QVariant varLayer = m_ui->m_inputGraphLayerComboBox->itemData(m_ui->m_inputGraphLayerComboBox->currentIndex(), Qt::UserRole);
  te::map::AbstractLayerPtr l = varLayer.value<te::map::AbstractLayerPtr>();

  te::graph::Layer* gLayer = dynamic_cast<te::graph::Layer*>(l.get());

  if (!gLayer)
  {
    QMessageBox::information(this, tr("Warning"), tr("Error getting Graph Layer."));
    return;
  }

  te::graph::AbstractGraph* inputGraph = gLayer->getGraph();

  //progress
  te::qt::widgets::ProgressViewerDialog v(this);
  int id = te::common::ProgressManager::getInstance().addViewer(&v);

  QApplication::setOverrideCursor(Qt::WaitCursor);

  te::hidro::DelimitationGraph dg;

  try
  {
    dg.execute(raster, inputGraph, endVertexId, path);
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

  accept();
}

void te::hidro::DelimitationGraphDialog::onTargetFileToolButtonPressed()
{
  m_ui->m_newLayerNameLineEdit->clear();
  m_ui->m_repositoryLineEdit->clear();

  QString fileName = QFileDialog::getSaveFileName(this, tr("Save as..."), QString(), te::qt::widgets::GetDiskRasterFileSelFilter(true), 0, QFileDialog::DontConfirmOverwrite);

  if (fileName.isEmpty())
    return;

  boost::filesystem::path outfile(fileName.toUtf8().data());
  std::string aux = outfile.leaf().string();
  m_ui->m_newLayerNameLineEdit->setText(aux.c_str());
  aux = outfile.string();

  m_ui->m_repositoryLineEdit->setText(aux.c_str());
  m_ui->m_newLayerNameLineEdit->setEnabled(false);
}

void te::hidro::DelimitationGraphDialog::onVertexPickToolButtonClicked()
{
  if (m_ui->m_inputGraphLayerComboBox->currentText().isEmpty())
  {
    QMessageBox::information(this, tr("Warning"), tr("Define the Graph Layer."));
    return;
  }

  te::qt::widgets::ExtentAcquire* ea = new te::qt::widgets::ExtentAcquire(m_mapDisplay, Qt::ArrowCursor);

  m_mapDisplay->setCurrentTool(ea);

  connect(ea, SIGNAL(extentAcquired(te::gm::Envelope)), this, SLOT(onExtentAcquired(te::gm::Envelope)));
}

void te::hidro::DelimitationGraphDialog::onExtentAcquired(te::gm::Envelope envelope)
{
  QVariant varLayer = m_ui->m_inputGraphLayerComboBox->itemData(m_ui->m_inputGraphLayerComboBox->currentIndex(), Qt::UserRole);
  te::map::AbstractLayerPtr l = varLayer.value<te::map::AbstractLayerPtr>();
  te::graph::Layer* gLayer = dynamic_cast<te::graph::Layer*>(l.get());

  te::graph::AbstractGraph* inputGraph = gLayer->getGraph();

  te::gm::Envelope* env = new te::gm::Envelope(envelope);

  //create iterator
  te::graph::BoxIterator* it = new te::graph::BoxIterator(inputGraph, env);

  te::graph::Vertex* vertex = it->getFirstVertex();

  if (vertex)
  {
    int id = vertex->getId();

    m_ui->m_endVertexLineEdit->setText(QString::number(id));
  }

  delete it;
}
