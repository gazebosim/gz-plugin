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


#ifndef IGNITION_COMMON_PLUGININFO_HH_
#define IGNITION_COMMON_PLUGININFO_HH_

#include <functional>
#include <string>
#include <unordered_map>

namespace ignition
{
  namespace common
  {
    /// \brief sentinel value to check if a plugin was built with the same
    /// version of the PluginInfo struct
    //
    /// This must be incremented when the PluginInfo struct changes
    const int PLUGIN_API_VERSION = 1;


    /// \brief Holds info required to construct a plugin
    struct PluginInfo
    {
      /// \brief The name of the plugin
      std::string name;

      /// \brief The keys are the names of the types of interfaces that this
      /// plugin provides. The values are functions that convert a void pointer
      /// (which actually points to the plugin instance) to another void pointer
      /// (which actually points to the location of the interface within the
      /// plugin instance).
      using InterfaceCastingMap =
          std::unordered_map< std::string, std::function<void*(void*)> >;
      InterfaceCastingMap interfaces;

      /// \brief A method that instantiates a new instance of a plugin
      std::function<void*()> factory;

      /// \brief A method that safely deletes an instance of the plugin
      std::function<void(void*)> deleter;
    };
  }
}

#endif
