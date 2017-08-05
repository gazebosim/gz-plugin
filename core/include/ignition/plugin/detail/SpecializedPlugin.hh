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

// This preprocessor token should only be used by the unittest that is
// responsible for checking that the specialized routines are being used to
// access specialized plugin interfaces.
#ifdef IGNITION_UNITTEST_SPECIALIZED_PLUGIN_ACCESS
bool usedSpecializedInterfaceAccess;
#endif


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
    std::shared_ptr<Interface> SpecializedPlugin<SpecInterface>::as_shared_ptr()
    {
      Interface *ptr = this->GetInterface<Interface>();
      if(ptr)
        return std::shared_ptr<Interface>(this->PrivateGetInstancePtr(), ptr);

      return nullptr;
    }

    /////////////////////////////////////////////////
    template <class SpecInterface>
    template <class Interface>
    std::shared_ptr<const Interface>
    SpecializedPlugin<SpecInterface>::as_shared_ptr() const
    {
      const Interface *ptr = this->GetInterface<Interface>();
      if(ptr)
        return std::shared_ptr<Interface>(this->PrivateGetInstancePtr(), ptr);

      return nullptr;
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
      #ifdef IGNITION_UNITTEST_SPECIALIZED_PLUGIN_ACCESS
      usedSpecializedInterfaceAccess = true;
      #endif
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
      #ifdef IGNITION_UNITTEST_SPECIALIZED_PLUGIN_ACCESS
      usedSpecializedInterfaceAccess = true;
      #endif
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
      #ifdef IGNITION_UNITTEST_SPECIALIZED_PLUGIN_ACCESS
      usedSpecializedInterfaceAccess = true;
      #endif
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
    SpecializedPlugin<SpecInterface>::SpecializedPlugin()
      : privateSpecInterfaceIterator(
          this->PrivateGetOrCreateIterator(SpecInterface::InterfaceName))
    {
      // Do nothing
    }


    namespace detail
    {
      template <class... OtherBases>
      class ComposePlugin
      {
        public: virtual ~ComposePlugin() = default;
      };

      template <class Base1>
      class ComposePlugin<Base1> : public virtual Base1
      {
        // Declare friendship
        template <class...> friend class SpecializedPlugin;
        template <class...> friend class ComposePlugin;

        /// \brief Default destructor
        public: virtual ~ComposePlugin() = default;

        public: ComposePlugin() = default;
      };


      template <class Base1, class Base2>
      class ComposePlugin<Base1, Base2> :
          public virtual Base1,
          public virtual Base2
      {
        // Declare friendship
        template <class...> friend class SpecializedPlugin;
        template <class...> friend class ComposePlugin;

        /// \brief Default destructor
        public: virtual ~ComposePlugin() = default;

        // Inherit function overloads
        using Plugin::GetInterface;
        using Plugin::as_shared_ptr;
        using Plugin::HasInterface;

        /// \brief Implement functions whose only role is to dispatch its functionality
        /// between two base classes, depending on which base is specialized for the
        /// template type. This must only be called within the ComposePlugin class.
        #define DETAIL_IGN_COMMON_COMPOSEPLUGIN_DISPATCH(\
                      ReturnType, Function, Suffix, Args)\
          public:\
          template <class T>\
          ReturnType Function Suffix\
          {\
            if (Base1::template IsSpecializedFor<T>())\
              return Base1::template Function <T> Args ;\
          \
            return Base2::template Function <T> Args ;\
          }

DETAIL_IGN_COMMON_COMPOSEPLUGIN_DISPATCH(T*, GetInterface, (), ())
DETAIL_IGN_COMMON_COMPOSEPLUGIN_DISPATCH(const T*, GetInterface, () const, ())
DETAIL_IGN_COMMON_COMPOSEPLUGIN_DISPATCH(std::shared_ptr<T>, as_shared_ptr, (), ())
DETAIL_IGN_COMMON_COMPOSEPLUGIN_DISPATCH(std::shared_ptr<const T>, as_shared_ptr, () const, ())
DETAIL_IGN_COMMON_COMPOSEPLUGIN_DISPATCH(bool, HasInterface, () const, ())

        public: template<class T>
                static constexpr bool IsSpecializedFor()
        {
          return (Base1::template IsSpecializedFor<T>()
                  || Base2::template IsSpecializedFor<T>());
        }

        // Declare friendship
        template <class...> friend class SpecializedPlugin;
        template <class...> friend class ComposePlugin;

        private: ComposePlugin() = default;
      };

      template <class Base1, class Base2, class... OtherBases>
      class ComposePlugin<Base1, Base2, OtherBases...> :
          public virtual ComposePlugin<
            Base1, ComposePlugin<Base2, OtherBases...> >
      {
        // Declare friendship
        template <class...> friend class SpecializedPlugin;
        template <class...> friend class ComposePlugin;

        /// \brief Virtual destructor
        public: virtual ~ComposePlugin() = default;

        using Base = ComposePlugin<Base1, ComposePlugin<Base2, OtherBases...>>;

        /// \brief Default constructor
        private: ComposePlugin() = default;
      };
    }

    template <class SpecInterface1, class... OtherSpecInterfaces>
    class SpecializedPlugin<SpecInterface1, OtherSpecInterfaces...> :
        public virtual detail::ComposePlugin<
          SpecializedPlugin<SpecInterface1>,
          SpecializedPlugin<OtherSpecInterfaces...> >
    {
      // Declare friendship
      template <class...> friend class SpecializedPlugin;
      template <class...> friend class detail::ComposePlugin;
      template <class> friend class TemplatePluginPtr;

      /// \brief Default constructor
      private: SpecializedPlugin() = default;

      /// \brief Virtual destructor
      public: virtual ~SpecializedPlugin() = default;
    };
  }
}

#endif
