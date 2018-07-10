cmake_minimum_required(VERSION 2.8.12)

project(th_app)

cmake_policy(SET CMP0020 NEW)

if(${CMAKE_MAJOR_VERSION} GREATER 2)
  cmake_policy(SET CMP0007 NEW)
  cmake_policy(SET CMP0026 OLD)
  cmake_policy(SET CMP0042 NEW)  
  cmake_policy(SET CMP0043 NEW)  
  cmake_policy(SET CMP0053 OLD)  
endif()

set(TH_APP_ABSOLUTE_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../..)

set_directory_properties(PROPERTIES CLEAN_NO_CUSTOM 1)

include_directories(${CMAKE_BINARY_DIR})

if(MSVC)
  set(CMAKE_DEBUG_POSTFIX "d")
endif()

#
# main variables for the build script
#

# variables that control the system versioning
set(TH_APP_VERSION_MINOR 0)
set(TH_APP_VERSION_MAJOR 5)
set(TH_APP_VERSION_PATCH 0)
set(TH_APP_VERSION_STATUS "rc")
set(TH_APP_VERSION_STRING "${TH_APP_VERSION_MAJOR}.${TH_APP_VERSION_MINOR}.${TH_APP_VERSION_PATCH}-${TH_APP_VERSION_STATUS}")
set(TH_APP_VERSION "0x050000")
set(TH_APP_PKG_VERSION ${TH_APP_VERSION_MAJOR}.${TH_APP_VERSION_MINOR}.${TH_APP_VERSION_PATCH})

find_package(terralib 5.2 REQUIRED)

find_package(Boost REQUIRED)

find_package(Qt5 5.1 REQUIRED COMPONENTS Core Gui Widgets PrintSupport)

find_package(Qt5LinguistTools REQUIRED COMPONENTS lupdate lrelease)

add_definitions(-D_SCL_SECURE_NO_WARNINGS -DBOOST_ALL_NO_LIB -DBOOST_FILESYSTEM_VERSION=3 -DBOOST_UBLAS_TYPE_CHECK=0)

set(CMAKE_AUTOMOC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(TH_APP_BASE_DESTINATION_DIR "")

set (TE_3RDPARTY "$ENV{TE_3RDPARTY}" CACHE PATH "Where to find TerraLib 3rdParty?")

if(NOT DEFINED TH_APP_DESTINATION_LIBRARY)
  set(TH_APP_DESTINATION_LIBRARY "${TH_APP_BASE_DESTINATION_DIR}lib")
endif()

if(NOT DEFINED TH_APP_DESTINATION_RUNTIME)
  set(TH_APP_DESTINATION_RUNTIME "${TH_APP_BASE_DESTINATION_DIR}bin")
endif()

if(NOT DEFINED TH_APP_DESTINATION_ARCHIVE)
  set(TH_APP_DESTINATION_ARCHIVE "${TH_APP_BASE_DESTINATION_DIR}lib")
endif()

if(NOT DEFINED TH_APP_DESTINATION_HEADERS)
  set(TH_APP_DESTINATION_HEADERS "${TH_APP_BASE_DESTINATION_DIR}include")
endif()

if(NOT DEFINED TH_APP_DESTINATION_SHARE)
  set(TH_APP_DESTINATION_SHARE "${TH_APP_BASE_DESTINATION_DIR}share")
endif()

include_directories(${terralib_DIR}
          ${terralib_INCLUDE_DIRS}
          ${terralib_INCLUDE_DIRS}/terralib
		  ${Boost_INCLUDE_DIR}
          )

 add_subdirectory(terralib_mod_hidro)
 add_subdirectory(terralib_mod_hidro_qt)
 add_subdirectory(hidroapp_plugin)

# Track dependencies
TeInstallQt5Plugins()

if(WIN32)
  get_filename_component(_teShare ${terralib_INCLUDE_DIRS}/../share/terralib DIRECTORY) 
  get_filename_component(_qtPlugins ${terralib_INCLUDE_DIRS}/../qtplugins/platforms DIRECTORY) 

  install (
    FILES ${_teplugins} ${_dep}
    DESTINATION bin
    COMPONENT runtime
  )  
  
  install(
    DIRECTORY ${_teShare}
    DESTINATION .
    COMPONENT runtime
  )
  
  file(GLOB _teBins ${terralib_DIR}/../../../bin/*.dll)
  
  install(
    FILES ${_teBins}
    DESTINATION bin
    COMPONENT runtime
  )
endif()


include(InstallRequiredSystemLibraries)

# CPack
set(CPACK_GENERATOR NSIS)
set(CPACK_PACKAGE_NAME "TerraHidroApp")
set(CPACK_PACKAGE_VENDOR "Terralib3rdParty")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "TH-APP")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "TH-APP")
SET(CPACK_NSIS_MODIFY_PATH OFF)

#for the sortcuts
set(shortCutName "hidro_app_${CPACK_PACKAGE_VERSION_MAJOR}_${CPACK_PACKAGE_VERSION_MINOR}_${CPACK_PACKAGE_VERSION_PATCH}${bitName}")

set(CPACK_PACKAGE_EXECUTABLES "TerraHidroApp;${shortCutName}")
  
set(CPACK_NSIS_CREATE_ICONS_EXTRA "SetOutPath \\\"$INSTDIR\\\\bin\\\";CreateShortCut \\\"$SMPROGRAMS\\\\$STARTMENU_FOLDER/${shortCutName}.lnk\\\" \\\"$INSTDIRb\\\\bin\\\\TerraHidroApp.exe\\\" \\\"\\\" \\\"$INSTDIR\\\\share\\\\terralib\\\\images\\\\ico\\\\terralib-globe.ico\\\";CreateShortCut \\\"$DESKTOP\\\\${shortCutName}.lnk\\\" \\\"$INSTDIR\\\\bin\\\\TerraHidroApp.exe\\\" \\\"\\\" \\\"$INSTDIR\\\\share\\\\terralib\\\\images\\\\ico\\\\terralib-globe.ico\\\"")

list (APPEND CPACK_NSIS_DELETE_ICONS_EXTRA "Delete \\\"$SMPROGRAMS\\\\$MUI_TEMP\\\\${shortCutName}.lnk\\\";Delete \\\"$DESKTOP\\\\${shortCutName}.lnk\\\"")

INCLUDE(CPack)

