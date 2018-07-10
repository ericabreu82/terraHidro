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
  \file terrahidro/src/hidroapp_plugin/LddGraphExtractorAction.h

  \brief This file defines the LddGraphExtractorAction class.
*/

#ifndef __THAPP_INTERNAL_PLUGIN_LDDGRAPHEXTRACTORACTION_H
#define __THAPP_INTERNAL_PLUGIN_LDDGRAPHEXTRACTORACTION_H

// Fiocruz
#include "AbstractAction.h"
#include "Config.h"

namespace te
{
  namespace qt
  {
    namespace plugins
    {
      namespace thapp
      {
        /*!
          \class LddGraphExtractorAction
          
          \brief This file defines the LddGraphExtractorAction class.

        */
        class HIDROAPPDLLEXPORT LddGraphExtractorAction : public te::qt::plugins::thapp::AbstractAction
        {
          Q_OBJECT

          public:

            LddGraphExtractorAction(QMenu* menu);

            virtual ~LddGraphExtractorAction();

          protected slots:

            virtual void onActionActivated(bool checked);
        };

      } // end namespace thapp
    }   // end namespace plugins
  }     // end namespace qt
}       // end namespace te

#endif //__THAPP_INTERNAL_PLUGIN_LDDGRAPHEXTRACTORACTION_H
