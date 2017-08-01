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
    class PluginPtrPrivate;
    struct PluginInfo;
    namespace detail { template<class...> class ComposePlugin; }

    class PluginPtr
    {
      /// \brief Destructor. Deletes this PluginPtr's reference to the plugin
      /// instance. Once all PluginPtrs that refer to the plugin instances are
      /// deleted, the plugin will also be deleted.
      public: virtual ~PluginPtr();

      /// \brief Default constructor. Creates a PluginPtr object that does not
      /// point to any plugin instance. IsValid() will return false until a
      /// plugin instance is provided.
      public: PluginPtr();

      /// \brief Copy constructor. This PluginPtr will now point at the same
      /// plugin instance as _other, and they will share ownership. If this
      /// PluginPtr was holding an instance to another plugin, that instance
      /// will be deleted if no other PluginPtr is referencing it.
      public: PluginPtr(const PluginPtr &_other);

      /// \brief Copy assignment operator. This PluginPtr will now point at the
      /// same plugin instance as _other, and they will share ownership. If this
      /// PluginPtr was holding an instance to another plugin, that instance
      /// will be deleted if no other PluginPtr is referencing it.
      public: PluginPtr& operator =(const PluginPtr &_other);

      /// \brief Move constructor. This PluginPtr will take ownership of the
      /// plugin instance held by _other. If this PluginPtr was holding an
      /// instance to another plugin, that instance will be deleted if no other
      /// PluginPtr is referencing it.
      public: PluginPtr(PluginPtr &&_other);

      /// \brief Move assignment operator. This PluginPtr will take ownership
      /// of the plugin instance held by _other.  If this PluginPtr was holding
      /// an instance to another plugin, that instance will be deleted if no
      /// other PluginPtr is referencing it.
      public: PluginPtr& operator=(PluginPtr &&_other);

      /// \brief Comparison operator. Returns true if this Plugin is holding the
      /// same plugin instance as _other, otherwise returns false.
      public: bool operator ==(const PluginPtr &_other) const;

      /// \brief Returns true if the pointer value of the plugin instance held
      /// by this PluginPtr is less than the pointer value of the instance held
      /// by _other.
      public: bool operator <(const PluginPtr &_other) const;

      /// \brief Returns true if the pointer value of the plugin instance held
      /// by this PluginPtr is greater than the pointer value of the instance
      /// held by _other.
      public: bool operator >(const PluginPtr &_other) const;

      /// \brief Returns true if the pointer instance held by this PluginPtr is
      /// different from the pointer instance held by _other.
      public: bool operator !=(const PluginPtr &_other) const;

      /// \brief Returns true if the value of the pointer instance held by this
      /// PluginPtr is less than or equal to the value of the pointer instance
      /// held by _other.
      public: bool operator <=(const PluginPtr &_other) const;

      /// \brief Returns true if the value of the pointer instance held by this
      /// PluginPtr is greater than or equal to the value of the pointer
      /// instance held by _other.
      public: bool operator >=(const PluginPtr &_other) const;

      /// \brief Produces a hash for the plugin instance that this PluginPtr is
      /// holding.
      public: std::size_t Hash() const;

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

      /// \brief Returns true if this PluginPtr has the specified type of
      /// interface. Note that this function only works when the Interface type
      /// has its name statically embedded in its class definition as `static
      /// constexpr const char* Interface::InterfaceName`. For more general
      /// interfaces which do not meet this condition, use
      /// GetInterface<Interface>(_interfaceName).
      public: template <typename Interface>
              bool HasInterface() const;

      /// \brief Returns true if this PluginPtr has the specified type of
      /// interface.
      public: bool HasInterface(const std::string &_interfaceName) const;

      /// \brief This function always returns false if it is called on this
      /// basic PluginPtr class type. The SpecializedPluginPtr can shadow this
      /// to return true when it is specialized for this Interface type, however
      /// the function must be called on the SpecializedPluginPtr type and not
      /// this base class type, because this is a shadowed function, not a
      /// virtual function.
      public: template <class Interface>
              static constexpr bool IsSpecializedFor();

      /// \brief Returns true if this PluginPtr is pointing at a valid plugin
      /// instance. If it is instead pointing at a nullptr, this returns false.
      public: bool IsValid() const;

      /// \brief Clears the Plugin instance from this PluginPtr. IsValid() will
      /// return false after this is used, and none of the interfaces will be
      /// available any longer.
      public: void Clear();

      /// \brief Type-agnostic retriever for interfaces
      private: void *PrivateGetInterface(
                  const std::string &_interfaceName) const;

      /// \brief PIMPL pointer to the implementation of this class
      private: PluginPtrPrivate* dataPtr;

      // Declare friendship
      friend class PluginLoader;
      template <typename...> friend class SpecializedPluginPtr;
      template <typename...> friend class detail::ComposePlugin;

      /// \brief Private constructor. Creates a plugin instance based on the
      /// PluginInfo provided. This should only be called by PluginLoader to
      /// ensure that the PluginInfo is well-formed, so we keep it private.
      private: explicit PluginPtr(const PluginInfo *info);

      public: using InterfaceMap = std::map<std::string, void*>;
      private: InterfaceMap::iterator PrivateGetOrCreateIterator(
          const std::string &_interfaceName);
    };
  }
}

#include "ignition/common/detail/PluginPtr.hh"

#endif // IGNITION_COMMON_PLUGIN_HH_
