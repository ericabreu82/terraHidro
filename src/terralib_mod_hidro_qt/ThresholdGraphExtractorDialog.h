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
\file terrahidro/src/hidro_qt/ThresholdraphExtractorDialog.h

\brief This file defines the UI for Threshold Graph extractor.
*/

#ifndef __THAPP_INTERNAL_HIDRO_QT_THRESHOLDGRAPHEXTRACTORDIALOG_H
#define __THAPP_INTERNAL_HIDRO_QT_THRESHOLDGRAPHEXTRACTORDIALOG_H

#include "Config.h"

// TerraLib
#include <terralib/dataaccess/datasource/DataSourceInfo.h>
#include <terralib/maptools/AbstractLayer.h>

// Qt
#include <QDialog>

namespace Ui { class ThresholdGraphExtractorDialogForm; }

namespace te
{
  namespace hidro
  {
    class TEHIDROQTEXPORT ThresholdGraphExtractorDialog : public QDialog
    {
      Q_OBJECT

      public:

        ThresholdGraphExtractorDialog(QWidget* parent = 0, Qt::WindowFlags f = 0);

        ~ThresholdGraphExtractorDialog();

        /*!
          \brief Set the layer that can be used

          \param layers   List of AbstractLayerPtr
        */
        void setLayers(std::list<te::map::AbstractLayerPtr> layers);

        te::map::AbstractLayerPtr getOutputLayer();

      protected slots:

        void onOkPushButtonClicked();

        void onTargetDatasourceToolButtonPressed();

        void onTargetFileToolButtonPressed();

        void onInputLayerComboBoxActivated(int index);

      private:

        std::auto_ptr<Ui::ThresholdGraphExtractorDialogForm> m_ui;

        te::da::DataSourceInfoPtr m_outputDatasource;

        te::map::AbstractLayerPtr m_outputLayer;                                          //!< Generated Layer.

        bool m_toFile;
    };
  }   // end namespace sa
}     // end namespace te

#endif  // __THAPP_INTERNAL_HIDRO_QT_THRESHOLDGRAPHEXTRACTORDIALOG_H
