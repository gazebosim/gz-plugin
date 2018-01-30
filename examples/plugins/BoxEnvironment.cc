/*
 * Copyright (C) 2018 Open Source Robotics Foundation
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

#include "robot.hh"

namespace BoxEnvironment {

/// \brief A plugin that creates a box environment
class Plugin : public virtual ignition::common::examples::Environment
{
  public: ignition::common::examples::Layout GenerateLayout() const override
  {
    ignition::common::examples::Layout layout;
    const double L = 5.0;

    using ignition::math::Vector2d;

    layout.push_back(std::make_pair(Vector2d(L, L), Vector2d(L, -L)));
    layout.push_back(std::make_pair(Vector2d(L, L), Vector2d(-L, L)));
    layout.push_back(std::make_pair(Vector2d(-L, -L), Vector2d(L, -L)));
    layout.push_back(std::make_pair(Vector2d(-L, -L), Vector2d(-L, L)));

    return layout;
  }
};

}

IGN_COMMON_REGISTER_SINGLE_PLUGIN(
    BoxEnvironment::Plugin,
    ignition::common::examples::Environment)
