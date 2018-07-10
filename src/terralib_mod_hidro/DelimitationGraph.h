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
  \file DelimitationGraph.h

  \brief This class defines the Delimitation Graph method.

*/

#ifndef __THAPP_INTERNAL_HIDRO_DELIMITATIONGRAPH_H
#define __THAPP_INTERNAL_HIDRO_DELIMITATIONGRAPH_H

#include "Config.h"

// STL Includes
#include <vector>
#include <map>
#include <memory>
#include <set>

namespace te
{
  // Forward declarations
  namespace da    { class DataSource; }
  namespace graph { class AbstractGraph; class Vertex; class BidirectionalGraph; }
  namespace rst   { class Raster; }

  namespace hidro
  {
  
    /*!
      \file DelimitationGraph.h

      \brief This class defines the Delimitation Graph method.

    */

    class TEHIDRODEXPORT DelimitationGraph
    {
      public:

        /*! \brief Default constructor. */
        DelimitationGraph();

        /*! \brief Virtual destructor. */
        virtual ~DelimitationGraph();

        /*! \brief Get error message. */
        std::string getErrorMessage();

        /*!
          \brief Function used to execute method.

          \param inputRaster
          \param inputGraph
          \param endVertexId
          \param dsInfo         Container with data source information
          
          \return 

        */
        bool execute(te::rst::Raster* inputRaster, te::graph::AbstractGraph* inputGraph, const std::string& endVertexId, const std::string& outputhPath);

    protected:

      void run(te::graph::Vertex* v, te::graph::BidirectionalGraph* g, std::set<int>& vertexIdSet, int attrIdx, te::rst::Raster* outRaster);

      std::set<int> getPredecessors(int vertexId, te::graph::AbstractGraph* inputGraph);


      private:

        te::graph::AbstractGraph* m_inputGraph;                     //!< Graph object

        te::rst::Raster* m_inputRaster;                             //!< Input raster with LDD information

        std::string m_errorMessage;                                 //!< Error message
    };

  } // end namespace graph
} // end namespace te

#endif // __THAPP_INTERNAL_HIDRO_DELIMITATIONGRAPH_H
