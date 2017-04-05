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


#ifndef IGNITION_COMMON_PLUGIN_PLUGININFO_HH_
#define IGNITION_COMMON_PLUGIN_PLUGININFO_HH_

#include <functional>
#include <string>

namespace ignition
{
namespace common
{

/// \brief sentinel value to check if a plugin was built with the same
///     version of the PluginInfo struct
//
/// This must be incremented when the PluginInfo struct changes
const int PLUGIN_API_VERSION = 1;


/// \brief Holds info required to construct a plugin
struct PluginInfo
{
  /// \brief the name of the plugin
  std::string name;

  /// \brief the name of the type of plugin this implements
  std::string interface;

  /// \brief returns a hash that can be used to compare the base class type
  ///
  /// This is only used to make the PluginLoader::Instantiate API simpler
  std::size_t baseClassHash;

  /// \brief a method that instantiates a new instance of a plugin
  std::function<void*()> factory;
};


}
}

#endif
