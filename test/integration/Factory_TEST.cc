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

#include <gtest/gtest.h>

#include <ignition/plugin/Factory.hh>
#include <ignition/plugin/Loader.hh>

class SomeBase
{

};

class SomeDerived : public SomeBase
{

};

using MyFactoryNoArgs = ignition::plugin::Factory<SomeBase>;
using MyFactoryWithArgs = ignition::plugin::Factory<SomeBase, double, int>;

/////////////////////////////////////////////////
TEST(Factory, Producer)
{
  ignition::plugin::Loader pl;

  pl.Factory<MyFactoryNoArgs>("some::factory::name")->Construct();
  pl.Factory<MyFactoryWithArgs>("some::factory::name")->Construct(0.0, 1u);
}


/////////////////////////////////////////////////
int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
