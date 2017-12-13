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

#ifndef IGNITION_COMMON_PLUGINUTILS_HH_
#define IGNITION_COMMON_PLUGINUTILS_HH_

#include <string>
#include "ignition/common/StringUtils.hh"

namespace ignition
{
  namespace common
  {
    /// \brief Format the name to start with "::"
    /// \param[in] _name The name of a plugin or interface
    /// \return The input, but with "::" prepended if it was not there already.
    inline std::string NormalizeName(const std::string &_name)
    {
      std::string name = _name;
      if (!StartsWith(_name, "::"))
      {
        name = std::string("::") + _name;
      }
      return name;
    }
  }
}


#endif
