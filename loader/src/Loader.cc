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

namespace ignition
{
  namespace common
  {
    /////////////////////////////////////////////////
    PluginInfo convertPluginFromOldVersion(const PluginInfo_v2& old_info)
    {
      PluginInfo info;
      info.name = old_info.name;
      info.interfaces.insert(old_info.interface);
      info.factory = old_info.factory;

      return info;
    }

    /////////////////////////////////////////////////
    class PluginLoaderPrivate
    {
      /// \brief Directories that should be searched for plugins
      public: std::vector<std::string> searchPaths;

      using PluginMap = std::unordered_map<std::string, PluginInfo>;

      /// \brief A list of known plugins
      public: PluginMap plugins;

      /// \brief format the name to start with "::"
      public: std::string NormalizeName(const std::string &_name) const;

      /// \brief attempt to load a library at the given path
      public: void *LoadLibrary(const std::string &_pathToLibrary) const;

      /// \brief get plugin info for a library that has only one plugin
      public: std::vector<PluginInfo> LoadPlugins(void *_dlHandle) const;
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
        pretty << "\t\t[" << plugin.name << "] which implements "
               << plugin.interfaces.size() << " interfaces:\n";
        for(const std::string& interface : plugin.interfaces)
          std::cout << "\t\t\t" << interface << "\n";
      }
      std::cout << std::endl;

      return pretty.str();
    }

    /////////////////////////////////////////////////
    PluginLoader::PluginLoader()
      : dataPtr(new PluginLoaderPrivate())
    {
    }

    /////////////////////////////////////////////////
    PluginLoader::~PluginLoader()
    {
    }

    /////////////////////////////////////////////////
    std::string PluginLoader::LoadLibrary(const std::string &_pathToLibrary)
    {
      std::string newPlugin;

      if (!exists(_pathToLibrary))
      {
        ignerr << "Library [" << _pathToLibrary << "] does not exist!\n";
        return newPlugin;
      }

      // Attempt to load the library at this path
      void *dlHandle = this->dataPtr->LoadLibrary(_pathToLibrary);
      if (nullptr != dlHandle)
      {
        // Found a shared library, does it have the symbols we're looking for?
        std::vector<PluginInfo> plugins = this->dataPtr->LoadPlugins(dlHandle);

        for(PluginInfo &plugin : plugins)
        {
          if(plugin.name.empty())
            continue;

          plugin.name = this->dataPtr->NormalizeName(plugin.name);

          std::unordered_set<std::string> normalizedSet;
          normalizedSet.reserve(plugin.interfaces.size());
          for(const std::string &interface : plugin.interfaces)
            normalizedSet.insert(this->dataPtr->NormalizeName(interface));
          plugin.interfaces = normalizedSet;

          this->dataPtr->plugins.insert(std::make_pair(plugin.name, plugin));
          newPlugin = plugin.name;
        }

        if(plugins.empty())
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
      return newPlugin;
    }

    /////////////////////////////////////////////////
    std::unordered_set<std::string> PluginLoader::InterfacesImplemented() const
    {
      std::unordered_set<std::string> interfaces;
      for (auto const &plugin : this->dataPtr->plugins)
      {
        for(auto const &interface : plugin.second.interfaces)
          interfaces.insert(interface);
      }
      return interfaces;
    }

    /////////////////////////////////////////////////
    std::unordered_set<std::string> PluginLoader::PluginsImplementing(
        const std::string &_interface) const
    {
      const std::string interface = this->dataPtr->NormalizeName(_interface);
      std::unordered_set<std::string> plugins;
      for (auto const &plugin : this->dataPtr->plugins)
      {
        if(plugin.second.interfaces.find(interface) !=
           plugin.second.interfaces.end())
          plugins.insert(plugin.second.name);
      }

      return plugins;
    }

    /////////////////////////////////////////////////
    void *PluginLoader::Instantiate(
        const std::string &_name) const
    {
      const std::string name = this->dataPtr->NormalizeName(_name);

      PluginLoaderPrivate::PluginMap::iterator it =
          this->dataPtr->plugins.find(name);

      if(this->dataPtr->plugins.end() == it)
        return nullptr;

      return it->second.factory();
    }

    /////////////////////////////////////////////////
    std::string PluginLoaderPrivate::NormalizeName(const std::string &_name)
      const
    {
      std::string name = _name;
      if (!StartsWith(_name, "::"))
      {
        name = std::string("::") + _name;
      }
      return name;
    }

    /////////////////////////////////////////////////
    void* PluginLoaderPrivate::LoadLibrary(const std::string &_full_path) const
    {
      // Somehow this works on windows builds?
      return dlopen(_full_path.c_str(), RTLD_LAZY|RTLD_GLOBAL);
    }

    /////////////////////////////////////////////////
    std::vector<PluginInfo> PluginLoaderPrivate::LoadPlugins(
        void *_dlHandle) const
    {
      std::vector<PluginInfo> plugins;

      if (nullptr == _dlHandle)
      {
        ignerr << "Passed NULL handle.\n";
        return plugins;
      }

      std::string versionSymbol = "IGNCOMMONPluginAPIVersion";
      std::string sizeSymbol = "IGNCOMMONSinglePluginInfoSize";
      std::string infoSymbol = "IGNCOMMONSinglePluginInfo";
      void *versionPtr = dlsym(_dlHandle, versionSymbol.c_str());
      void *sizePtr = dlsym(_dlHandle, sizeSymbol.c_str());
      void *infoPtr = dlsym(_dlHandle, infoSymbol.c_str());

      // Does the library have the right symbols?
      if (nullptr == versionPtr || nullptr == sizePtr || nullptr == infoPtr)
      {
        ignerr << "Library doesn't have the right symbols.\n";
        return plugins;
      }

      // Check abi version, and also check size because bugs happen
      int version = *(static_cast<int*>(versionPtr));
      std::size_t size = *(static_cast<std::size_t*>(sizePtr));
      if (PLUGIN_API_VERSION == version && sizeof(PluginInfo) == size)
      {
        // TODO(MXG): Implement this

        std::size_t (*Info)(void *, std::size_t) =
          reinterpret_cast<std::size_t(*)(void *, std::size_t)>(infoPtr);
        void *vPlugin = static_cast<void *>(&plugin);
        Info(vPlugin, sizeof(PluginInfo));
      }
      else if (PLUGIN_API_VERSION == 2 && sizeof(PluginInfo_v2) == size)
      {
        PluginInfo_v2 plugin;
        // API 2 IGNCOMMONSinglePluginInfo accepts a void * and size_t
        // The pointer is a PluginInfo_v2 struct, and the size is the size
        // of the struct. If successful it returns the size, else 0
        std::size_t (*Info)(void *, std::size_t) =
          reinterpret_cast<std::size_t(*)(void *, std::size_t)>(infoPtr);
        void *vPlugin = static_cast<void *>(&plugin);
        Info(vPlugin, sizeof(PluginInfo_v2));
        plugins.push_back(convertPluginFromOldVersion(plugin));
      }
      else if (PLUGIN_API_VERSION == 1 && sizeof(PluginInfo_v2) == size)
      {
        // API 1 IGNCOMMONSinglePluginInfo returns a PluginInfo struct,
        // but that causes a compiler warning on OSX about c-linkage since
        // the struct is not C compatible
        PluginInfo_v2 (*Info)() = reinterpret_cast<PluginInfo_v2(*)()>(infoPtr);
        plugins.push_back(convertPluginFromOldVersion(Info()));
      }
      else
      {
        ignerr << "Wrong plugin size for API version [" <<
                  PLUGIN_API_VERSION << "]. Expected [" << sizeof(PluginInfo_v2)
                  << "], got [" << size << "]" << std::endl;
        return plugins;
      }

      return plugins;
    }
  }
}
