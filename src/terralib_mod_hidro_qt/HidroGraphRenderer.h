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
\file terrahidro/src/hidro_qt/HidroGraphRenderer.h

\brief This file defines the Graph Renderer class
*/

#ifndef __THAPP_INTERNAL_HIDRO_QT_HIDROGRAPHRENDERER_H
#define __THAPP_INTERNAL_HIDRO_QT_HIDROGRAPHRENDERER_H

// TerraLib
#include "Config.h"

#include <terralib/color/RGBAColor.h>
#include <terralib/maptools/AbstractLayerRenderer.h>
#include <terralib/maptools/RendererFactory.h>

namespace te
{
  namespace gm
  {
    class Envelope;
    class LineString;
    class MultiLineString;
  }

  namespace se { class Mark; }

  namespace hidro
  {

    /*!
    \class HidroGraphRenderer

    \brief This file defines the Graph Renderer class
    */
    class TEHIDROQTEXPORT HidroGraphRenderer : public te::map::AbstractLayerRenderer
    {

      public:

        HidroGraphRenderer();

        virtual ~HidroGraphRenderer();

        virtual void drawFlowLine(te::map::Canvas* canvas, te::gm::LineString* line);

        virtual void drawFlowMultiLine(te::map::Canvas* canvas, te::gm::MultiLineString* line);

        virtual void drawDatSetGeometries(te::da::DataSet* dataset, const std::size_t& gpos,
        te::map::Canvas* canvas, int fromSRID, int toSRID, te::map::Chart* chart, bool* cancel, te::common::TaskProgress* task = 0);

      protected:

        te::color::RGBAColor** m_pointPattern;            //!< Represents the pattern to draw a point
        te::se::Mark*          m_pointMark;               //!< Represents the mark of a point

        te::color::RGBAColor** m_arrowPattern;            //!< Represents the pattern to draw a point
        te::se::Mark*          m_arrowMark;               //!< Represents the mark of a point

    };

    class TEHIDROQTEXPORT HidroGraphRendererFactory : public te::map::RendererFactory
    {
      public:

        static void initialize();

        static void finalize();

        ~HidroGraphRendererFactory();

      protected:

        te::map::AbstractRenderer* build();

        HidroGraphRendererFactory();

      private:

        static HidroGraphRendererFactory* sm_factory; //!< A pointer to the global renderer factory.
    };
  }       // end namespace hidro
}         // end namespace te

#endif  // __THAPP_INTERNAL_HIDRO_QT_HIDROGRAPHRENDERER_H
