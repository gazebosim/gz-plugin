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


#ifndef IGNITION_PLUGIN_DETAIL_PLUGINMACROS_HH_
#define IGNITION_PLUGIN_DETAIL_PLUGINMACROS_HH_

#include <string>
#include <typeinfo>
#include <type_traits>
#include <unordered_set>
#include <utility>

#include <ignition/utilities/SuppressWarning.hh>

#include <ignition/plugin/PluginInfo.hh>


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

#define DETAIL_IGN_PLUGIN_SPECIALIZE_INTERFACE(interfaceName) \
  static_assert(std::is_same<interfaceName, ::interfaceName>::value, \
      #interfaceName " must be fully qualified like ::ns::MyClass"); \
  static constexpr const char* IGNPLUGINInterfaceName = #interfaceName;

/// \brief Register the Info meta data
#define DETAIL_IGN_PLUGIN_REGISTER_PLUGININFO_META_DATA \
  extern "C" { \
    std::size_t DETAIL_IGN_PLUGIN_VISIBLE IGNPLUGINPluginInfoSize = \
      sizeof(ignition::plugin::Info); \
    \
    std::size_t DETAIL_IGN_PLUGIN_VISIBLE IGNPLUGINPluginInfoAlignment = \
      alignof(ignition::plugin::Info); \
    \
    int DETAIL_IGN_PLUGIN_VISIBLE IGNPLUGINPluginAPIVersion = \
      ignition::plugin::IGN_PLUGIN_INFO_API_VERSION; \
  }


#define DETAIL_IGN_PLUGIN_BEGIN_ADDING_PLUGINS \
  DETAIL_IGN_PLUGIN_REGISTER_PLUGININFO_META_DATA \
IGN_UTILS_WARN_IGNORE__DELETE_NON_VIRTUAL_DESTRUCTOR \
  /* This struct attempts to make sure that the macro is being called from */ \
  /* a global namespace */ \
  struct IGN_macro_must_be_used_in_global_namespace; \
  static_assert(std::is_same < IGN_macro_must_be_used_in_global_namespace, \
      ::IGN_macro_must_be_used_in_global_namespace>::value, \
      "Macro for registering plugins must be used in global namespace"); \
  \
  /* \brief Extract PluginInfo from this dynamically loaded library */ \
  \
  /* \param[out] _outputInfo A void* which will be filled with the */ \
  /* PluginInfo that corresponds to _pluginId */ \
  \
  /* \param[in] _pluginId The index of the plugin which we want to load */ \
  /* during this call to the function */ \
  \
  /* \param[in] _size The expected size of a single PluginInfo object, to */ \
  /* ensure that this function agrees with the PluginLoader about the size */ \
  /* of a PluginInfo object. */ \
  \
  /* \return The number of plugins from _pluginId onward that are remaining */ \
  /* in this library. The expression */ \
  /* (IGNPLUGINMultiPluginInfo(info, id, size) > 0) will evaluate as true */ \
  /* if `info` has been filled with useful plugin information. */ \
  extern "C" std::size_t DETAIL_IGN_PLUGIN_VISIBLE IGNPLUGINMultiPluginInfo( \
      void * * const _outputInfo, \
      const std::size_t _pluginId, \
      const std::size_t _size) \
  { \
    if (_size != sizeof(ignition::plugin::Info)) \
    { \
      return 0u; \
    } \
    std::unordered_set<std::string> visitedPlugins; \
    ignition::plugin::Info * * const ptrToPlugin = \
        reinterpret_cast<ignition::plugin::Info * * const>(_outputInfo); \
    if ( !(*ptrToPlugin) ) \
    { \
       *ptrToPlugin = new ignition::plugin::Info; \
    } \
    ignition::plugin::Info *plugin = *ptrToPlugin; \
    plugin->name.clear(); \
    plugin->interfaces.clear(); \
    plugin->factory = nullptr; \
    plugin->deleter = nullptr;


#define DETAIL_IGN_PLUGIN_ADD_PLUGIN(pluginName, interface) \
  /* Attempt to ensure that the user provides fully-qualified class names*/ \
  static_assert(std::is_same<pluginName, ::pluginName>::value, \
      #pluginName " must be fully qualified like ::ns::MyClass"); \
  static_assert(std::is_same<interface, ::interface>::value, \
      #interface " must be fully qualified like ::ns::MyClass"); \
  \
  /* Print out a clear error when the plugin class is pure abstract (which */ \
  /* would make it impossible to load as a plugin). The compiler prevents */ \
  /* this from being an issue, but its printout might be difficult for */ \
  /* users to interpret, so we provide a very explicit explanation here. */ \
  static_assert(!std::is_abstract<pluginName>::value, \
      "[" #pluginName "] must not be an abstract class. It contains at least " \
      "one pure virtual function!"); \
  /* Print out a clear error when the plugin does not actually provide the */ \
  /* specified interface. */ \
  static_assert(std::is_base_of<interface, pluginName>::value, \
      "[" #interface "] is not a base class of [" #pluginName "], so it " \
      "cannot be used as a plugin interface for [" #pluginName "]!"); \
  /* We create an extra scope here so that the same variable names can be */ \
  /* reused between calls to this macro without any risk of interfering */ \
  /* with each other. */\
  { \
    const bool insertion = visitedPlugins.insert(#pluginName).second; \
    if (insertion) /* NOLINT */ \
    { \
      if (_pluginId == visitedPlugins.size() - 1) /* NOLINT */ \
      { \
        /* If the visitedPlugins has reached the requested _pluginId, fill */ \
        /* in the Info output parameter. */ \
        plugin->name = #pluginName; \
        plugin->factory = []() { \
          return static_cast<void*>(new pluginName()); \
        }; \
        plugin->deleter = [](void* ptr) { \
          delete static_cast< pluginName* >(ptr); \
        }; \
      } \
    } \
  \
    if ( #pluginName == plugin->name ) /* NOLINT */ \
    { \
      /* If the name of the desired plugin matches this call to the macro, */ \
      /* add a map entry for the interface specified by this macro. */ \
      plugin->interfaces.insert(std::make_pair( \
          #interface , [=](void* v_ptr) { \
              pluginName * d_ptr = static_cast< pluginName *>(v_ptr); \
              return static_cast< interface *>(d_ptr); \
          })); \
    } \
  }


#define DETAIL_IGN_PLUGIN_FINISH_ADDING_PLUGINS \
    if (_pluginId >= visitedPlugins.size()) /* NOLINT */ \
    { \
      if (plugin) \
        delete plugin; \
      return 0u; \
    } \
    return visitedPlugins.size() - _pluginId; \
  } \
IGN_UTILS_WARN_RESUME__DELETE_NON_VIRTUAL_DESTRUCTOR


#endif
