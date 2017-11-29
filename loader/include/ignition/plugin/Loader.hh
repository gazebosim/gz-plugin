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

#include "ignition/common/System.hh"

namespace ignition
{
  namespace common
  {
    /// \brief Forward declaration
    class PluginLoaderPrivate;
    struct PluginInfo;
    class Plugin;

    using PluginPtr = std::shared_ptr<Plugin>;

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

      /// \brief Get demangled names of interfaces that the loader has plugins
      /// for.
      /// \returns Demangled names of the interfaces that are implemented
      public: std::unordered_set<std::string> InterfacesImplemented() const;

      /// \brief Get plugin names that implement the specified interface
      /// \return names of plugins that implement the interface.
      public: template <typename Interface>
      std::unordered_set<std::string> PluginsImplementing() const;

      /// \brief Get plugin names that implement the specified interface string.
      /// Note that the templated version of this function is recommended
      /// instead of this version to avoid confusion about whether a mangled or
      /// demangled version of a string is being used. Note that the function
      /// InterfacesImplemented() returns demangled versions of the interface
      /// names.
      ///
      /// If you want to pass in a mangled version of an interface name, e.g.
      /// the result that would be produced by typeid(T).name(), then set
      /// `demangled` to false.
      /// \param[in] _interface Name of an interface
      /// \param[in] _demangled Specify whether the _interface string is
      /// demangled (default, true) or mangled (false).
      /// \returns names of plugins that implement the interface
      public: std::unordered_set<std::string> PluginsImplementing(
          const std::string &_interface,
          const bool demangled = true) const;

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

      /// \brief Get a pointer to the PluginInfo corresponding to _pluginName.
      /// Returns nullptr if there is no info for the requested _pluginName.
      private: const PluginInfo *PrivateGetPluginInfo(
                  const std::string &_pluginName) const;

      /// \brief PIMPL pointer to class implementation
      private: std::unique_ptr<PluginLoaderPrivate> dataPtr;
    };
  }
}

#include <ignition/common/detail/PluginLoader.hh>

#endif
