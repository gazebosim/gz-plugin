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
      /// \returns A pretty string
      public: std::string PrettyStr() const;

      /// \brief Get names of interfaces that the loader has plugins for
      /// \returns Interfaces that are implemented
      public: std::unordered_set<std::string> InterfacesImplemented() const;

      /// \brief Get plugin names that implement the interface
      /// \param[in] _interface Name of an interface
      /// \returns Names of plugins that implement the interface
      public: std::unordered_set<std::string> PluginsImplementing(
                  const std::string &_interface) const;

      /// \brief Load a library at the given path
      /// \param[in] _pathToLibrary is the path to a libaray
      /// \returns The set of plugins that have been loaded from the library
      public: std::unordered_set<std::string> LoadLibrary(
                  const std::string &_pathToLibrary);

      /// \brief Instantiates a plugin for the given plugin name
      ///
      /// \param[in] _plugin name of the plugin to instantiate
      /// \returns Pointer to instantiated plugin
      public: PluginPtr Instantiate(const std::string &_pluginName) const;

      /// \brief Instantiates a plugin of PluginType for the given plugin name.
      /// This can be used to create a specialized PluginPtr.
      ///
      /// \tparam PluginPtrType The specialized type of PluginPtrPtr that you
      /// want to construct.
      /// \param[in] _pluginName The name of the plugin that you want to
      /// instantiate
      /// \returns pointer for the instantiated PluginPtr
      public: template <typename PluginPtrType>
              PluginPtrType Instantiate(
                  const std::string &_pluginName) const;

      /// \brief Get a pointer to the PluginInfo corresponding to _pluginName.
      /// \param[in] _pluginName The name of the plugin that you want to
      /// instantiate
      /// \return Pointer to PluginInfo or nullptr if there is no info for the
      /// requested _pluginName.
      private: const PluginInfo *PrivateGetPluginInfo(
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
