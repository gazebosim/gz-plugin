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

#include <ignition/common/System.hh>

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
      /// not meet this condition, use GetInterface<Interface>(_interfaceName).
      public: template <class Interface>
              Interface *GetInterface();

      /// \brief const-qualified version of GetInterface<Interface>()
      public: template <class Interface>
              const Interface *GetInterface() const;

      /// \brief Get an interface with the given name, casted to the specified
      /// class type.
      public: template <class Interface>
              Interface *GetInterface(const std::string &_interfaceName);

      /// \brief const-qualified version of GetInterface<Interface>(std::string)
      public: template <class Interface>
              const Interface *GetInterface(
                  const std::string &_interfaceName) const;

      /// \brief Get the requested interface as a std::shared_ptr. Note that
      /// this function only works when the Interface type is specialized using
      /// the macro IGN_COMMON_SPECIALIZE_INTERFACE. For more general interfaces
      /// which do not meet this condition, use
      /// as_shared_ptr<Interface>(_interfaceName).
      public: template <class Interface>
              std::shared_ptr<Interface> as_shared_ptr();

      /// \brief Same as as_shared_ptr<Interface>(), but it returns a
      /// std::shared_ptr to a const-qualified Interface.
      public: template <class Interface>
              std::shared_ptr<const Interface> as_shared_ptr() const;

      /// \brief Get the requested interface as a std::shared_ptr.
      public: template <class Interface>
              std::shared_ptr<Interface> as_shared_ptr(
                  const std::string &_interfaceName);

      /// \brief Same as as_shared_ptr<Interface>(std::string), but it returns a
      /// std::shared_ptr to a const-qualified Interface.
      public: template <class Interface>
              std::shared_ptr<const Interface> as_shared_ptr(
                  const std::string &_interfaceName) const;

      /// \brief Returns true if this Plugin has the specified type of
      /// interface. Note that this function only works when the Interface type
      /// is specialized using the macro IGN_COMMON_SPECIALIZE_INTERFACE. For
      /// more general interfaces which do not meet this condition, use
      /// GetInterface<Interface>(_interfaceName).
      public: template <class Interface>
              bool HasInterface() const;

      /// \brief Returns true if this Plugin has the specified type of
      /// interface.
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
      /// Plugin.
      public: using InterfaceMap = std::map<std::string, void*>;

      /// \brief Get or create an iterator to the std::map that holds pointers
      /// to the various interfaces provided by this plugin instance.
      private: InterfaceMap::iterator PrivateGetOrCreateIterator(
          const std::string &_interfaceName);

      /// \brief PIMPL pointer to the implementation of this class.
      private: const std::unique_ptr<PluginPrivate> dataPtr;

      /// \brief Virtual destructor
      public: virtual ~Plugin();
    };
  }
}

#include "ignition/common/detail/Plugin.hh"

#endif
