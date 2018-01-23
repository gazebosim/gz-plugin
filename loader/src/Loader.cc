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

#if defined(__GNUC__) or defined(__clang__)
// This header is used for name demangling on GCC and Clang
#include <cxxabi.h>
#endif

#ifdef __GNUC__
// This is used to check the version of glibc when compiling with GCC
#include <features.h>
#endif

#include "ignition/common/Console.hh"
#include "ignition/common/PluginPtr.hh"
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
#if defined(__GNUC__) or defined(__clang__)
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
#elif _MSC_VER

      assert(_name.substr(0, 6) == "class ");

      // Visual Studio's typeid(~).name() does not mangle the name, except that
      // it prefixes the normal name of the class with the character sequence 
      // "class ". So to get the "demangled" name, all we have to do is remove
      // the first six characters. The plugin framework does not handle any
      // non-class types, so we do not lose anything by removing the "class "
      // designator.
      return _name.substr(6);
#else
      // If we don't know the compiler, then we can't perform name demangling.
      // The tests will probably fail in this situation, and the class names
      // will probably look gross to users. Plugin name aliasing can be used
      // to make plugins robust to this situation.
      return _name;
#endif
    }

    /////////////////////////////////////////////////
    /// \brief PIMPL Implementation of the PluginLoader class
    class PluginLoaderPrivate
    {
      /// \brief attempt to load a library at the given path
      public: std::shared_ptr<void> LoadLibrary(
        const std::string &_pathToLibrary);

      /// \brief get all the plugin info for a library
      public: std::vector<PluginInfo> LoadPlugins(
        const std::shared_ptr<void> &_dlHandle,
        const std::string& _pathToLibrary) const;

      public: bool ForgetLibrary(void *_dlHandle);

      public: using PluginToDlHandleMap =
          std::unordered_map<std::string, std::shared_ptr<void>>;
      /// \brief A map from known plugin names to the handle of the library that
      /// provides it.
      ///
      /// CRUCIAL DEV NOTE (MXG): `pluginToDlHandlePtrs` MUST come BEFORE
      /// `plugins` in this class definition to ensure that `plugins` gets
      /// deleted first (member variables get destructed in the reverse order of
      /// their appearance in the class definition). The destructors of the
      /// `deleter` members of the PluginInfo class depend on the shared library
      /// still being available, so this map of std::shared_ptrs to the library
      /// handles must be destroyed after the PluginInfo.
      ///
      /// If you change this class definition for ANY reason, be sure to
      /// maintain the ordering of these member variables.
      public: PluginToDlHandleMap pluginToDlHandlePtrs;

      public: using PluginMap = std::unordered_map<std::string, PluginInfo>;
      /// \brief A map from known plugin names to their PluginInfo
      ///
      /// CRUCIAL DEV NOTE (MXG): `plugins` MUST come AFTER
      /// `pluginToDlHandlePtrs` in this class definition. See the comment on
      /// pluginToDlHandlePtrs for an explanation.
      ///
      /// If you change this class definition for ANY reason, be sure to
      /// maintain the ordering of these member variables.
      public: PluginMap plugins;

      using DlHandleMap = std::unordered_map<void*, std::weak_ptr<void>>;
      /// \brief A map which keeps track of which shared libraries have been
      /// loaded by this PluginLoader.
      public: DlHandleMap dlHandlePtrMap;

      public: using DlHandleToPluginMap =
          std::unordered_map<void*, std::unordered_set<std::string>>;
      /// \brief A map from the shared library handle to the names of the
      /// plugins that it provides.
      public: DlHandleToPluginMap dlHandleToPluginMap;
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
      const std::shared_ptr<void> &dlHandle =
          this->dataPtr->LoadLibrary(_pathToLibrary);

      // Quit early and return an empty set of plugin names if we did not
      // actually get a valid dlHandle.
      if (nullptr == dlHandle)
        return newPlugins;

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

        // Add the plugin to the map
        this->dataPtr->plugins.insert(std::make_pair(plugin.name, plugin));

        // Add the plugin's name to the set of newPlugins
        newPlugins.insert(plugin.name);

        // Save the dl handle for this plugin
        this->dataPtr->pluginToDlHandlePtrs[plugin.name] = dlHandle;
      }

      dataPtr->dlHandleToPluginMap[dlHandle.get()] = newPlugins;

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
        const std::string &_pluginName) const
    {
      return PluginPtr(this->PrivateGetPluginInfo(_pluginName),
                       this->PrivateGetPluginDlHandlePtr(_pluginName));
    }

    /////////////////////////////////////////////////
    bool PluginLoader::ForgetLibrary(const std::string &_pathToLibrary)
    {
      if (!exists(_pathToLibrary))
        return false;

#ifndef __GLIBC__
// This macro is not part of the POSIX standard, and is a custom addition to
// glibc-2.2, so we need create a no-op stand-in flag for it if we are not
// using glibc-2.2.
#define RTLD_NOLOAD 0
#else
#  if __GLIBC__ < 2
#    define RTLD_NOLOAD 0
#  elif __GLIBC__ == 2 and __GLIBC_MINOR__ < 2
#    define RTLD_NOLOAD 0
#  endif
#endif

      void *dlHandle = dlopen(_pathToLibrary.c_str(),
                              RTLD_NOLOAD | RTLD_LAZY | RTLD_LOCAL);

      if (!dlHandle)
        return false;

      // We should decrement the reference count because we called dlopen. Even
      // with the RTLD_NOLOAD flag, the call to dlopen will still (allegedly)
      // increment the reference count when it returns a valid handle. Note that
      // this knowledge is according to online discussions and is not explicitly
      // stated in the manual pages of dlopen (but it is consistent with the
      // overall behavior of dlopen).
      dlclose(dlHandle);

      return this->dataPtr->ForgetLibrary(dlHandle);
    }

    /////////////////////////////////////////////////
    bool PluginLoader::ForgetLibraryOfPlugin(const std::string &_pluginName)
    {
      PluginLoaderPrivate::PluginToDlHandleMap::iterator it =
          dataPtr->pluginToDlHandlePtrs.find(_pluginName);

      if (dataPtr->pluginToDlHandlePtrs.end() == it)
        return false;

      return dataPtr->ForgetLibrary(it->second.get());
    }

    /////////////////////////////////////////////////
    const PluginInfo *PluginLoader::PrivateGetPluginInfo(
        const std::string &_pluginName) const
    {
      PluginLoaderPrivate::PluginMap::const_iterator it =
          this->dataPtr->plugins.find(_pluginName);

      if (this->dataPtr->plugins.end() == it)
      {
        ignerr << "Failed to get info for plugin ["
               << _pluginName
               << "] since it has not been loaded."
               << std::endl;
        return nullptr;
      }

      return &(it->second);
    }

    /////////////////////////////////////////////////
    std::shared_ptr<void> PluginLoader::PrivateGetPluginDlHandlePtr(
        const std::string &_pluginName) const
    {
      PluginLoaderPrivate::PluginToDlHandleMap::iterator it =
          dataPtr->pluginToDlHandlePtrs.find(_pluginName);

      if (this->dataPtr->pluginToDlHandlePtrs.end() == it)
        return nullptr;

      return it->second;
    }

    /////////////////////////////////////////////////
    std::shared_ptr<void> PluginLoaderPrivate::LoadLibrary(
        const std::string &_full_path)
    {
      std::shared_ptr<void> dlHandlePtr;

      // Call dlerror() before dlopen(~) to ensure that we get accurate error
      // reporting afterwards. The function dlerror() is stateful, and that
      // state gets cleared each time it is called.
      dlerror();

      // NOTE: We open using RTLD_LOCAL instead of RTLD_GLOBAL to prevent the
      // symbols of different libraries from writing over each other.
      void *dlHandle = dlopen(_full_path.c_str(), RTLD_LAZY | RTLD_LOCAL);

      const char *loadError = dlerror();
      if (nullptr == dlHandle || nullptr != loadError)
      {
        ignerr << "Error while loading the library [" << _full_path << "]: "
               << loadError << std::endl;

        // Just return a nullptr if the library could not be loaded. The
        // PluginLoader::LoadLibrary(~) function will handle this gracefully.
        return nullptr;
      }

      // The dl library maintains a reference count of how many times dlopen(~)
      // or dlclose(~) has been called on each loaded library. dlopen(~)
      // increments the reference count while dlclose(~) decrements the count.
      // When the count reaches zero, the operating system is free to unload
      // the library (which it might or might not choose to do; we do not seem
      // to have control over that, probably for the best).

      // The idea in our implementation here is to first check if the library
      // has already been loaded by this PluginLoader. If it has been, then we
      // will see it in the dlHandlePtrMap, and we'll be able to get a shared
      // reference to its handle. The advantage of this is it allows us to keep
      // track of how many places the shared library is being used by plugins
      // that were generated from this PluginLoader. When all of the instances
      // spawned by this PluginLoader are gone, the destructor of the
      // std::shared_ptr will call dlclose on the library handle, bringing down
      // its dl library reference count.

      bool inserted;
      DlHandleMap::iterator it;
      std::tie(it, inserted) = dlHandlePtrMap.insert(
            std::make_pair(dlHandle, std::weak_ptr<void>()));

      if (!inserted)
      {
        // This shared library has already been loaded by this PluginLoader in
        // the past, so we should use the reference counter that already
        // exists for it.
        dlHandlePtr = it->second.lock();

        if(dlHandlePtr)
        {
          // The reference counter of this library is still active.

          // The functions dlopen and dlclose keep their own independent
          // counters for how many times each of them has been called. The
          // library is unloaded once dlclose has been called as many times as
          // dlopen.
          //
          // At this line of code, we know that dlopen had been called by this
          // PluginLoader instance prior to this run of LoadLibrary. Therefore,
          // we should undo the dlopen that we did just a moment ago in this
          // function so that only one dlclose must be performed to finally
          // close the shared library. That final dlclose will be performed in
          // the destructor of the std::shared_ptr<void> which stores the
          // library handle.
          dlclose(dlHandle);
        }
      }

      if (!dlHandlePtr)
      {
        // The library was not already loaded (or if it was loaded in the past,
        // it is no longer active), so we should create a reference counting
        // handle for it.
        dlHandlePtr = std::shared_ptr<void>(
              dlHandle, [](void *ptr) { dlclose(ptr); });

        it->second = dlHandlePtr;
      }

      return dlHandlePtr;
    }

    /////////////////////////////////////////////////
    std::vector<PluginInfo> PluginLoaderPrivate::LoadPlugins(
        const std::shared_ptr<void> &_dlHandle,
        const std::string& _pathToLibrary) const
    {
      std::vector<PluginInfo> loadedPlugins;

      if (nullptr == _dlHandle)
      {
        ignerr << "Passed NULL handle.\n";
        return loadedPlugins;
      }

      const std::string infoSymbol = "IGNCOMMONInputOrOutputPluginInfo";
      void *infoFuncPtr = dlsym(_dlHandle.get(), infoSymbol.c_str());

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

    /////////////////////////////////////////////////
    bool PluginLoaderPrivate::ForgetLibrary(void *_dlHandle)
    {
      DlHandleToPluginMap::iterator it = dlHandleToPluginMap.find(_dlHandle);
      if(dlHandleToPluginMap.end() == it)
        return false;

      const std::unordered_set<std::string> &forgottenPlugins = it->second;

      for(const std::string &forget : forgottenPlugins)
      {
        // CRUCIAL DEV NOTE (MXG): Be sure to erase the PluginInfo from
        // `plugins` BEFORE erasing the plugin entry in `pluginToDlHandlePtrs`,
        // because the PluginInfo structs require the library to remain loaded
        // for the destructors of their `deleter` member variables.

        // This erase should come FIRST.
        plugins.erase(forget);

        // This erase should come LAST.
        pluginToDlHandlePtrs.erase(forget);
      }

      // Dev note (MXG): We do not need to delete anything from `dlHandlePtrMap`
      // because it uses std::weak_ptrs. It will clear itself automatically.

      // Dev note (MXG): This erase call should come at the very end of this
      // function to ensure that the `forgottenPlugins` reference remains valid
      // while it is being used.
      dlHandleToPluginMap.erase(it);

      // Dev note (MXG): We do not need to call dlclose because that will be
      // taken care of automatically by the std::shared_ptr that manages the
      // shared library handle.

      return true;
    }
  }
}
