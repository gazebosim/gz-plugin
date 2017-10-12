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

#include <algorithm>
#include <dlfcn.h>
#include <functional>
#include <locale>
#include <sstream>
#include <unordered_map>

#include "ignition/common/Console.hh"
#include "ignition/common/PluginInfo.hh"
#include "ignition/common/PluginLoader.hh"
#include "ignition/common/StringUtils.hh"
#include "ignition/common/Util.hh"
#include "ignition/common/Plugin.hh"

#include "PluginUtils.hh"

namespace ignition
{
  namespace common
  {
  /////////////////////////////////////////////////
    /// \brief PIMPL Implementation of the PluginLoader class
    class PluginLoaderPrivate
    {
      public: using PluginMap = std::unordered_map<std::string, PluginInfo>;

      /// \brief A map from known plugin names to their PluginInfo
      public: PluginMap plugins;

      /// \brief attempt to load a library at the given path
      public: void *LoadLibrary(const std::string &_pathToLibrary) const;

      /// \brief get all the plugin info for a library
      public: std::vector<PluginInfo> LoadPlugins(
        void *_dlHandle, const std::string& _pathToLibrary) const;
    };

    /////////////////////////////////////////////////
    std::string PluginLoader::PrettyStr() const
    {
      auto interfaces = this->InterfacesImplemented();
      std::stringstream pretty;
      pretty << "PluginLoader State" << std::endl;
      pretty << "\tKnown Interfaces: " << interfaces.size() << std::endl;
      for (auto const &interface : interfaces)
        pretty << "\t\t" << interface << std::endl;

      pretty << "\tKnown Plugins: " << dataPtr->plugins.size() << std::endl;
      for (const auto& pair : dataPtr->plugins)
      {
        const PluginInfo& plugin = pair.second;
        const size_t i_size = plugin.interfaces.size();
        pretty << "\t\t[" << plugin.name << "] which implements "
               << i_size << PluralCast(" interface", i_size) << ":\n";
        for (const auto& interface : plugin.interfaces)
          pretty << "\t\t\t" << interface.first << "\n";
      }
      pretty << std::endl;

      return pretty.str();
    }

    /////////////////////////////////////////////////
    PluginLoader::PluginLoader()
      : dataPtr(new PluginLoaderPrivate())
    {
      // Do nothing.
    }

    /////////////////////////////////////////////////
    PluginLoader::~PluginLoader()
    {
      // Do nothing.
    }

    /////////////////////////////////////////////////
    std::unordered_set<std::string> PluginLoader::LoadLibrary(
        const std::string &_pathToLibrary)
    {
      std::unordered_set<std::string> newPlugins;

      if (!exists(_pathToLibrary))
      {
        ignerr << "Library [" << _pathToLibrary << "] does not exist!\n";
        return newPlugins;
      }

      // Attempt to load the library at this path
      void *dlHandle = this->dataPtr->LoadLibrary(_pathToLibrary);
      if (nullptr != dlHandle)
      {
        // Found a shared library, does it have the symbols we're looking for?
        std::vector<PluginInfo> loadedPlugins = this->dataPtr->LoadPlugins(
              dlHandle, _pathToLibrary);

        for (PluginInfo &plugin : loadedPlugins)
        {
          if (plugin.name.empty())
            continue;

          plugin.name = NormalizeName(plugin.name);

          PluginInfo::InterfaceCastingMap normalizedMap;
          normalizedMap.reserve(plugin.interfaces.size());
          for (const auto &interface : plugin.interfaces)
            normalizedMap.insert(std::make_pair(
                     NormalizeName(interface.first),
                     interface.second));
          plugin.interfaces = normalizedMap;

          this->dataPtr->plugins.insert(std::make_pair(plugin.name, plugin));
          newPlugins.insert(plugin.name);
        }

        if (loadedPlugins.empty())
        {
          ignerr << "Failed to load plugins from library [" << _pathToLibrary <<
                    "].\n";
        }
      }
      else
      {
        ignerr << "Library[" << _pathToLibrary << "] error: " << dlerror()
               << std::endl;
      }
      return newPlugins;
    }

    /////////////////////////////////////////////////
    std::unordered_set<std::string> PluginLoader::InterfacesImplemented() const
    {
      std::unordered_set<std::string> interfaces;
      for (auto const &plugin : this->dataPtr->plugins)
      {
        for (auto const &interface : plugin.second.interfaces)
          interfaces.insert(interface.first);
      }
      return interfaces;
    }

    /////////////////////////////////////////////////
    std::unordered_set<std::string> PluginLoader::PluginsImplementing(
        const std::string &_interface) const
    {
      const std::string interface = NormalizeName(_interface);
      std::unordered_set<std::string> plugins;
      for (auto const &plugin : this->dataPtr->plugins)
      {
        if (plugin.second.interfaces.find(interface) !=
           plugin.second.interfaces.end())
          plugins.insert(plugin.second.name);
      }

      return plugins;
    }

    /////////////////////////////////////////////////
    PluginPtr PluginLoader::Instantiate(
        const std::string &_plugin) const
    {
      PluginPtr instance = std::shared_ptr<Plugin>(new Plugin());
      instance->PrivateSetPluginInstance(this->PrivateGetPluginInfo(_plugin));
      return instance;
    }

    /////////////////////////////////////////////////
    const PluginInfo *PluginLoader::PrivateGetPluginInfo(
        const std::string &_pluginName) const
    {
      const std::string plugin = NormalizeName(_pluginName);

      PluginLoaderPrivate::PluginMap::const_iterator it =
          this->dataPtr->plugins.find(plugin);

      if (this->dataPtr->plugins.end() == it)
        return nullptr;

      return &(it->second);
    }

    /////////////////////////////////////////////////
    void* PluginLoaderPrivate::LoadLibrary(const std::string &_full_path) const
    {
      // Somehow this works on windows builds?
      return dlopen(_full_path.c_str(), RTLD_LAZY|RTLD_GLOBAL);
      // TODO(MXG): Consider checking for errors here using dlerror()
    }

    /////////////////////////////////////////////////
    std::vector<PluginInfo> PluginLoaderPrivate::LoadPlugins(
        void *_dlHandle, const std::string& _pathToLibrary) const
    {
      std::vector<PluginInfo> loadedPlugins;

      if (nullptr == _dlHandle)
      {
        ignerr << "Passed NULL handle.\n";
        return loadedPlugins;
      }

      const std::string infoSymbol = "IGNCOMMONInputOrOutputPluginInfo";
      void *infoFuncPtr = dlsym(_dlHandle, infoSymbol.c_str());

      // Does the library have the right symbol?
      if (nullptr == infoFuncPtr)
      {
        ignerr << "Library [" << _pathToLibrary << "] does not export any "
               << "plugins. The symbol [" << infoSymbol << "] is missing, or "
               << "it is not externally visible.\n";

        return loadedPlugins;
      }

      using PluginLoadFunctionSignature =
          void(*)(void * const, void *, int *, std::size_t *, std::size_t *);

      auto Info = reinterpret_cast<PluginLoadFunctionSignature>(infoFuncPtr);

      int version = PLUGIN_API_VERSION;
      std::size_t size = sizeof(PluginInfo);
      std::size_t alignment = alignof(PluginInfo);
      PluginInfoMap allInfo;

      void *vAllInfo = static_cast<void *>(&allInfo);
      Info(nullptr, vAllInfo, &version, &size, &alignment);

      if (ignition::common::PLUGIN_API_VERSION != version)
      {
        // TODO: When we need to support multiple API versions, put the logic
        // for it into here. We can call Info(~) again with the API version that
        // it expects.

        ignerr << "The library [" << _pathToLibrary << "] is using a newer "
               << "version [" << version << "] of the ignition::common Plugin "
               << "API. The version in this library is [" << PLUGIN_API_VERSION
               << "].\n";
        return loadedPlugins;
      }

      if (sizeof(PluginInfo) != size || alignof(PluginInfo) != alignment)
      {
        ignerr << "The PluginInfo size or alignment are not consistent with "
               << "the expected values for the library [" << _pathToLibrary
               << "]:\n -- size: expected " << sizeof(PluginInfo)
               << " | received " << size << "\n -- alignment: expected "
               << alignof(PluginInfo) << " | received " << alignment << "\n"
               << " -- We will not be able to safely load plugins from that "
               << "library.\n";

        return loadedPlugins;
      }

      for(const PluginInfoMap::value_type &info : allInfo)
      {
        loadedPlugins.push_back(info.second);
      }

      return loadedPlugins;
    }
  }
}
