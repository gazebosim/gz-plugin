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


#ifndef IGNITION_COMMON_SPECIALIZEDPLUGINPTR_HH_
#define IGNITION_COMMON_SPECIALIZEDPLUGINPTR_HH_

#include "ignition/common/PluginPtr.hh"
#include "ignition/common/detail/SpecializedPluginPtrMacros.hh"

namespace ignition
{
  namespace common
  {
    // Forward declarations
    namespace detail { template <class...> class ComposePlugin; }
    struct PluginInfo;

    /// \brief Declaration of the variadic template for SpecializedPluginPtr.
    /// This class definition is a C++ syntax formality.
    ///
    /// Look at the definition of SpecializedPluginPtr<SpecInterface> instead.
    template <class... OtherSpecInterfaces>
    class SpecializedPluginPtr
    {
      public: ~SpecializedPluginPtr() = default;
    };

    /// \brief This class allows PluginPtr instances to have low-cost access to
    /// interfaces that can be anticipated at compile time. The PluginPtr does
    /// not have to actually contain the interface in order to get this
    /// performance improvement. This template is variadic, so it can support
    /// arbitrarily many interfaces.
    ///
    /// Usage example:
    ///
    ///     using MySpecialPlugin = SpecializedPluginPtr<
    ///         MyInterface1, FooInterface, MyInterface2, BarInterface>;
    ///
    ///     std::unique_ptr<MySpecialPlugin> plugin =
    ///             loader->Instantiate<MySpecialPlugin>(pluginName);
    ///
    /// Then, calling the function
    ///
    ///     plugin->GetInterface<FooInterface>();
    ///
    /// will not have any cost associated with it. It will provide direct access
    /// to the the `FooInterface*` of `plugin`. If `plugin` does not actually
    /// offer `FooInterface`, then it will return a nullptr at very low cost.
    ///
    /// Only interfaces that have been "specialized" can be passed as arguments
    /// to the SpecializedPluginPtr template. To specialize an interface, simply
    /// put the macro IGN_COMMON_SPECIALIZE_INTERFACE(~) from
    /// ignition/common/PluginMacros.hh into its class definition.
    template <class SpecInterface>
    class SpecializedPluginPtr<SpecInterface> : public virtual TemplatePluginPtr
    {
      // ---------- Public API ----------

      /// \brief Default constructor. Initializes this object without any plugin
      /// instance. IsValid() will return false.
      public: SpecializedPluginPtr();

      DETAIL_IGN_COMMON_PLUGIN_CONSTRUCT_DESTRUCT_ASSIGN(SpecializedPluginPtr)

      // Inherit function overloads
      public: using TemplatePluginPtr::GetInterface;
      public: using TemplatePluginPtr::HasInterface;

      // Documentation inherited
      public: template <class Interface>
              Interface *GetInterface();

      // Documentation inherited
      public: template <class Interface>
              const Interface *GetInterface() const;

      // Documentation inherited
      public: template <class Interface>
              bool HasInterface() const;

      /// \brief Returns true if this SpecializedPluginPtr has been specialized
      /// for Interface, otherwise returns false.
      public: template <class Interface>
              static constexpr bool IsSpecializedFor();

      // ---------- Private API ----------

      // Declare friendship
      friend class PluginLoader;
      template <class...> friend class SpecializedPluginPtr;
      template <class...> friend class detail::ComposePlugin;

      private: template <class T> struct type { };

      /// \brief Delegate the function to the standard PluginPtr method
      private: template <class Interface>
               Interface *PrivateGetSpecInterface(type<Interface>);

      /// \brief Use a low-cost accessor to provide this specialized interface
      private: SpecInterface *PrivateGetSpecInterface(type<SpecInterface>);

      /// \brief Delegate the function to the standard PluginPtr method
      private: template <class Interface>
               const Interface *PrivateGetSpecInterface(type<Interface>) const;

      /// \brief Use a low-cost accessor to provide this specialized interface
      private: const SpecInterface *PrivateGetSpecInterface(
                   type<SpecInterface>) const;

      /// \brief Delegate the function to the standard PluginPtr method
      private: template <class Interface>
               bool PrivateHasSpecInterface(type<Interface>) const;

      /// \brief Use a low-cost accessor to check on this specialized interface
      private: bool PrivateHasSpecInterface(type<SpecInterface>) const;

      /// \brief Return false because Interface does not match SpecInterface
      private: template <class Interface>
               static constexpr bool PrivateIsSpecializedFor(type<Interface>);

      /// \brief Return true
      private: static constexpr bool PrivateIsSpecializedFor(
                   type<SpecInterface>);

      /// \brief Iterator that points to the entry of the specialized interface
      private:
      const TemplatePluginPtr::InterfaceMap::iterator privateSpecInterfaceIterator;
      // Dev note (MXG): The privateSpecInterfaceIterator object must be
      // available to the user during their compile time, so it cannot be hidden
      // using PIMPL. The iterator is const because it must always point to the
      // same entry throughout its entire lifecycle.

      /// \brief Standard constructor
      private: explicit SpecializedPluginPtr(const PluginInfo *info);
    };
  }
}

#include "ignition/common/detail/SpecializedPluginPtr.hh"

#endif // IGNITION_COMMON_SPECIALIZEDPLUGINPTR_HH_
