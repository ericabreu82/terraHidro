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
  \file ThresholdGraphBuilder.h

  \brief This class defines the Query strategy to build a graph having
         a exist graph as input parameters.

         The query must be defined using only the terralib Query module
         elements.

*/

#ifndef __THAPP_INTERNAL_HIDRO_THRESHOLDGRAPHBUILDER_H
#define __THAPP_INTERNAL_HIDRO_THRESHOLDGRAPHBUILDER_H

#include "Config.h"

// STL Includes
#include <vector>
#include <map>
#include <memory>

namespace te
{
  // Forward declarations
  namespace da    
  { 
    class DataSource;
    class Expression;
  }

  namespace graph { class AbstractGraph; }
  namespace rst   { class Raster; }

  namespace hidro
  {
  
    /*!
      \class ThresholdGraphBuilder
   */

    class TEHIDRODEXPORT ThresholdGraphBuilder
    {
      public:

        /*! \brief Default constructor. */
        ThresholdGraphBuilder();

        /*! \brief Virtual destructor. */
        virtual ~ThresholdGraphBuilder();

        /** @name Abstract Methods
        *  Method common to each graph builder
        */
        //@{

        /*! \brief Get error message. */
        std::string getErrorMessage();

        /*! \brief Get generated graph. */
        te::graph::AbstractGraph* getGraph();

        //@}

        
        /** @name Methods
         *  Methods used by this builder
         */
        //@{

        /*!
          \brief Function used to generated the vertex id based on raster coordenate

          \param g            Inpput graph
          \param e            Expression tha defines a query
          \param dsInfo       Container with data source information
          \param graphType    Attribute used to define the output graph type
          \param gInfo        Container with graph generation parameters

          \return True if the graph was correctly generated and false in other case.

          */
        bool build(te::graph::AbstractGraph* g, te::da::Expression* e, const std::string& dsInfo, const std::string& graphType, const std::map<std::string, std::string>& gInfo);

        //@}

      private:

        std::auto_ptr<te::graph::AbstractGraph> m_graph;       //!< Graph object

        std::string m_errorMessage;                            //!< Error message
    };

  } // end namespace graph
} // end namespace te

#endif // __THAPP_INTERNAL_HIDRO_THRESHOLDGRAPHBUILDER_H
