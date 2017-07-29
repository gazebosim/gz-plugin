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


#ifndef IGNITION_COMMON_PLUGIN_HH_
#define IGNITION_COMMON_PLUGIN_HH_

#include <map>

namespace ignition
{
  namespace common
  {
    // Forward declarations
    class PluginPrivate;
    struct PluginInfo;
    namespace detail { template<class...> class ComposePlugin; }

    class Plugin
    {
      /// \brief Destructor. Deletes the plugin instance that this object is
      /// providing interfaces for.
      public: virtual ~Plugin();

      /// \brief Get an interface of the specified type. Note that this function
      /// only works when the Interface type has its name statically embedded
      /// in its class definition as
      /// `static constexpr const char* Interface::InterfaceName`. For more
      /// general  interfaces which do not meet this condition, use
      /// GetInterface<Interface>(_interfaceName).
      public: template <typename Interface>
              Interface *GetInterface();

      /// \brief const-qualified version of GetInterface<Interface>()
      public: template <typename Interface>
              const Interface *GetInterface() const;

      /// \brief Get an interface with the given name, casted to the specified
      /// class type.
      public: template <typename Interface>
              Interface *GetInterface(const std::string &_interfaceName);

      /// \brief Get a const-qualified interface with the given name, casted
      /// to the specified const class type.
      public: template <typename Interface>
              const Interface *GetInterface(
                  const std::string &_interfaceName) const;

      /// \brief Returns true if this Plugin has the specified type of
      /// interface. Note that this function only works when the Interface type
      /// has its name statically embedded in its class definition as `static
      /// constexpr const char* Interface::InterfaceName`. For more general
      /// interfaces which do not meet this condition, use
      /// GetInterface<Interface>(_interfaceName).
      public: template <typename Interface>
              bool HasInterface() const;

      /// \brief Returns true if this Plugin has the specified type of
      /// interface.
      public: bool HasInterface(const std::string &_interfaceName) const;

      /// \brief This function always returns false if it is called on this
      /// basic Plugin class type. The SpecializedPlugin can shadow this to
      /// return true when it is specialized for this Interface type, however
      /// the function must be called on the SpecializedPlugin type and not
      /// this base class type, because this is a shadowed function, not a
      /// virtual function.
      public: template <class Interface>
              static constexpr bool IsSpecializedFor();

      /// \brief Type-agnostic retriever for interfaces
      private: void *PrivateGetInterface(
                  const std::string &_interfaceName) const;

      /// \brief PIMPL pointer to the implementation of this class
      private: PluginPrivate* dataPtr;

      // Declare friendship
      friend class PluginLoader;
      template <typename...> friend class SpecializedPlugin;
      template <typename...> friend class detail::ComposePlugin;

      /// \brief Constructor. Creates a plugin instance based on the PluginInfo
      /// provided. This should only be called by PluginLoader to ensure safety.
      private: Plugin(const PluginInfo *info = nullptr);

      public: using InterfaceMap = std::map<std::string, void*>;
      private: InterfaceMap::iterator PrivateGetOrCreateIterator(
          const std::string &_interfaceName);

      // It is not safe to copy or assign this Plugin
      // TODO(MXG): Consider if this can be made safe
      public: Plugin(const Plugin&) = delete;
      public: Plugin& operator=(const Plugin&) = delete;
      public: Plugin(Plugin &&other) = delete;
      public: Plugin& operator=(Plugin &&other) = delete;
    };
  }
}

#include "ignition/common/detail/Plugin.hh"

#endif // IGNITION_COMMON_PLUGIN_HH_
