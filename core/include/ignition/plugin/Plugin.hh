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

#include <memory>

namespace ignition
{
  namespace common
  {
    class PluginPrivate;
    class PluginInfo;

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
      public: template<typename Interface>
              Interface *GetInterface()
              {
                return static_cast<Interface*>(
                      PrivateGetInterface(Interface::InterfaceName));
              }

      /// \brief const-qualified version of GetInterface<Interface>()
      public: template<typename Interface>
              const Interface *GetInterface() const
              {
                return static_cast<const Interface*>(
                      PrivateGetInterface(Interface::InterfaceName));
              }

      /// \brief Get an interface with the given name, casted to the specified
      /// class type.
      public: template<typename Interface>
              Interface *GetInterface(const std::string &_interfaceName)
              {
                return static_cast<Interface*>(
                      PrivateGetInterface(_interfaceName));
              }

      /// \brief Get a const-qualified interface with the given name, casted
      /// to the specified const class type.
      public: template<typename Interface>
              const Interface *GetInterface(
                  const std::string &_interfaceName) const
              {
                return static_cast<const Interface*>(
                      PrivateGetInterface(_interfaceName));
              }

      /// \brief Returns true if this Plugin has the specified type of
      /// interface. Note that this function only works when the Interface type
      /// has its name statically embedded in its class definition as `static
      /// constexpr const char* Interface::InterfaceName`. For more general
      /// interfaces which do not meet this condition, use
      /// GetInterface<Interface>(_interfaceName).
      public: template<typename Interface>
              bool HasInterface() const
              {
                return HasInterface(Interface::InterfaceName);
              }

      /// \brief Returns true if this Plugin has the specified type of
      /// interface.
      public: bool HasInterface(const std::string &_interfaceName) const;

      /// \brief Type-agnostic retriever for interfaces
      private: void *PrivateGetInterface(
                  const std::string &_interfaceName) const;

      /// \brief PIMPL pointer to the implementation of this class
      private: PluginPrivate* dataPtr;

      friend class PluginLoader;

      /// \brief Constructor. Creates a plugin instance based on the PluginInfo
      /// provided. This should only be called by PluginLoader to ensure safety.
      private: Plugin(const PluginInfo &info);

      // It is not safe to copy or assign this Plugin by value
      public: Plugin(const Plugin&) = delete;
      public: Plugin& operator=(const Plugin&) = delete;

      // It is safe to move this Plugin
      public: Plugin(Plugin &&other);
      public: Plugin& operator=(Plugin &&other);
    };
  }
}

#endif // IGNITION_COMMON_PLUGIN_HH_
