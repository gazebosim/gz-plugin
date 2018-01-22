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
#include <set>

namespace ignition
{
  namespace common
  {
    /// \brief sentinel value to check if a plugin was built with the same
    /// version of the PluginInfo struct
    //
    /// This must be incremented when the PluginInfo struct changes
    const int PLUGIN_API_VERSION = 1;

    // We use an inline namespace to assist in forward-compatibility. Eventually
    // we may want to support a version-2 of the PluginInfo API, in which case
    // we will remove the "inline" declaration here, and create a new inline
    // namespace called "v2". This original PluginInfo object will continue to
    // be accessible for backwards compatibility, and even its symbol name in
    // the ABI should remain the same.
    inline namespace v1
    {
      /// \brief Holds info required to construct a plugin
      struct PluginInfo
      {
        /// \brief Clear out all information contained in this PluginInfo object
        void Clear();

        /// \brief The name of the plugin
        std::string name;

        /// \brief Alternative names that may be used to instantiate the plugin
        std::set<std::string> aliases;

        /// \brief The keys are the names of the types of interfaces that this
        /// plugin provides. The values are functions that convert a void pointer
        /// (which actually points to the plugin instance) to another void pointer
        /// (which actually points to the location of the interface within the
        /// plugin instance).
        using InterfaceCastingMap =
            std::unordered_map< std::string, std::function<void*(void*)> >;
        InterfaceCastingMap interfaces;

        /// \brief This is a set containing the demangled versions of the names of
        /// the interfaces provided by this plugin. This gets filled in by the
        /// PluginLoader after receiving the PluginInfo. It is only used by the
        /// user-facing API. Internally, when looking up Interfaces, the mangled
        /// `interfaces` map will still be used.
        std::set<std::string> demangledInterfaces;

        /// \brief A method that instantiates a new instance of a plugin
        std::function<void*()> factory;

        /// \brief A method that safely deletes an instance of the plugin
        std::function<void(void*)> deleter;
      };
    }

    /// This typedef is used simultaneously by detail/RegisterPlugin.hh and
    /// PluginLoader.cc, so we store it in a location that is visible to both
    /// of them.
    using PluginInfoMap = std::unordered_map<std::string, PluginInfo>;
  }
}

#endif
