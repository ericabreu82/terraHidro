#
#  Copyright (C) 2008-2014 National Institute For Space Research (INPE) - Brazil.
#
#  This file is part of the TerraLib - a Framework for building GIS enabled applications.
#
#  TerraLib is free software: you can redistribute it and/or modify
#  it under the terms of the GNU Lesser General Public License as published by
#  the Free Software Foundation, either version 3 of the License,
#  or (at your option) any later version.
#
#  TerraLib is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#  GNU Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public License
#  along with TerraLib. See COPYING. If not, write to
#  TerraLib Team at <terralib-team@terralib.org>.
#
#
#  Description: Build configuration for the hidro qt Module.
#
#  Author: 	Eric Silva Abreu <eric.abreu@dpi.inpe.br>
#          

if(WIN32)
  add_definitions(-D_SCL_SECURE_NO_WARNINGS -DTEHIDROQTDLL -D_CRT_SECURE_NO_WARNINGS -DBOOST_LOG_DYN_LINK)
endif()

include_directories(
  ${TH_APP_ABSOLUTE_ROOT_DIR}/src
  ${terralib_INCLUDE_DIRS}
  ${terralib_DIR}
  ${Boost_INCLUDE_DIR}
  ${Boost_INCLUDE_DIR}/qwt
)

if(Qt5_FOUND)
  set(CMAKE_AUTOMOC ON)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

file(GLOB HIDRO_SRC_FILES ${TH_APP_ABSOLUTE_ROOT_DIR}/src/terralib_mod_hidro_qt/*.cpp)
file(GLOB HIDRO_HDR_FILES ${TH_APP_ABSOLUTE_ROOT_DIR}/src/terralib_mod_hidro_qt/*.h)
file(GLOB HIDRO_UI_FILES ${TH_APP_ABSOLUTE_ROOT_DIR}/src/terralib_mod_hidro_qt/ui/*.ui)

if(Qt5_FOUND)
  QT5_WRAP_UI(HIDRO_UI_GEN_FILES ${HIDRO_UI_FILES})
endif()

source_group("Source Files"  FILES ${HIDRO_SRC_FILES})
source_group("Header Files"  FILES ${HIDRO_HDR_FILES})

add_library(terralib_mod_hidro_qt SHARED ${HIDRO_SRC_FILES} ${HIDRO_HDR_FILES} ${HIDRO_UI_GEN_FILES})

set(HIDRO_LIBRARIES_DEPENDENCIES "terralib_mod_hidro")
list(APPEND HIDRO_LIBRARIES_DEPENDENCIES "terralib_mod_qt_widgets")

target_link_libraries(terralib_mod_hidro_qt ${HIDRO_LIBRARIES_DEPENDENCIES})

set_target_properties(terralib_mod_hidro_qt
                      PROPERTIES VERSION ${TH_APP_VERSION_MAJOR}.${TH_APP_VERSION_MINOR}
                                 SOVERSION ${TH_APP_VERSION_MAJOR}.${TH_APP_VERSION_MINOR})
								 
if(Qt5_FOUND)
  qt5_use_modules(terralib_mod_hidro_qt Widgets)
endif()

install(TARGETS terralib_mod_hidro_qt
        EXPORT th_app-targets
        RUNTIME DESTINATION ${TH_APP_DESTINATION_RUNTIME} COMPONENT runtime
        LIBRARY DESTINATION ${TH_APP_DESTINATION_LIBRARY} COMPONENT runtime
        ARCHIVE DESTINATION ${TH_APP_DESTINATION_ARCHIVE} COMPONENT runtime)

install(FILES ${HIDRO_HDR_FILES}
        DESTINATION ${HIDRO_DESTINATION_HEADERS}/terralib_mod_hidro_qt COMPONENT devel)

export(TARGETS terralib_mod_hidro_qt APPEND FILE ${CMAKE_BINARY_DIR}/th_app-exports.cmake)
