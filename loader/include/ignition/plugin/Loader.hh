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

#include <ignition/common/System.hh>

namespace ignition
{
  namespace common
  {
    /// \brief Forward declaration
    class PluginLoaderPrivate;

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
      /// \returns name of plugin if library exists and contains a plugin
      public: std::string LoadLibrary(const std::string &_pathToLibrary);

      /// \brief Instantiates a plugin of the name and base class
      ///
      /// ex: pl.Instantiate<animals::AnimalBase>("animals::farm::Donkey")
      /// \param[in] _name name of a plugin
      /// \returns ptr to instantiated plugin
      public: template <typename T>
              std::unique_ptr<T> Instantiate(const std::string &_name) const
              {
                // type hash used to simplify this API
                std::unique_ptr<T> ptr;
                ptr.reset(static_cast<T*>(
                      this->Instantiate(_name)));
                return ptr;
              }

      /// \brief Instantiates a plugin of the name and base class hash
      /// \param[in] _name name of a plugin
      /// \param[in] _baseId typeid() hash_code() of base class type
      /// \returns pointer to instantiated plugin
      private: void *Instantiate(const std::string &_name) const;

      private: std::shared_ptr<PluginLoaderPrivate> dataPtr;
    };
  }
}

#endif
