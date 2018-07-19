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
#define IGNITION_ADD_PLUGIN(...)\
  DETAIL_IGNITION_ADD_PLUGIN(__VA_ARGS__)

#endif
