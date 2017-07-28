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


#ifndef IGNITION_COMMON_DETAIL_SPECIALIZEDPLUGIN_HH_
#define IGNITION_COMMON_DETAIL_SPECIALIZEDPLUGIN_HH_

#include "ignition/common/SpecializedPlugin.hh"

namespace ignition
{
  namespace common
  {
    /////////////////////////////////////////////////
    template <class SpecInterface>
    template <class Interface>
    Interface *SpecializedPlugin<SpecInterface>::GetInterface()
    {
      return this->PrivateGetSpecInterface(type<Interface>());
    }

    /////////////////////////////////////////////////
    template <class SpecInterface>
    template <class Interface>
    const Interface *SpecializedPlugin<SpecInterface>::GetInterface() const
    {
      return this->PrivateGetSpecInterface(type<Interface>());
    }

    /////////////////////////////////////////////////
    template <class SpecInterface>
    template <class Interface>
    bool SpecializedPlugin<SpecInterface>::HasInterface() const
    {
      return this->PrivateHasSpecInterface(type<Interface>());
    }

    /////////////////////////////////////////////////
    template <class SpecInterface>
    template <class Interface>
    constexpr bool SpecializedPlugin<SpecInterface>::IsSpecializedFor()
    {
      return PrivateIsSpecializedFor(type<Interface>());
    }

    /////////////////////////////////////////////////
    template <class SpecInterface>
    template <class Interface>
    Interface *SpecializedPlugin<SpecInterface>::PrivateGetSpecInterface(
        type<Interface>)
    {
      return this->Plugin::GetInterface<Interface>();
    }

    /////////////////////////////////////////////////
    template <class SpecInterface>
    SpecInterface *SpecializedPlugin<SpecInterface>::PrivateGetSpecInterface(
        type<SpecInterface>)
    {
      return static_cast<SpecInterface*>(
            this->privateSpecInterfaceIterator->second);
    }

    /////////////////////////////////////////////////
    template <class SpecInterface>
    template <class Interface>
    const Interface *SpecializedPlugin<SpecInterface>::
    PrivateGetSpecInterface(type<Interface>) const
    {
      return this->Plugin::GetInterface<Interface>();
    }

    /////////////////////////////////////////////////
    template <class SpecInterface>
    const SpecInterface *SpecializedPlugin<SpecInterface>::
    PrivateGetSpecInterface(type<SpecInterface>) const
    {
      return static_cast<SpecInterface*>(
            this->privateSpecInterfaceIterator->second);
    }

    /////////////////////////////////////////////////
    template <class SpecInterface>
    template <class Interface>
    bool SpecializedPlugin<SpecInterface>::PrivateHasSpecInterface(
        type<Interface>) const
    {
      return this->Plugin::HasInterface<Interface>();
    }

    /////////////////////////////////////////////////
    template <class SpecInterface>
    bool SpecializedPlugin<SpecInterface>::PrivateHasSpecInterface(
        type<SpecInterface>) const
    {
      return (nullptr != this->privateSpecInterfaceIterator->second);
    }

    /////////////////////////////////////////////////
    template <class SpecInterface>
    template <class Interface>
    constexpr bool SpecializedPlugin<SpecInterface>::PrivateIsSpecializedFor(
        type<Interface>)
    {
      return false;
    }

    template <class SpecInterface>
    constexpr bool SpecializedPlugin<SpecInterface>::PrivateIsSpecializedFor(
        type<SpecInterface>)
    {
      return true;
    }

    /////////////////////////////////////////////////
    template <class SpecInterface>
    SpecializedPlugin<SpecInterface>::SpecializedPlugin(
        const PluginInfo *info)
      : Plugin(info),
        privateSpecInterfaceIterator(
          this->PrivateGetOrCreateIterator(SpecInterface::InterfaceName))
    {
      // Do nothing
    }

  }
}

#endif // IGNITION_COMMON_DETAIL_SPECIALIZEDPLUGIN_HH_
