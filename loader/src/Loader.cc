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
#include "ignition/common/PluginPtr.hh"
#include "ignition/common/PluginInfo.hh"
#include "ignition/common/PluginLoader.hh"
#include "ignition/common/StringUtils.hh"
#include "ignition/common/Util.hh"

#include "PluginUtils.hh"

namespace ignition
{
  namespace common
  {
  /////////////////////////////////////////////////
    /// \brief PIMPL Implementation of the PluginLoader class
    class PluginLoaderPrivate
    {
      /// \brief Directories that should be searched for plugins
      public: std::vector<std::string> searchPaths;

      using PluginMap = std::unordered_map<std::string, PluginInfo>;

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
      return PluginPtr(this->PrivateGetPluginInfo(_plugin));
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

      const std::string versionSymbol = "IGNCOMMONPluginAPIVersion";
      const std::string sizeSymbol = "IGNCOMMONSinglePluginInfoSize";
      const std::string multiInfoSymbol = "IGNCOMMONMultiPluginInfo";
      void *versionPtr = dlsym(_dlHandle, versionSymbol.c_str());
      void *sizePtr = dlsym(_dlHandle, sizeSymbol.c_str());
      void *multiInfoPtr = dlsym(_dlHandle, multiInfoSymbol.c_str());

      // Does the library have the right symbols?
      if (nullptr == versionPtr || nullptr == sizePtr
          || nullptr == multiInfoPtr)
      {
        ignerr << "Library [" << _pathToLibrary
               << "] doesn't have the right symbols.\n";
        return loadedPlugins;
      }

      // Check abi version, and also check size because bugs happen
      int version = *(static_cast<int*>(versionPtr));
      std::size_t size = *(static_cast<std::size_t*>(sizePtr));

      if (version < PLUGIN_API_VERSION)
      {
        ignwarn << "The library [" << _pathToLibrary <<"] is using an outdated "
                << "version [" << version << "] of the ignition::common Plugin "
                << "API. The latest version is [" << PLUGIN_API_VERSION
                << "].\n";
      }

      if (sizeof(PluginInfo) == size)
      {
        using PluginLoadFunctionSignature =
            std::size_t(*)(void * const, std::size_t, std::size_t);

        std::size_t (*Info)(void * const, std::size_t, std::size_t) =
          reinterpret_cast<PluginLoadFunctionSignature>(multiInfoPtr);

        PluginInfo plugin;
        void *vPlugin = static_cast<void *>(&plugin);
        size_t id = 0;
        while (Info(vPlugin, id, sizeof(PluginInfo)) > 0)
        {
          loadedPlugins.push_back(plugin);
          ++id;
        }
      }
      else
      {
        const size_t expectedSize = sizeof(PluginInfo);

        ignerr << "The library [" << _pathToLibrary << "] has the wrong plugin "
               << "size for API version [" << PLUGIN_API_VERSION
               << "]. Expected [" << expectedSize << "], got ["
               << size << "]\n";

        return loadedPlugins;
      }

      return loadedPlugins;
    }
  }
}
