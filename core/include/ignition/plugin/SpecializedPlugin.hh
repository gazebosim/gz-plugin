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


#ifndef IGNITION_COMMON_SPECIALIZEDPLUGIN_HH_
#define IGNITION_COMMON_SPECIALIZEDPLUGIN_HH_

#include "ignition/common/Plugin.hh"

namespace ignition
{
  namespace common
  {
    class PluginInfo;

    template <class... OtherSpecInterfaces>
    class SpecializedPlugin
    {
      public: ~SpecializedPlugin() = default;
    };

    template <class SpecInterface>
    class SpecializedPlugin<SpecInterface> : public virtual Plugin
    {
      // ---------- Public API ----------

      /// \brief Default destructor
      public: virtual ~SpecializedPlugin() = default;

      public: template <class Interface>
              Interface *GetInterface();

      public: template <class Interface>
              const Interface *GetInterface() const;

      public: template <class Interface>
              bool HasInterface() const;

      public: template <class Interface>
              static constexpr bool IsSpecializedFor();

      // ---------- Private API ----------

      // Declare friendship
      friend class PluginLoader;
      template <class...> friend class SpecializedPlugin;

      private: template <class T> struct type { };

      private: template <class Interface>
               Interface *PrivateGetSpecInterface(type<Interface>);

      private: SpecInterface *PrivateGetSpecInterface(type<SpecInterface>);

      private: template <class Interface>
               const Interface *PrivateGetSpecInterface(type<Interface>) const;

      private: const SpecInterface *PrivateGetSpecInterface(
                   type<SpecInterface>) const;

      private: template <class Interface>
               bool PrivateHasSpecInterface(type<Interface>) const;

      private: bool PrivateHasSpecInterface(type<SpecInterface>) const;

      private: template <class Interface>
               static constexpr bool PrivateIsSpecializedFor(type<Interface>);

      private: static constexpr bool PrivateIsSpecializedFor(
                   type<SpecInterface>);

      /// \brief Iterator that points to the entry of the specialized interface
      private:
      const Plugin::InterfaceMap::iterator privateSpecInterfaceIterator;
      // Dev note (MXG): The privateSpecInterfaceIterator object must be
      // available to the user during their compile time, so it cannot be hidden
      // using PIMPL. The iterator is const because it must always point to the
      // same entry throughout its entire lifecycle.

      /// \brief Default constructor
      private: SpecializedPlugin(const PluginInfo *info);
    };
  }
}

#include "ignition/common/detail/SpecializedPlugin.hh"

#endif // IGNITION_COMMON_SPECIALIZEDPLUGIN_HH_
