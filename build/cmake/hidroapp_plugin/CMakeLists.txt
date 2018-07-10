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
#  Description: Build configuration for the hiro app plugin Module.
#
#  Author: 	Eric Silva Abreu <eric.abreu@dpi.inpe.br>
#          

if(WIN32)
  add_definitions(-D_SCL_SECURE_NO_WARNINGS -DHIDROAPPDLL -D_CRT_SECURE_NO_WARNINGS -DBOOST_LOG_DYN_LINK)
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

file(GLOB HIDROAPP_SRC_FILES ${TH_APP_ABSOLUTE_ROOT_DIR}/src/hidroapp_plugin/*.cpp)
file(GLOB HIDROAPP_HDR_FILES ${TH_APP_ABSOLUTE_ROOT_DIR}/src/hidroapp_plugin/*.h)
file(GLOB HIDROAPP_RESOURCES_FILES ${TH_APP_ABSOLUTE_ROOT_DIR}/resources/*.qrc)

source_group("Source Files"  FILES ${HIDROAPP_SRC_FILES})
source_group("Header Files"  FILES ${HIDROAPP_HDR_FILES})

set(HIDROAPP_DEPENDENCIES "terralib_mod_hidro")
list(APPEND HIDROAPP_DEPENDENCIES "terralib_mod_hidro_qt")
list(APPEND HIDROAPP_DEPENDENCIES "terralib_mod_core")
list(APPEND HIDROAPP_DEPENDENCIES "terralib_mod_qt_apf")

if(Qt5_FOUND)
  QT5_ADD_RESOURCES(HIDRO_RESOURCES ${HIDROAPP_RESOURCES_FILES})
  
  add_library(hidroAppPlugin SHARED ${HIDROAPP_SRC_FILES} ${HIDROAPP_HDR_FILES} ${HIDRO_RESOURCES})
  
  target_link_libraries(hidroAppPlugin ${HIDROAPP_DEPENDENCIES} ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY})
  
  #SET (QT_USE_QTMAIN TRUE)
  
  set_target_properties(hidroAppPlugin PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
  
  qt5_use_modules(hidroAppPlugin Widgets)
endif()

set_target_properties(hidroAppPlugin
                      PROPERTIES VERSION ${TH_APP_VERSION_MAJOR}.${TH_APP_VERSION_MINOR}
                                 SOVERSION ${TH_APP_VERSION_MAJOR}.${TH_APP_VERSION_MINOR})

install(TARGETS hidroAppPlugin
        EXPORT th_app-targets
		BUNDLE DESTINATION . COMPONENT runtime
        RUNTIME DESTINATION ${TH_APP_DESTINATION_RUNTIME} COMPONENT runtime
        LIBRARY DESTINATION ${TH_APP_DESTINATION_LIBRARY} COMPONENT runtime
        ARCHIVE DESTINATION ${TH_APP_DESTINATION_ARCHIVE} COMPONENT runtime)

install(FILES ${HIDROAPP_HDR_FILES}
        DESTINATION ${HIDRO_DESTINATION_HEADERS}/hidroAppPlugin COMPONENT devel)
		
add_custom_command(
      TARGET hidroAppPlugin
      POST_BUILD
	  COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_BINARY_DIR}/bin/Debug/hidroAppPlugind.dll" "${terralib_DIR}/debug"
	  COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_BINARY_DIR}/bin/Debug/terralib_mod_hidrod.dll" "${terralib_DIR}/debug"
	  COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_BINARY_DIR}/bin/Debug/terralib_mod_hidro_qtd.dll" "${terralib_DIR}/debug"
      
      COMMENT "Copying plugin files..."
      VERBATIM
    )
		

