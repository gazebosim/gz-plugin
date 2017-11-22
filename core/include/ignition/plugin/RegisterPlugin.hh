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


// ------------- Add a set of plugins or a set of interfaces ------------------

/// \brief Add a plugin and interface from this shared library.
#define IGN_COMMON_ADD_PLUGIN(plugin, interface)\
  DETAIL_IGN_COMMON_ADD_PLUGIN(plugin, interface)



// -------------- Add a single plugin with a single interface -----------------

/// \brief Same as IGN_COMMON_ADD_PLUGIN(~,~). This macro currently exists for
/// backwards API compatibility.
#define IGN_COMMON_REGISTER_SINGLE_PLUGIN(plugin, interface) \
    IGN_COMMON_ADD_PLUGIN(plugin, interface)

#endif
