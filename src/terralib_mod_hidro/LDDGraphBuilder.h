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
  \file LDDGraphBuilder.h

  \brief This class defines the LDD strategy to build a graph.

        This strategy is based on Serio Rosim method, using this
        "mask" is possible extract a graph from a LDD image.

                    -------------------
                    |  32 |  64 | 128 |
                    -------------------
                    |  16 |  *  |  1  |
                    -------------------
                    |  8  |  4  |  2  |
                    -------------------

*/

#ifndef __THAPP_INTERNAL_HIDRO_LDDGRAPHBUILDER_H
#define __THAPP_INTERNAL_HIDRO_LDDGRAPHBUILDER_H

#include "Config.h"

// STL Includes
#include <vector>
#include <map>
#include <memory>

namespace te
{
  // Forward declarations
  namespace da    { class DataSource; }
  namespace graph { class AbstractGraph; }
  namespace rst   { class Raster; }

  namespace hidro
  {
  
    /*!
      \class LDDGraphBuilder

      \brief This strategy is based on Serio Rosim method, using this
            "mask" is possible extract a graph from a LDD image.

                            -------------------
                            |  32 |  64 | 128 |
                            -------------------
                            |  16 |  *  |  1  |
                            -------------------
                            |  8  |  4  |  2  |
                            -------------------

      \sa AbstractGraphBuilder
    */

    class TEHIDRODEXPORT LDDGraphBuilder
    {
      public:

        /*! \brief Default constructor. */
        LDDGraphBuilder();

        /*! \brief Virtual destructor. */
        virtual ~LDDGraphBuilder();

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
          \brief Function used to build the output graph based on input parameters.

          \param raster
          \param dsInfo         Container with data source information
          \param graphType      Attribute used to define the output graph type
          \param gInfo          Container with graph generation parameters

          \return True if the graph was correctly generated and false in other case.

        */
         bool build(te::rst::Raster* raster, const std::string& dsInfo, const std::string& graphType, const std::map<std::string, std::string>& gInfo);

        //@}

      protected:

        /*!
          \brief Function used to generated the vertex id based on raster coordenate

          \param row    Row position over the raster
          \param col    Column position over the raster
          \param id     Used to set the id generated

          \return True if a valid id was generated and false in other case

        */
        bool getVertexId(int row, int col, int& id);

        /*!
          \brief Function used to generated the edge id

          \return Integer value as ID

        */
        int getEdgeId();

      private:

        std::auto_ptr<te::graph::AbstractGraph> m_graph;       //!< Graph object

        std::string m_errorMessage;                            //!< Error message

        te::rst::Raster* m_raster;                             //!< Input raster with LDD information

        int m_edgeId;                                          //!< Attribute used as a index counter for edge objects
    };

  } // end namespace graph
} // end namespace te

#endif // __THAPP_INTERNAL_HIDRO_LDDGRAPHBUILDER_H
