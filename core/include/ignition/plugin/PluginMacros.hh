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


/// \brief Register a shared library with only one plugin
///
/// Adds a function that returns a struct with info about the plugin
#define IGN_COMMON_REGISTER_SINGLE_PLUGIN(className, baseClass) \
  struct IGN_macro_must_be_used_in_global_namespace; \
  static_assert(std::is_same < IGN_macro_must_be_used_in_global_namespace, \
      ::IGN_macro_must_be_used_in_global_namespace>::value, \
      "Macro must be used in global namespace"); \
  \
  /* TODO FQN name check does not catch all cases */ \
  /* cppcheck-suppress */ \
  static_assert(std::is_same<baseClass, ::baseClass>::value, \
      #baseClass " must be fully qualified like ::ns::BaseClass"); \
  \
  /* TODO FQN name check does not catch all cases */ \
  static_assert(std::is_same<className, ::className>::value, \
      #className " must be fully qualified like ::ns::MyClass"); \
  \
  static_assert(std::is_base_of<baseClass, className>::value, \
      #className " must inherit from " #baseClass); \
  \
  static_assert(!std::is_same<className, baseClass>::value, \
      "Class and Base class must be different"); \
  \
  extern "C" IGN_PLUGIN_VISIBLE const \
  std::size_t IGNCOMMONSinglePluginInfo(void *outputInfo, std::size_t size) \
  { \
    if (size == sizeof(ignition::common::PluginInfo)) \
    { \
      ignition::common::PluginInfo *plugin = \
          static_cast<ignition::common::PluginInfo*>(outputInfo); \
      plugin->name = #className; \
      plugin->interface = #baseClass; \
      plugin->baseClassHash = typeid(baseClass).hash_code(); \
      plugin->factory = []() { \
        return static_cast<void*>( new className()); \
      }; \
      return size; \
    } \
    return 0; \
  }; \
  \
  extern "C" IGN_PLUGIN_VISIBLE const \
  std::size_t IGNCOMMONSinglePluginInfoSize = \
    sizeof(ignition::common::PluginInfo); \
  \
  extern "C" IGN_PLUGIN_VISIBLE const \
  int IGNCOMMONPluginAPIVersion = \
    ignition::common::PLUGIN_API_VERSION;

/// TODO macro supporting multiple plugins per shared library

#endif
