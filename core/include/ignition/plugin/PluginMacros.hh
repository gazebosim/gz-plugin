/*
 * Copyright (C) 2017 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/


#ifndef IGNITION_COMMON_REGISTERMACROS_HH_
#define IGNITION_COMMON_REGISTERMACROS_HH_

#include <typeinfo>
#include <type_traits>
#include "ignition/common/PluginInfo.hh"


#if defined _WIN32 || defined __CYGWIN__
  #ifdef __GNUC__
    #define IGN_PLUGIN_VISIBLE __attribute__ ((dllexport))
  #else
    #define IGN_PLUGIN_VISIBLE __declspec(dllexport)
  #endif
#else
  #if __GNUC__ >= 4
    #define IGN_PLUGIN_VISIBLE __attribute__ ((visibility ("default")))
  #else
    #define IGN_PLUGIN_VISIBLE
  #endif
#endif


/// \brief Register the PluginInfo meta data
#define IGN_COMMON_REGISTER_PLUGININFO_META_DATA\
  extern "C" IGN_PLUGIN_VISIBLE const \
  std::size_t IGNCOMMONSinglePluginInfoSize = \
    sizeof(ignition::common::PluginInfo); \
  \
  extern "C" IGN_PLUGIN_VISIBLE const \
  int IGNCOMMONPluginAPIVersion = \
    ignition::common::PLUGIN_API_VERSION;


/// \brief Begin registering a set of plugins that are contained within this
/// shared library. Use a sequence of calls to the macro IGN_COMMON_ADD_PLUGIN(),
/// passing in a different class name to each call. When all the classes have
/// been added, call IGN_COMMON_FINISH_ADDING_PLUGINS.
///
/// Be sure to only use this macro in the global namespace, and only use it once.
#define IGN_COMMON_BEGIN_ADDING_PLUGINS\
  IGN_COMMON_REGISTER_PLUGININFO_META_DATA\
  struct IGN_macro_must_be_used_in_global_namespace; \
  static_assert(std::is_same < IGN_macro_must_be_used_in_global_namespace, \
      ::IGN_macro_must_be_used_in_global_namespace>::value, \
      "Macro for registering plugins must be used in global namespace"); \
  extern "C" IGN_PLUGIN_VISIBLE const\
  std::size_t IGNCOMMONMultiPluginInfo(\
      void *_outputInfo, const std::size_t _pluginId, const std::size_t _size)\
  {\
    if (_size != sizeof(ignition::common::PluginInfo))\
    {\
      return 0;\
    }\
    std::size_t pluginCount = 0;
// TODO Should we print some kind of error in the event that the size does not
// match? Otherwise it may be difficult to debug cases where
// IGNCOMMONMultiPluginInfo seems to return 0 for no apparent reason.

/// \brief Add a plugin from this shared library. This macro could be called
/// consecutively on each class that this shared library wants to provide as a
/// plugin. This macro must be called in between IGN_COMMON_BEGIN_ADDING_PLUGINS
/// and IGN_COMMON_FINISH_ADDING_PLUGINS.
#define IGN_COMMON_ADD_PLUGIN(className)\
    /* cppcheck-suppress */ \
    static_assert(std::is_same<className, ::className>::value, \
        #className " must be fully qualified like ::ns::MyClass"); \
    \
    if(_pluginId == pluginCount)\
    {\
      ignition::common::PluginInfo *plugin =\
          static_cast<ignition::common::PluginInfo*>(_outputInfo);\
      plugin->name = #className;\
      const std::array<const char*, className :: PluginInterfaces.size()> interfaces\
          = className :: PluginInterfaces;\
      plugin->interfaces.reserve(interfaces.size());\
      for (const char* interface : interfaces)\
        plugin->interfaces.insert(interface);\
      plugin->factory = []() {\
        return static_cast<void*>( new className() );\
      };\
    }\
    ++pluginCount;

/// \brief Call this macro after all calls to IGN_COMMON_ADD_PLUGIN have been
/// finished.
#define IGN_COMMON_FINISH_ADDING_PLUGINS\
    if(pluginId > pluginCount)\
      return 0;\
    return pluginCount - pluginId;\
  };


/// \brief Register a shared library with only one plugin
///
/// Adds a function that returns a struct with info about the plugin
#define IGN_COMMON_REGISTER_SINGLE_PLUGIN(className) \
  IGN_COMMON_BEGIN_ADDING_PLUGINS\
  IGN_COMMON_ADD_PLUGIN(className)\
  IGN_COMMON_FINISH_ADDING_PLUGINS

#endif
