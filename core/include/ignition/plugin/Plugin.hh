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
#include <map>
#include <string>

#include <ignition/common/Export.hh>
#include <ignition/common/SuppressWarning.hh>

namespace ignition
{
  namespace common
  {
    // Forward declarations
    struct PluginInfo;
    class PluginPrivate;
    namespace detail { template <class, class> class ComposePlugin; }

    class IGNITION_COMMON_VISIBLE Plugin
    {
      // -------------------- Public API ---------------------

      /// \brief Get an interface of the specified type. Note that this function
      /// only works when the Interface type is specialized using the macro
      /// IGN_COMMON_SPECIALIZE_INTERFACE. For more general  interfaces which do
      /// not meet this condition, use
      /// QueryInterface<Interface>(_interfaceName).
      ///
      /// Note that the interface pointer you receive is owned by the Plugin
      /// object. You MUST NOT ever try to deallocate it yourself. Moreover, the
      /// pointer will be invalidated once all Plugin objects that refer to the
      /// same Plugin instance are destructed. Use the QueryInterfaceSharedPtr
      /// function in order to get a reference-counting pointer to an interface
      /// of this Plugin object. The pointer will remain valid as long as the
      /// std::shared_ptr provided by QueryInterfaceSharedPtr is alive.
      public: template <class Interface>
              Interface *QueryInterface();

      /// \brief const-qualified version of QueryInterface<Interface>()
      public: template <class Interface>
              const Interface *QueryInterface() const;

      /// \brief Get an interface with the given name, casted to the specified
      /// class type. The template argument Interface must exactly match the
      /// underlying type associated with _interfaceName, or else the behavior
      /// of this function is undefined.
      ///
      /// Note that the interface pointer you receive is owned by the Plugin
      /// object. You MUST NOT ever try to deallocate it yourself. Moreover, the
      /// pointer will be invalidated once all Plugin objects that refer to the
      /// same Plugin instance are destructed. Use the QueryInterfaceSharedPtr
      /// function in order to get a reference-counting pointer to an interface
      /// of this Plugin object. The pointer will remain valid as long as the
      /// std::shared_ptr provided by QueryInterfaceSharedPtr is alive.
      ///
      /// \param[in] _interfaceName The name of the desired interface, as a
      /// string.
      /// \return A raw pointer to the specified interface. If the requested
      /// _interfaceName is not provided by this Plugin, this returns a nullptr.
      /// This pointer is invalidated when the reference count of the plugin
      /// instance drops to zero.
      public: template <class Interface>
              Interface *QueryInterface(const std::string &_interfaceName);

      /// \brief const-qualified version of
      /// QueryInterface<Interface>(std::string)
      public: template <class Interface>
              const Interface *QueryInterface(
                  const std::string &_interfaceName) const;

      /// \brief Get the requested interface as a std::shared_ptr. Note that
      /// this function only works when the Interface type is specialized using
      /// the macro IGN_COMMON_SPECIALIZE_INTERFACE. For more general interfaces
      /// which do not meet this condition, use
      /// QueryInterfaceSharedPtr<Interface>(const std::string&).
      public: template <class Interface>
              std::shared_ptr<Interface> QueryInterfaceSharedPtr();

      /// \brief Same as QueryInterfaceSharedPtr<Interface>(), but it returns a
      /// std::shared_ptr to a const-qualified Interface.
      public: template <class Interface>
              std::shared_ptr<const Interface> QueryInterfaceSharedPtr() const;

      /// \brief Get the requested interface as a std::shared_ptr. The template
      /// argument Interface must exactly match the underlying type associated
      /// with _interfaceName, or else the behavior of this function is
      /// undefined.
      ///
      /// This std::shared_ptr and the interface+plugin that it refers to will
      /// remain valid, even if all Plugin objects which refer to the plugin
      /// instance are destructed.
      ///
      /// You MUST NOT attempt to pass a QueryInterface pointer into a
      /// std::shared_ptr yourself; that will result in double-delete memory
      /// errors. You must always call QueryInterfaceSharedPtr for a reference-
      /// counting pointer to an interface.
      ///
      /// \param[in] _interfaceName The name of the desired interface, as a
      /// string.
      /// \return A reference-counting pointer to the specified interface. This
      /// will keep the interface valid and the plugin instance alive, even if
      /// all Plugin objects that refer to this plugin instance are destructed.
      public: template <class Interface>
              std::shared_ptr<Interface> QueryInterfaceSharedPtr(
                  const std::string &_interfaceName);

      /// \brief Same as QueryInterfaceSharedPtr<Interface>(std::string), but
      /// it returns a std::shared_ptr to a const-qualified Interface.
      public: template <class Interface>
              std::shared_ptr<const Interface> QueryInterfaceSharedPtr(
                  const std::string &_interfaceName) const;

      /// \brief Returns true if this Plugin has the specified type of
      /// interface. Note that this function only works when the Interface type
      /// is specialized using the macro IGN_COMMON_SPECIALIZE_INTERFACE. For
      /// more general interfaces which do not meet this condition, use
      /// QueryInterface<Interface>(_interfaceName).
      public: template <class Interface>
              bool HasInterface() const;

      /// \brief Returns true if this Plugin has the specified type of
      /// interface, otherwise returns false.
      ///
      /// \param[in] _interfaceName The name of the desired interface, as a
      /// string.
      public: bool HasInterface(const std::string &_interfaceName) const;


      // -------------------- Private API -----------------------

      template <class> friend class TemplatePluginPtr;
      template <class...> friend class SpecializedPlugin;
      template <class, class> friend class detail::ComposePlugin;

      /// \brief Default constructor. This is kept private to ensure that
      /// Plugins are always managed by a PluginPtr object.
      private: Plugin();

      /// \brief Type-agnostic retriever for interfaces
      private: void *PrivateGetInterface(
                  const std::string &_interfaceName) const;

      /// \brief Copy the plugin instance from another Plugin object
      private: void PrivateCopyPluginInstance(const Plugin &_other) const;

      /// \brief Create a new plugin instance based on the info provided
      private: void PrivateSetPluginInstance(const PluginInfo *_info) const;

      /// \brief Get a reference to the std::shared_ptr being managed by this
      /// wrapper
      private: const std::shared_ptr<void>& PrivateGetInstancePtr() const;

      /// \brief The InterfaceMap type needs to get used in several places, like
      /// PluginPrivate and SpecializedPlugin<T>. We make the typedef public so
      /// that those other classes can use it without needing to be friends of
      /// Plugin. End-users should not have any need for this typedef.
      public: using InterfaceMap = std::map<std::string, void*>;

      /// \brief Get or create an iterator to the std::map that holds pointers
      /// to the various interfaces provided by this plugin instance.
      private: InterfaceMap::iterator PrivateGetOrCreateIterator(
          const std::string &_interfaceName);

      IGN_COMMON_WARN_IGNORE__DLL_INTERFACE_MISSING
      /// \brief PIMPL pointer to the implementation of this class.
      private: const std::unique_ptr<PluginPrivate> dataPtr;
      IGN_COMMON_WARN_RESUME__DLL_INTERFACE_MISSING

      /// \brief Virtual destructor
      public: virtual ~Plugin();
    };
  }
}

#include "ignition/common/detail/Plugin.hh"

#endif
