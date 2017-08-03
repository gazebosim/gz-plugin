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


#ifndef IGNITION_COMMON_DETAIL_PLUGIN_HH_
#define IGNITION_COMMON_DETAIL_PLUGIN_HH_

#include "ignition/common/PluginPtr.hh"

namespace ignition
{
  namespace common
  {
    //////////////////////////////////////////////////
    template <typename Interface>
    Interface *PluginPtr::GetInterface()
    {
      return static_cast<Interface*>(
            this->PrivateGetInterface(Interface::InterfaceName));
    }

    //////////////////////////////////////////////////
    template <typename Interface>
    const Interface *PluginPtr::GetInterface() const
    {
      return static_cast<const Interface*>(
            this->PrivateGetInterface(Interface::InterfaceName));
    }

    //////////////////////////////////////////////////
    template <typename Interface>
    Interface *PluginPtr::GetInterface(const std::string &_interfaceName)
    {
      return static_cast<Interface*>(
            this->PrivateGetInterface(_interfaceName));
    }

    //////////////////////////////////////////////////
    template <typename Interface>
    const Interface *PluginPtr::GetInterface(
        const std::string &_interfaceName) const
    {
      return static_cast<const Interface*>(
            this->PrivateGetInterface(_interfaceName));
    }

    //////////////////////////////////////////////////
    template <typename Interface>
    bool PluginPtr::HasInterface() const
    {
      return this->HasInterface(Interface::InterfaceName);
    }
  }
}

// Note that opening up namespace std is legal here because we are specializing
// a templated structure from the STL, which is permitted (and even encouraged).
namespace std
{
  /// \brief Template specialization that provides a hash function for PluginPtr
  /// so that it can easily be used in STL objects like std::unordered_set and
  /// std::unordered_map
  template <> struct hash<ignition::common::PluginPtr>
  {
    size_t operator()(const ignition::common::PluginPtr &ptr) const
    {
      return ptr.Hash();
    }
  };
}

#endif
