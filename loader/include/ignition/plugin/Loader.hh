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


#ifndef IGNITION_COMMON_PLUGIN_PLUGINLOADER_HH_
#define IGNITION_COMMON_PLUGIN_PLUGINLOADER_HH_

#include <memory>
#include <string>
#include <typeinfo>
#include <vector>

namespace ignition
{
namespace common
{
namespace plugin
{

/// \brief Forward declaration
class PluginLoaderPrivate;

/// \brief Class for loading plugins
class PluginLoader
{
  /// \brief Constructor
  public: PluginLoader();

  /// \brief Destructor
  public: ~PluginLoader();

  /// \brief Returns a printable string with info about plugins
  public: std::string PrettyStr() const;

  /// \brief Adds a path to search for plugins
  public: void AddSearchPath(std::string _path);

  /// \brief Returns paths that are being searched for plugins
  public: std::vector<std::string> SearchPaths() const;

  /// \brief Returns a list of interfaces that the loader has plugins for
  public: std::vector<std::string> InterfacesImplemented() const;

  /// \brief Returns a list of plugin names that implement the interface
  public: std::vector<std::string> PluginsImplementing(
               std::string _interface) const;

  /// \brief Load a library with a plugin by name
  public: bool LoadLibrary(std::string _libName);

  /// \brief Instantiates a plugin of the name and base class
  ///
  /// ex: pl.Instantiate<animals::AnimalBase>("animals::farm::Donkey")
  public: template <typename T>
  std::unique_ptr<T> Instantiate(std::string _name) const
  {
    // type hash used to simplify this API
    std::unique_ptr<T> ptr;
    ptr.reset(static_cast<T*>(
        this->Instantiate(_name, typeid(T).hash_code())));
    return ptr;
  }

  private: void* Instantiate(std::string _name, std::size_t _baseId) const;

  private: std::shared_ptr<PluginLoaderPrivate> dataPtr;

};

}
}
}

#endif
