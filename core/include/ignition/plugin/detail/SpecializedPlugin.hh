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

        /// \brief Imperfect forwarding constructor. See the note in
        /// ComposePlugin<Base1,Base2> for why we do not use perfect forwarding.
        private: template <typename T>
                 ComposePlugin(const T &arg)
                   : Plugin(arg),
                     Base1(arg)
                 {
                   // Do nothing
                 }
      };

#define DETAIL_IGN_COMMON_COMPOSEPLUGIN_DISPATCH_IMPL(ReturnType, Function, Suffix, Args)\
  public:\
  template <class T>\
  ReturnType Function Suffix\
  {\
    if(Base1::template IsSpecializedFor<T>())\
      return Base1::template Function <T> Args ;\
  \
    return Base2::template Function <T> Args ;\
  }

      template <class Base1, class Base2>
      class ComposePlugin<Base1, Base2> : public virtual Base1, public virtual Base2
      {
        // Declare friendship
        template <class...> friend class SpecializedPlugin;
        template <class...> friend class ComposePlugin;

        /// \brief Default destructor
        public: virtual ~ComposePlugin() = default;

        // Implement the various functions that need to be dispatched to the
        // base classes.
        DETAIL_IGN_COMMON_COMPOSEPLUGIN_DISPATCH_IMPL(T*, GetInterface, (), ())
        DETAIL_IGN_COMMON_COMPOSEPLUGIN_DISPATCH_IMPL(const T*, GetInterface, () const, ())
        DETAIL_IGN_COMMON_COMPOSEPLUGIN_DISPATCH_IMPL(bool, HasInterface, () const, ())

        public: template<class T>
                static constexpr bool IsSpecializedFor()
        {
          return (Base1::template IsSpecializedFor<T>()
                  || Base2::template IsSpecializedFor<T>());
        }

        /// \brief Imperfect forwarding constructor. We do not use a perfect
        /// forwarding constructor here because arg will be getting split down
        /// two different branches in the inhertiance structure, so passing
        /// along an rvalue reference could be problematic. Nevertheless, this
        /// function is templated so that we can change the arguments used by
        /// the constructor of SpecializedPlugin without modifying this class.
        private: template <typename T>
                 ComposePlugin(const T &arg)
                   : Plugin(arg),
                     Base1(arg),
                     Base2(arg)
                 {
                   // Do nothing
                 }

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

        /// \brief Default destructor
        public: virtual ~ComposePlugin() = default;

        using Base = ComposePlugin<Base1, ComposePlugin<Base2, OtherBases...>>;

        /// \brief Imperfect forwarding constructor. See the note in
        /// ComposePlugin<Base1, Base2> for the reason that we do not use
        /// perfect forwarding.
        private: template <typename T>
                 ComposePlugin(const T &arg)
                   : Plugin(arg),
                     Base(arg)
                 {
                   // Do nothing
                 }

        private: ComposePlugin() = default;
      };
    } // namespace detail

    template <class SpecInterface1, class... OtherSpecInterfaces>
    class SpecializedPlugin<SpecInterface1, OtherSpecInterfaces...> :
        public virtual detail::ComposePlugin<
          SpecializedPlugin<SpecInterface1>,
          SpecializedPlugin<OtherSpecInterfaces...> >
    {
      // Declare friendship
      template <class...> friend class SpecializedPlugin;
      template <class...> friend class detail::ComposePlugin;
      friend class PluginLoader;

      /// \brief Default destructor
      public: virtual ~SpecializedPlugin() = default;

      using Base = detail::ComposePlugin<
                      SpecializedPlugin<SpecInterface1>,
                      SpecializedPlugin<OtherSpecInterfaces...> >;

      /// \brief Imperfect forwarding constructor. See the note in
      /// ComposePlugin<Base1, Base2> for the reason that we do not use
      /// perfect forwarding.
      private: template <typename T>
               SpecializedPlugin(const T &arg)
                 : Plugin(arg),
                   Base(arg)
               {
                 // Do nothing
               }

      /// \brief Default constructor
      private: SpecializedPlugin() = default;
    };
  }
}

#endif // IGNITION_COMMON_DETAIL_SPECIALIZEDPLUGIN_HH_
