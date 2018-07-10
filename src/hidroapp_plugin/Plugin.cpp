/*  Copyright (C) 2008-2013 National Institute For Space Research (INPE) - Brazil.

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
  \file terrahidro/src/hidroapp_plugin/Plugin.cpp

  \brief Plugin implementation for the hidroapp Qt plugin.
*/

#include "Plugin.h"
#include "../terralib_mod_hidro_qt/HidroGraphRenderer.h"
#include "DelimitationGraphAction.h"
#include "LddGraphExtractorAction.h"
#include "ThresholdGraphExtractorAction.h"

// TerraLib
#include <terralib/core/translator/Translator.h>
#include <terralib/core/logger/Logger.h>
#include <terralib/graph/Module.h>
#include <terralib/qt/af/ApplicationController.h>


// QT
#include <QMenu>
#include <QMenuBar>


te::qt::plugins::thapp::Plugin::Plugin(const te::core::PluginInfo& pluginInfo)
  : te::core::CppPlugin(pluginInfo), m_menu(0)
{
}

te::qt::plugins::thapp::Plugin::~Plugin()
{
}

void te::qt::plugins::thapp::Plugin::startup()
{
  if(m_initialized)
    return;

  //initialize graph module
  te::graph::Module::initialize();

  te::qt::af::AppCtrlSingleton::getInstance().addListener(this, te::qt::af::SENDER);

  TE_LOG_TRACE(TE_TR("TH-APP Plugin startup!"));

// add plugin menu
  m_menu = te::qt::af::AppCtrlSingleton::getInstance().getMenu("HidroApp");
  m_menu->setTitle(TE_TR("HidroApp"));

  m_hidroMenu = new QMenu(m_menu);
  m_hidroMenu->setTitle(TE_TR("Hidro"));
  m_menu->addMenu(m_hidroMenu);

  m_appMenu = new QMenu(m_menu);
  m_appMenu->setTitle(TE_TR("App"));
  m_menu->addMenu(m_appMenu);

// register actions
  registerActions();

  m_initialized = true;
}

void te::qt::plugins::thapp::Plugin::shutdown()
{
  if(!m_initialized)
    return;

// remove menu
  delete m_menu;

// unregister actions
  unRegisterActions();

  TE_LOG_TRACE(TE_TR("TH-APP Plugin shutdown!"));

  m_initialized = false;
}

void te::qt::plugins::thapp::Plugin::registerActions()
{
  m_lddExtractor = new te::qt::plugins::thapp::LddGraphExtractorAction(m_hidroMenu);
  connect(m_lddExtractor, SIGNAL(triggered(te::qt::af::evt::Event*)), SIGNAL(triggered(te::qt::af::evt::Event*)));

  m_thresholdExtractor = new te::qt::plugins::thapp::ThresholdGraphExtractorAction(m_hidroMenu);
  connect(m_thresholdExtractor, SIGNAL(triggered(te::qt::af::evt::Event*)), SIGNAL(triggered(te::qt::af::evt::Event*)));

  m_delimitationGraph = new te::qt::plugins::thapp::DelimitationGraphAction(m_hidroMenu);
  connect(m_delimitationGraph, SIGNAL(triggered(te::qt::af::evt::Event*)), SIGNAL(triggered(te::qt::af::evt::Event*)));

  te::hidro::HidroGraphRendererFactory::initialize();
}

void  te::qt::plugins::thapp::Plugin::unRegisterActions()
{
  delete m_lddExtractor;

  delete m_thresholdExtractor;

  delete m_delimitationGraph;

  te::hidro::HidroGraphRendererFactory::finalize();
}

TERRALIB_PLUGIN_CALL_BACK_IMPL(te::qt::plugins::thapp::Plugin)
