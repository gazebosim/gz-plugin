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
#include "ignition/common/PluginInfo.hh"
#include "ignition/common/SuppressWarning.hh"

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

namespace ignition
{
  namespace common
  {
    namespace detail
    {
      //////////////////////////////////////////////////
      class PluginRegistrar
      {
        public: template<typename Plugin, typename Interface>
        void Register(const std::string &_derivedName,
                      const std::string &_interfaceName)
        {
          PluginRegister::iterator plugin_it;
          bool inserted;

          std::tie(plugin_it, inserted) =
              pluginRegister.insert(std::make_pair(_derivedName, PluginInfo()));

          PluginInfo &info = plugin_it->second;

          if(inserted)
          {
            // This Plugin has not been seen by the registrar before, so we need
            // to fill in the plugin-wide info members.
            info.name = _derivedName;

            // Create a factory for generating new plugin instances
            info.factory = []() {
              return static_cast<void*>(new Plugin);
            };

            IGN_COMMON_BEGIN_WARNING_SUPPRESSION(IGN_COMMON_DELETE_NON_VIRTUAL_DESTRUCTOR)
            // Create a deleter to clean up destroyed instances
            info.deleter = [](void *ptr) {
              delete static_cast<Plugin*>(ptr);
            };
            IGN_COMMON_FINISH_WARNING_SUPPRESSION(IGN_COMMON_DELETE_NON_VIRTUAL_DESTRUCTOR)
          }

          // Provide a map from the plugin to its interface
          info.interfaces.insert(std::make_pair(
                _interfaceName,
                [=](void* v_ptr) {
                    Plugin *d_ptr = static_cast<Plugin*>(v_ptr);
                    return static_cast<Interface*>(d_ptr);
                }));
        }

        using Map = std::unordered_map<std::string, PluginInfo>;

        inline const Map &GetPlugins()
        {
          return pluginRegister;
        }

        private: Map pluginRegister;
      };

      extern PluginRegistrar libraryPluginRegistrar;
    }
  }
}

extern "C"
{
  extern std::size_t DETAIL_IGN_PLUGIN_VISIBLE IGNCOMMONMultiPluginInfo(
      void *_outputInfo, const std::size_t _pluginId, const std::size_t _size)
  {
    if(_size != sizeof(ignition::common::PluginInfo))
    {
      return 0u;
    }

    const ignition::common::detail::PluginRegistrar::Map &plugins =
        ignition::common::detail::libraryPluginRegistrar.GetPlugins();

    ignition::common::PluginInfo *plugin =
        static_cast<ignition::common::PluginInfo*>(_outputInfo);

    // Probably not the most efficient way to grab plugins, but it is effective
    ignition::common::detail::PluginRegistrar::Map::iterator it =
        plugins.begin();
    for(std::size_t i=0; i < _pluginId; ++i)
    {
      ++it;
      if(it == plugins.end())
        break;
    }

    if(it == plugins.end())
      return plugins.size();

    *plugin = it->second;

    return plugins.size();
  }
}

#define DETAIL_IGN_COMMON_SPECIALIZE_INTERFACE(interfaceName) \
  static_assert(std::is_same<interfaceName, ::interfaceName>::value, \
      #interfaceName " must be fully qualified like ::ns::MyClass"); \
  static constexpr const char* IGNCOMMONInterfaceName = #interfaceName;

/// \brief Register the PluginInfo meta data
#define DETAIL_IGN_COMMON_REGISTER_PLUGININFO_META_DATA \
  extern "C" { \
    std::size_t DETAIL_IGN_PLUGIN_VISIBLE IGNCOMMONPluginInfoSize = \
      sizeof(ignition::common::PluginInfo); \
    \
    std::size_t DETAIL_IGN_PLUGIN_VISIBLE IGNCOMMONPluginInfoAlignment = \
      alignof(ignition::common::PluginInfo); \
    \
    int DETAIL_IGN_PLUGIN_VISIBLE IGNCOMMONPluginAPIVersion = \
      ignition::common::PLUGIN_API_VERSION; \
  }


#define DETAIL_IGN_COMMON_BEGIN_ADDING_PLUGINS \
  DETAIL_IGN_COMMON_REGISTER_PLUGININFO_META_DATA \
  struct IGN_macro_must_be_used_in_global_namespace;\
  static_assert(std::is_same < IGN_macro_must_be_used_in_global_namespace, \
      ::IGN_macro_must_be_used_in_global_namespace>::value, \
      "Macro for registering plugins must be used in global namespace"); \
  extern "C" std::size_t DETAIL_IGN_PLUGIN_VISIBLE IGNCOMMONMultiPluginInfo( \
      void *_outputInfo, const std::size_t _pluginId, const std::size_t _size) \
  { \
    if (_size != sizeof(ignition::common::PluginInfo)) \
    { \
      return 0u; \
    } \
    std::size_t pluginCount = 0; \
    std::unordered_set<std::string> visitedPlugins; \
    ignition::common::PluginInfo *plugin = \
        static_cast<ignition::common::PluginInfo*>(_outputInfo); \
    plugin->name.clear();


#define DETAIL_IGN_COMMON_ADD_PLUGIN(pluginName, interface) \
  /* cppcheck-suppress */ \
  static_assert(std::is_same<pluginName, ::pluginName>::value, \
      #pluginName " must be fully qualified like ::ns::MyClass"); \
  static_assert(std::is_same<interface, ::interface>::value, \
      #interface " must be fully qualified like ::ns::MyClass"); \
  \
  static_assert(!std::is_abstract<pluginName>::value, \
      "[" #pluginName "] must not be an abstract class. It contains at least " \
      "one pure virtual function!"); \
  static_assert(std::is_base_of<interface, pluginName>::value, \
      "[" #interface "] is not a base class of [" #pluginName "], so it " \
      "cannot be used as a plugin interface for [" #pluginName "]!"); \
  { \
    const bool insertion = visitedPlugins.insert(#pluginName).second; \
    if (insertion) \
    { \
      ++pluginCount; \
      if (_pluginId == pluginCount - 1) \
      { \
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
    if ( #pluginName == plugin->name ) \
    { \
      plugin->interfaces.insert(std::make_pair( \
          #interface , [=](void* v_ptr) { \
              pluginName * d_ptr = static_cast< pluginName *>(v_ptr); \
              return static_cast< interface *>(d_ptr); \
          })); \
    } \
  }


#define DETAIL_IGN_COMMON_FINISH_ADDING_PLUGINS \
    if (_pluginId > pluginCount) \
      return 0u; \
    return pluginCount - _pluginId; \
  } \


#endif
