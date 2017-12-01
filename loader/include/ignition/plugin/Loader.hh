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


#ifndef IGNITION_COMMON_PLUGINLOADER_HH_
#define IGNITION_COMMON_PLUGINLOADER_HH_

#include <memory>
#include <string>
#include <typeinfo>
#include <unordered_set>

#include <ignition/common/Export.hh>
#include <ignition/common/SuppressWarning.hh>
#include <ignition/common/PluginPtr.hh>

namespace ignition
{
  namespace common
  {
    /// \brief Forward declaration
    class PluginLoaderPrivate;
    struct PluginInfo;

    /// \brief Class for loading plugins
    class IGNITION_COMMON_VISIBLE PluginLoader
    {
      /// \brief Constructor
      public: PluginLoader();

      /// \brief Destructor
      public: ~PluginLoader();

      /// \brief Makes a printable string with info about plugins
      /// \returns a pretty string
      public: std::string PrettyStr() const;

      /// \brief get names of interfaces that the loader has plugins for
      /// \returns interfaces that are implemented
      public: std::unordered_set<std::string> InterfacesImplemented() const;

      /// \brief get plugin names that implement the interface
      /// \param[in] _interface name of an interface
      /// \returns names of plugins that implement the interface
      public: std::unordered_set<std::string> PluginsImplementing(
                  const std::string &_interface) const;

      /// \brief Load a library at the given path
      /// \param[in] _pathToLibrary is the path to a libaray
      /// \returns the set of plugins that have been loaded from the library
      public: std::unordered_set<std::string> LoadLibrary(
                  const std::string &_pathToLibrary);

      /// \brief Instantiates a plugin for the given plugin name
      ///
      /// \param[in] _plugin name of the plugin to instantiate
      /// \returns ptr to instantiated plugin
      public: PluginPtr Instantiate(const std::string &_pluginName) const;

      /// \brief Instantiates a plugin of PluginType for the given plugin name.
      /// This can be used to create a specialized PluginPtr.
      ///
      /// \param[in] PluginType The specialized type of PluginPtrPtr that you
      /// want to construct.
      /// \param[in] _pluginName The name of the plugin that you want to
      /// instantiate
      /// \returns pointer for the instantiated PluginPtr
      public: template <typename PluginPtrType>
              PluginPtrType Instantiate(
                  const std::string &_pluginName) const;

      /// \brief This loader will forget about the library with the given name.
      /// If you want to instantiate a plugin from this library using this
      /// loader, you will first need to call LoadLibrary again.
      ///
      /// After this function has been called, once all plugin instances that
      /// are tied to the library have been deleted, the library will
      /// automatically be unloaded from the executable. Note that when this
      /// PluginLoader leaves scope (or gets deleted), it will have the same
      /// effect as calling ForgetLibrary on all of the libraries that it
      /// loaded, so there is generally no need to call this function. However,
      /// it may be useful if you want to reduce clutter in the PluginLoader
      /// instance or let go of library resources that are no longer being used.
      ///
      /// Note that even if you have released all references to a library, it is
      /// still up to the discretion of your operating system whether (or when)
      /// that library will be unloaded. In some cases, the operating system
      /// might not choose to unload it until the program exits completely.
      ///
      /// \param[in] _pathToLibrary Path to the library that you want to forget
      /// \return True if the library was actively loaded and is now
      /// successfully forgotten. If the library was not actively loaded, this
      /// returns false.
      public: bool ForgetLibrary(const std::string &_pathToLibrary);

      /// \brief Forget the library that provides the plugin with the given
      /// name. Note that this will also forget all other plugin types which
      /// are provided by that library.
      ///
      /// \param[in] _pluginName Name of the plugin whose library you want to
      /// forget.
      ///
      /// \sa bool ForgetLibrary(const std::string &_pathToLibrary)
      public: bool ForgetLibraryOfPlugin(const std::string &_pluginName);

      /// \brief Get a pointer to the PluginInfo corresponding to _pluginName.
      /// Returns nullptr if there is no info for the requested _pluginName.
      ///
      /// \param[in] _pluginName Name of the plugin that is being loaded.
      /// \return Pointer to the corresponding PluginInfo, or nullptr if the
      /// PluginInfo was unavailable.
      private: const PluginInfo *PrivateGetPluginInfo(
                  const std::string &_pluginName) const;

      /// \brief Get a std::shared_ptr that manages the lifecycle of the shared
      /// library handle which provides the specified plugin.
      ///
      /// \param[in] _pluginName Name of the plugin that is being loaded.
      /// \return Reference-counting pointer to a library handle, or else a
      /// nullptr if the plugin is not available.
      private: std::shared_ptr<void> PrivateGetPluginDlHandlePtr(
                  const std::string &_pluginName) const;

      IGN_COMMON_WARN_IGNORE__DLL_INTERFACE_MISSING
      /// \brief PIMPL pointer to class implementation
      private: std::unique_ptr<PluginLoaderPrivate> dataPtr;
      IGN_COMMON_WARN_RESUME__DLL_INTERFACE_MISSING
    };
  }
}

#include "ignition/common/detail/PluginLoader.hh"

#endif
