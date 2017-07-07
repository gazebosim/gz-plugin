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
    class PluginLoaderPrivate
    {
      /// \brief Directories that should be searched for plugins
      public: std::vector<std::string> searchPaths;

      /// \brief A list of known plugins
      public: std::vector<PluginInfo> plugins;

      /// \brief format the name to start with "::"
      public: std::string NormalizeName(const std::string &_name) const;

      /// \brief attempt to load a library at the given path
      public: void *LoadLibrary(const std::string &_pathToLibrary) const;

      /// \brief get plugin info for a library that has only one plugin
      public: PluginInfo LoadPlugin(void *_dlHandle) const;
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
      pretty << "\tKnown Plugins: " << interfaces.size() << std::endl;
      for (auto const &interface : interfaces)
      {
        for (auto const &plugin : this->PluginsImplementing(interface))
          pretty << "\t\t" << plugin << " (" << interface << ")" << std::endl;
      }
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
        PluginInfo plugin = this->dataPtr->LoadPlugin(dlHandle);
        if (plugin.name.size())
        {
          plugin.name = this->dataPtr->NormalizeName(plugin.name);
          plugin.interface = this->dataPtr->NormalizeName(plugin.interface);
          this->dataPtr->plugins.push_back(plugin);
          newPlugin = plugin.name;
        }
        else
        {
          ignerr << "Failed to load plugin from library [" << _pathToLibrary <<
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
    std::vector<std::string> PluginLoader::InterfacesImplemented() const
    {
      std::vector<std::string> interfaces;
      for (auto const &plugin : this->dataPtr->plugins)
      {
        if (interfaces.cend() == std::find(
              interfaces.cbegin(), interfaces.cend(), plugin.interface))
        {
          interfaces.push_back(plugin.interface);
        }
      }
      return interfaces;
    }

    /////////////////////////////////////////////////
    std::vector<std::string> PluginLoader::PluginsImplementing(
        const std::string &_interface) const
    {
      std::string interface = this->dataPtr->NormalizeName(_interface);
      std::vector<std::string> plugins;
      for (auto const &plugin : this->dataPtr->plugins)
      {
        if (plugin.interface == interface)
        {
          plugins.push_back(plugin.name);
        }
      }
      return plugins;
    }

    /////////////////////////////////////////////////
    void *PluginLoader::Instantiate(
        const std::string &_name, std::size_t _baseId) const
    {
      void *instance = nullptr;
      std::string name = this->dataPtr->NormalizeName(_name);
      for (auto const &plugin : this->dataPtr->plugins)
      {
        if (plugin.baseClassHash == _baseId && plugin.name == name)
        {
          // Creates plugin on heap
          instance = plugin.factory();
          break;
        }
      }
      return instance;
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
    PluginInfo PluginLoaderPrivate::LoadPlugin(void *_dlHandle) const
    {
      PluginInfo plugin;

      if (nullptr == _dlHandle)
      {
        ignerr << "Passed NULL handle.\n";
        return plugin;
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
        return plugin;
      }

      // Check abi version, and also check size because bugs happen
      int version = *(static_cast<int*>(versionPtr));
      std::size_t size = *(static_cast<std::size_t*>(sizePtr));
      if (PLUGIN_API_VERSION == version && sizeof(PluginInfo) == size)
      {
        // API 2 IGNCOMMONSinglePluginInfo accepts a void * and size_t
        // The pointer is a PluginInfo struct, and the size is the size
        // of the struct. If successfull it returns the size, else 0
        std::size_t (*Info)(void *, std::size_t) =
          reinterpret_cast<std::size_t(*)(void *, std::size_t)>(infoPtr);
        void *vPlugin = static_cast<void *>(&plugin);
        Info(vPlugin, sizeof(PluginInfo));
      }
      else if (PLUGIN_API_VERSION == 1 && sizeof(PluginInfo) == size)
      {
        // API 1 IGNCOMMONSinglePluginInfo returns a PluginInfo struct,
        // but that causes a compiler warning on OSX about c-linkage since
        // the struct is not C compatible
        PluginInfo (*Info)() = reinterpret_cast<PluginInfo(*)()>(infoPtr);
        plugin = Info();
      }
      else
      {
        ignerr << "Wrong plugin size for API version [" <<
                  PLUGIN_API_VERSION << "]. Expected [" << size << "], got [" <<
                  sizeof(PluginInfo) << "]" << std::endl;
        return plugin;
      }

      return plugin;
    }
  }
}
