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

#if defined __GNUC__ or defined __clang__
// This header is used for name demangling on GCC and Clang
#include <cxxabi.h>
#endif

#include "ignition/common/Console.hh"
#include "ignition/common/PluginInfo.hh"
#include "ignition/common/PluginLoader.hh"
#include "ignition/common/StringUtils.hh"
#include "ignition/common/Util.hh"
#include "ignition/common/Plugin.hh"

namespace ignition
{
  namespace common
  {
    /////////////////////////////////////////////////
    std::string Demangle(const std::string &_name)
    {
#if defined __GNUC__ or defined __clang__
      int status;
      char *demangled_cstr = abi::__cxa_demangle(
            _name.c_str(), nullptr, nullptr, &status);

      if (0 != status)
      {
        ignerr << "[Demangle] Failed to demangle the symbol name [" << _name
               << "]. Error code: " << status << "\n";
        return _name;
      }

      const std::string demangled(demangled_cstr);
      free(demangled_cstr);

      return demangled;
#else
      // MSVC does not require any demangling. Any other compilers besides GCC
      // or clang may be using an unknown ABI, so we won't be able to demangle
      // them anyway.
      return _name;
#endif
    }

    /////////////////////////////////////////////////
    /// \brief PIMPL Implementation of the PluginLoader class
    class PluginLoaderPrivate
    {
      /// \brief Attempt to load a library at the given path.
      /// \param[in] _pathToLibrary The full path to the desired library
      /// \return If a library exists at the given path, get a point to its dl
      /// handle. If the library does not exist, get a nullptr.
      public: void *LoadLibrary(const std::string &_pathToLibrary) const;

      /// \brief Using a dl handle produced by LoadLibrary, extract the
      /// PluginInfo from the loaded library.
      /// \param[in] _dlHandle A handle produced by LoadLibrary
      /// \param[in] _pathToLibrary The path that the library was loaded from
      /// (used for debug purposes)
      /// \return All the PluginInfo provided by the loaded library.
      public: std::vector<PluginInfo> LoadPlugins(
        void *_dlHandle, const std::string& _pathToLibrary) const;

      public: using PluginMap = std::unordered_map<std::string, PluginInfo>;

      /// \brief A map from known plugin names to their PluginInfo
      public: PluginMap plugins;
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
      for (const auto &pair : dataPtr->plugins)
      {
        const PluginInfo &plugin = pair.second;
        const size_t iSize = plugin.interfaces.size();
        pretty << "\t\t[" << plugin.name << "] which implements "
               << iSize << PluralCast(" interface", iSize) << ":\n";
        for (const auto &interface : plugin.demangledInterfaces)
          pretty << "\t\t\t" << interface << "\n";
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
          // Demangle the plugin name before creating an entry for it.
          plugin.name = Demangle(plugin.name);

          // Make a list of the demangled interface names for later convenience.
          for (auto const &interface : plugin.interfaces)
            plugin.demangledInterfaces.insert(Demangle(interface.first));

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
        for (auto const &interface : plugin.second.demangledInterfaces)
          interfaces.insert(interface);
      }
      return interfaces;
    }

    /////////////////////////////////////////////////
    std::unordered_set<std::string> PluginLoader::PluginsImplementing(
        const std::string &_interface,
        const bool demangled) const
    {
      std::unordered_set<std::string> plugins;

      if (demangled)
      {
        for (auto const &plugin : this->dataPtr->plugins)
        {
          if (plugin.second.demangledInterfaces.find(_interface) !=
              plugin.second.demangledInterfaces.end())
            plugins.insert(plugin.second.name);
        }
      }
      else
      {
        for (auto const &plugin : this->dataPtr->plugins)
        {
          if (plugin.second.interfaces.find(_interface) !=
              plugin.second.interfaces.end())
            plugins.insert(plugin.second.name);
        }
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
      PluginLoaderPrivate::PluginMap::const_iterator it =
          this->dataPtr->plugins.find(_pluginName);

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
        void *_dlHandle, const std::string &_pathToLibrary) const
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
          void(*)(void * const, const void ** const,
                  int *, std::size_t *, std::size_t *);

      // Note: Info (below) is a function with a signature that matches
      // PluginLoadFunctionSignature.
      auto Info = reinterpret_cast<PluginLoadFunctionSignature>(infoFuncPtr);

      int version = PLUGIN_API_VERSION;
      std::size_t size = sizeof(PluginInfo);
      std::size_t alignment = alignof(PluginInfo);
      const PluginInfoMap *allInfo = nullptr;

      // Note: static_cast cannot be used to convert from a T** to a void**
      // because of the possibility of breaking the type system by assigning a
      // Non-T pointer to the T* memory location. However, we need to retrieve
      // a reference to an STL-type using a C-compatible function signature, so
      // we resort to a reinterpret_cast to achieve this.
      //
      // Despite its many dangers, reinterpret_cast is well-defined for casting
      // between pointer types as of C++11, as explained in bullet point 1 of
      // the "Explanation" section in this reference:
      // http://en.cppreference.com/w/cpp/language/reinterpret_cast
      //
      // We have a tight grip over the implementation of how the `allInfo`
      // pointer gets used, so we do not need to worry about its memory address
      // being filled with a non-compatible type. The only risk would be if a
      // user decides to implement their own version of
      // IGNCOMMONInputOrOutputPluginInfo, but they surely would have no
      // incentive in doing that.
      //
      // Also note that the main reason we jump through these hoops is in order
      // to safely support plugin libraries on Windows that choose to compile
      // against the static runtime. Using this pointer-to-a-pointer approach is
      // the cleanest way to ensure that all dynamically allocated objects are
      // deleted in the same heap that they were allocated from.
      Info(nullptr, reinterpret_cast<const void** const>(&allInfo),
           &version, &size, &alignment);

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

      if (!allInfo)
      {
        ignerr << "The library [" << _pathToLibrary << "] failed to provide "
               << "PluginInfo for unknown reasons. Please report this error as "
               << "a bug!\n";
        assert(false);

        return loadedPlugins;
      }

      for(const PluginInfoMap::value_type &info : *allInfo)
      {
        loadedPlugins.push_back(info.second);
      }

      return loadedPlugins;
    }
  }
}
