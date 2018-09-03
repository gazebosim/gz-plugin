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


#ifndef IGNITION_PLUGIN_REGISTER_HH_
#define IGNITION_PLUGIN_REGISTER_HH_

#include <ignition/plugin/detail/Register.hh>


// ------------- Add a set of plugins or a set of interfaces ------------------

/// \brief Add a plugin and interface from this shared library.
///
/// This macro can be put in any namespace and may be called any number of
/// times. It can be called multiple times on the same plugin class in order to
/// register multiple interfaces, e.g.:
///
/// \code
/// IGNITION_ADD_PLUGIN(PluginClass, Interface1)
/// IGNITION_ADD_PLUGIN(PluginClass, Interface2)
///
/// /* Some other code */
///
/// IGNITION_ADD_PLUGIN(PluginClass, Interface3)
/// \endcode
///
/// Or you can list multiple interfaces in a single call to the macro, e.g.:
///
/// \code
/// IGNITION_ADD_PLUGIN(PluginClass, Interface1, Interface2, Interface3)
/// \endcode
///
/// If your library has multiple translation units (.cpp files) and you want to
/// register plugins in multiple translation units, use this
/// ignition/plugin/Register.hh header in ONE of the translation units, and then
/// the ignition/plugin/RegisterMore.hh header in all of the rest of the
/// translation units.
#define IGNITION_ADD_PLUGIN(...) \
  DETAIL_IGNITION_ADD_PLUGIN(__VA_ARGS__)


#define IGNITION_ADD_FACTORY(ProductType, FactoryType) \
  DETAIL_IGNITION_ADD_FACTORY(ProductType, FactoryType)


#endif
