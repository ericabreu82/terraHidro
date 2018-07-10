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

#ifndef __THAPP_INTERNAL_HIDRO_QT_DELIMITATIONGRAPHDIALOG_H
#define __THAPP_INTERNAL_HIDRO_QT_DELIMITATIONGRAPHDIALOG_H

#include "Config.h"

// TerraLib
#include <terralib/dataaccess/datasource/DataSourceInfo.h>
#include <terralib/geometry/Envelope.h>
#include <terralib/maptools/AbstractLayer.h>
#include <terralib/qt/widgets/canvas/MapDisplay.h>

// Qt
#include <QDialog>

namespace Ui { class DelimitationGraphDialogForm; }

namespace te
{
  namespace hidro
  {
    class TEHIDROQTEXPORT DelimitationGraphDialog : public QDialog
    {
      Q_OBJECT

      public:

        DelimitationGraphDialog(QWidget* parent = 0, Qt::WindowFlags f = 0);

        ~DelimitationGraphDialog();

        /*!
          \brief Set the layer that can be used

          \param layers   List of AbstractLayerPtr
        */
        void setLayers(std::list<te::map::AbstractLayerPtr> layers);

        te::map::AbstractLayerPtr getOutputLayer();

        void setMapDisplay(te::qt::widgets::MapDisplay* mapDisplay);

      protected slots:

        void onOkPushButtonClicked();

        void onTargetFileToolButtonPressed();

        void onVertexPickToolButtonClicked();

        void onExtentAcquired(te::gm::Envelope envelope);

      private:

        std::auto_ptr<Ui::DelimitationGraphDialogForm> m_ui;

        te::map::AbstractLayerPtr m_outputLayer;                                          //!< Generated Layer.

        te::qt::widgets::MapDisplay* m_mapDisplay;
    };
  }   // end namespace sa
}     // end namespace te

#endif  // __THAPP_INTERNAL_HIDRO_QT_DELIMITATIONGRAPHDIALOG_H
