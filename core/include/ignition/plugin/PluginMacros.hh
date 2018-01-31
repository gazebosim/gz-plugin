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


#ifndef IGNITION_COMMON_REGISTERMACROS_HH_
#define IGNITION_COMMON_REGISTERMACROS_HH_

#include "ignition/common/detail/PluginMacros.hh"

// --------------- Specialize a plugin interface (optional) -------------------

/// \brief Call this macro inside a public scope of an interface in order to get
/// performance benefits for that interface in a SpecializedPlugin. Pass in
/// the fully qualified name of the interface class (i.e. explicitly include the
/// namespaces of the class).
///
/// Usage example:
///
/// \code
///     namespace mylibrary {
///       namespace ns {
///         class SomeInterface
///         {
///           public:
///             IGN_COMMON_SPECIALIZE_INTERFACE(mylibrary::ns::SomeInterface)
///           // ... declarations of interface functions ...
///         };
///       } // namespace ns
///     } // namespace mylibrary
/// \endcode
///
/// Interfaces with this macro can be utilized by SpecializedPlugin to get
/// high-speed access to this type of interface. Note that this performance
/// benefit is available even if the plugin that gets loaded does not offer this
/// interface; you just get high-speed access to a nullptr instead. Always be
/// sure to verify the existence of an interface that you query from a plugin by
/// checking whether it's a nullptr!
#define IGN_COMMON_SPECIALIZE_INTERFACE(interfaceName)\
  DETAIL_IGN_COMMON_SPECIALIZE_INTERFACE(interfaceName)


// ------------- Add a set of plugins or a set of interfaces ------------------

// The following three macros can be used to produce multiple plugins and/or
// multiple interfaces from your shared library.

/* Usage example for multiple plugins and one interface:
 *
 *     IGN_COMMON_BEGIN_ADDING_PLUGINS
 *       IGN_COMMON_ADD_PLUGIN(mylibrary::ns::SomeClass1, mylibrary::ns::SomeInterface)
 *       IGN_COMMON_ADD_PLUGIN(mylibrary::ns::SomeClass2, mylibrary::ns::SomeInterface)
 *     IGN_COMMON_FINISH_ADDING_PLUGINS
 *
 *
 * Usage example for one plugin and multiple interfaces:
 *
 *     IGN_COMMON_BEGIN_ADDING_PLUGINS
 *       IGN_COMMON_ADD_PLUGIN(mylibrary::ns::SomeClass, mylibrary::ns::Interface1)
 *       IGN_COMMON_ADD_PLUGIN(mylibrary::ns::SomeClass, mylibrary::ns::Interface2)
 *     IGN_COMMON_FINISH_ADDING_PLUGINS
 *
 *
 * Usage example for multiple plugins and multiple interfaces:
 *
 *     IGN_COMMON_BEGIN_ADDING_PLUGINS
 *       IGN_COMMON_ADD_PLUGIN(mylibrary::ns::SomeClass, mylibrary::ns::FooInterface)
 *       IGN_COMMON_ADD_PLUGIN(mylibrary::ns::SomeOtherClass, mylibrary::ns::BarInterface)
 *       IGN_COMMON_ADD_PLUGIN(mylibrary::ns::SomeClass, mylibrary::ns::SomeInterface)
 *       IGN_COMMON_ADD_PLUGIN(mylibrary::ns::SomeOtherClass, mylibrary::ns::FooInterface)
 *       IGN_COMMON_ADD_PLUGIN(mylibrary::ns::SomeOtherClass, mylibrary::ns::SomeInterface)
 *     IGN_COMMON_FINISH_ADDING_PLUGINS
 */

// Note that the order in which (Plugin, Interface) pairs are added does not
// matter. Multiple plugins are allowed to provide the same interface. There is
// no inherent limit on the number of interfaces or plugins that are allowed.
//
// These macros MUST be called in the global namespace in a source file of your
// library's codebase. A single library is only allowed to have ONE block of
// these macros (i.e. if you find yourself invoking
// IGN_COMMON_BEGIN_ADDING_PLUGINS more than once in your library, then you are
// doing something that is not permitted). Recommended practice is to have a
// single source file (e.g. plugins.cpp) in your library's codebase that is
// dedicated to hosting these macros.


/// \brief Begin registering a set of plugins that are contained within this
/// shared library. After invoking this macro, use a sequence of calls to
/// IGN_COMMON_ADD_PLUGIN(~,~), passing in a different plugin and interface name
/// to each call. When all the plugins and interfaces have been added, call
/// IGN_COMMON_FINISH_ADDING_PLUGINS.
///
/// Be sure to only use this macro in the global namespace, and only use it once
/// in your library.
#define IGN_COMMON_BEGIN_ADDING_PLUGINS\
  DETAIL_IGN_COMMON_BEGIN_ADDING_PLUGINS


/// \brief Add a plugin and interface from this shared library. This macro must
/// be called consecutively on each class that this shared library wants to
/// provide as a plugin or interface. This macro must be called in between
/// IGN_COMMON_BEGIN_ADDING_PLUGINS and IGN_COMMON_FINISH_ADDING_PLUGINS. If a
/// plugin provides multiple interfaces, then simply call this macro repeatedly
/// on the plugin, once for each interface. The multiple interfaces will
/// automatically be collapsed into one plugin that provides all of them.
#define IGN_COMMON_ADD_PLUGIN(plugin, interface)\
  DETAIL_IGN_COMMON_ADD_PLUGIN(plugin, interface)


/// \brief Call this macro after all calls to IGN_COMMON_ADD_PLUGIN have been
/// finished.
#define IGN_COMMON_FINISH_ADDING_PLUGINS\
  DETAIL_IGN_COMMON_FINISH_ADDING_PLUGINS


// -------------- Add a single plugin with a single interface -----------------

/// \brief Register a shared library with only one plugin and one interface.
/// This macro is NOT compatible with IGN_COMMON_ADD_PLUGIN or any of the
/// above macros for adding multiple plugins and/or multiple interfaces. This is
/// simply a convenience function if you want to add one plugin with one
/// interface from your library.
#define IGN_COMMON_REGISTER_SINGLE_PLUGIN(plugin, interface) \
  IGN_COMMON_BEGIN_ADDING_PLUGINS\
    IGN_COMMON_ADD_PLUGIN(plugin, interface)\
  IGN_COMMON_FINISH_ADDING_PLUGINS

#endif
