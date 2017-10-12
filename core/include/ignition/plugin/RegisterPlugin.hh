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


#ifndef IGNITION_COMMON_REGISTERPLUGIN_HH_
#define IGNITION_COMMON_REGISTERPLUGIN_HH_

#include "ignition/common/detail/RegisterPlugin.hh"

// --------------- Specialize a plugin interface (optional) -------------------

/// \brief Call this macro inside a public scope of an interface in order to get
/// performance benefits for that interface in a SpecializedPlugin. Pass in
/// the fully qualified name of the interface class (i.e. explicitly include the
/// namespaces of the class).
///
/// Usage example:
///
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
///
/// Interfaces with this macro can be utilized by SpecializedPlugin to get
/// low cost access to this type of interface. Note that this performance
/// benefit is available even if the plugin that gets loaded does not offer this
/// interface; you just get low cost access to a nullptr instead. Always be sure
/// to verify the existence of an interface that you query from a plugin by
/// checking whether it's a nullptr!
#define IGN_COMMON_SPECIALIZE_INTERFACE(interfaceName)\
  DETAIL_IGN_COMMON_SPECIALIZE_INTERFACE(interfaceName)


// ------------- Add a set of plugins or a set of interfaces ------------------

/// \brief Add a plugin and interface from this shared library.
#define IGN_COMMON_ADD_PLUGIN(plugin, interface)\
  DETAIL_IGN_COMMON_ADD_PLUGIN(plugin, interface)



// -------------- Add a single plugin with a single interface -----------------

/// \brief Same as IGN_COMMON_ADD_PLUGIN(~,~), currently exists for backwards
/// compatibility.
#define IGN_COMMON_REGISTER_SINGLE_PLUGIN(plugin, interface) \
    IGN_COMMON_ADD_PLUGIN(plugin, interface)

#endif
