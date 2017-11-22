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


#ifndef IGNITION_COMMON_DETAIL_REGISTERPLUGIN_HH_
#define IGNITION_COMMON_DETAIL_REGISTERPLUGIN_HH_

#include <string>
#include <typeinfo>
#include <type_traits>
#include <unordered_set>
#include <ignition/common/PluginInfo.hh>
#include <ignition/common/SuppressWarning.hh>


#if defined _WIN32 || defined __CYGWIN__
  #ifdef __GNUC__
    #define DETAIL_IGN_PLUGIN_VISIBLE __attribute__ ((dllexport))
  #else
    #define DETAIL_IGN_PLUGIN_VISIBLE __declspec(dllexport)
  #endif
#else
  #if __GNUC__ >= 4
    #define DETAIL_IGN_PLUGIN_VISIBLE __attribute__ ((visibility ("default")))
  #else
    #define DETAIL_IGN_PLUGIN_VISIBLE
  #endif
#endif

extern "C"
{
  inline void DETAIL_IGN_PLUGIN_VISIBLE IGNCOMMONInputOrOutputPluginInfo(
      const void *_inputSingleInfo, const void ** const _outputAllInfo,
      int *_inputAndOutputAPIVersion,
      std::size_t *_inputAndOutputPluginInfoSize,
      std::size_t *_inputAndOutputPluginInfoAlign)
  {
    using PluginInfoMap = ignition::common::PluginInfoMap;
    static PluginInfoMap pluginMap;

    if (_inputSingleInfo)
    {
      const ignition::common::PluginInfo *input =
          static_cast<const ignition::common::PluginInfo*>(_inputSingleInfo);

      PluginInfoMap::iterator it;
      bool inserted;
      std::tie(it, inserted) =
          pluginMap.insert(std::make_pair(input->name, *input));

      if (!inserted)
      {
        // If the object was not inserted, then an entry already existed for
        // this plugin type. We should still insert each of the interface map
        // entries provided by the input info, just in case any of them are
        // missing from the currently existing entry. This allows the user to
        // specify different interfaces for the same plugin type using different
        // macros in different locations or different translation units.
        ignition::common::PluginInfo &entry = it->second;
        for(const auto &interfaceMapEntry : input->interfaces)
          entry.interfaces.insert(interfaceMapEntry);
      }
    }

    if (_outputAllInfo)
    {
      if (nullptr == _inputAndOutputAPIVersion ||
          nullptr == _inputAndOutputPluginInfoSize ||
          nullptr == _inputAndOutputPluginInfoAlign)
      {
        // This should never happen, or else the function is being misused.
        return;
      }

      bool agreement = true;

      if (ignition::common::PLUGIN_API_VERSION
          != *_inputAndOutputAPIVersion)
      {
        agreement = false;
      }

      if (sizeof(ignition::common::PluginInfo)
          != *_inputAndOutputPluginInfoSize)
      {
        agreement = false;
      }

      if (alignof(ignition::common::PluginInfo)
          != *_inputAndOutputPluginInfoAlign)
      {
        agreement = false;
      }

      *_inputAndOutputAPIVersion = ignition::common::PLUGIN_API_VERSION;
      *_inputAndOutputPluginInfoSize = sizeof(ignition::common::PluginInfo);
      *_inputAndOutputPluginInfoAlign = alignof(ignition::common::PluginInfo);

      // If the size, alignment, or API do not agree, we should return without
      // outputting any of the plugin info; otherwise, we could get a
      // segmentation fault.
      //
      // We will return the current API version to the PluginLoader, and it may
      // then decide to attempt the call to this function again with the correct
      // API version if it supports backwards/forwards compatibility.
      if(!agreement)
        return;

      *_outputAllInfo = &pluginMap;
    }
  }
}

namespace ignition
{
  namespace common
  {
    namespace detail
    {

      template <typename PluginClass, typename Interface>
      void RegisterPlugin(const std::string &_pluginName)
      {
        PluginInfo info;

        // Set the name of the plugin
        info.name = _pluginName;

        // Create a factory for generating new plugin instances
        info.factory = []() {
          return static_cast<void*>(new PluginClass);
        };

        IGN_COMMON_BEGIN_WARNING_SUPPRESSION(IGN_COMMON_DELETE_NON_VIRTUAL_DESTRUCTOR)
        // Create a deleter to clean up destroyed instances
        info.deleter = [](void *ptr) {
          delete static_cast<PluginClass*>(ptr);
        };
        IGN_COMMON_FINISH_WARNING_SUPPRESSION(IGN_COMMON_DELETE_NON_VIRTUAL_DESTRUCTOR)

        // Provide a map from the plugin to its interface
        info.interfaces.insert(std::make_pair(
              typeid(Interface).name(),
              [=](void* v_ptr) {
                  PluginClass *d_ptr = static_cast<PluginClass*>(v_ptr);
                  return static_cast<Interface*>(d_ptr);
              }));

        // Send this information as input to this library's global repository
        // of plugins.
        IGNCOMMONInputOrOutputPluginInfo(
              &info, nullptr, nullptr, nullptr, nullptr);
      }
    }
  }
}


#define DETAIL_IGN_COMMON_ADD_PLUGIN_HELPER(UniqueID, pluginName, interface) \
  \
  /* Print out a clear error when the plugin class is pure abstract (which */ \
  /* would make it impossible to load as a plugin). The compiler prevents */ \
  /* this from being an issue, but its printout might be difficult for */ \
  /* users to interpret, so we provide a very explicit explanation here. */ \
  static_assert(!std::is_abstract<pluginName>::value, \
      "[" #pluginName "] must not be an abstract class. It contains at least " \
      "one pure virtual function!"); \
  \
  /* Print out a clear error when the plugin does not actually provide the */ \
  /* specified interface. */ \
  static_assert(std::is_base_of<interface, pluginName>::value, \
      "[" #interface "] is not a base class of [" #pluginName "], so it " \
      "cannot be used as a plugin interface for [" #pluginName "]!"); \
  \
  namespace ignition \
  { \
    namespace common \
    { \
      namespace \
      { \
        struct ExecuteWhenLoadingLibrary##UniqueID \
        { \
          ExecuteWhenLoadingLibrary##UniqueID() \
          { \
            ignition::common::detail::RegisterPlugin<pluginName, interface>( \
              #pluginName); \
          } \
        }; \
  \
        static ExecuteWhenLoadingLibrary##UniqueID execute##UniqueID; \
      } \
    } \
  }


/// This macro is needed to force the __COUNTER__ macro to expand to a value
/// before being passed to the *_HELPER macro.
#define DETAIL_IGN_COMMON_ADD_PLUGIN_WITH_COUNTER(UniqueID, pluginName, interface) \
  DETAIL_IGN_COMMON_ADD_PLUGIN_HELPER(UniqueID, pluginName, interface)


/// We use the __COUNTER__ here to give each plugin instantiation its own unique
/// name, which is required in order to statically initialize each one.
#define DETAIL_IGN_COMMON_ADD_PLUGIN(pluginName, interface) \
  DETAIL_IGN_COMMON_ADD_PLUGIN_WITH_COUNTER(__COUNTER__, pluginName, interface)


#endif
