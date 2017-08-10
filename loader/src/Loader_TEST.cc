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

#include <algorithm>
#include <gtest/gtest.h>
#include "ignition/common/PluginLoader.hh"


/////////////////////////////////////////////////
TEST(PluginLoader, InitialNoInterfacesImplemented)
{
  ignition::common::PluginLoader pm;
  EXPECT_EQ(0u, pm.InterfacesImplemented().size());
}

/////////////////////////////////////////////////
TEST(PluginLoader, LoadNonexistantLibrary)
{
  ignition::common::PluginLoader pm;
  // We compare the output to an empty string so that the test prints out the
  // results in the event of a failure, which can be helpful for debugging.
  EXPECT_EQ(std::string(""), pm.LoadLibrary("/path/to/libDoesNotExist.so"));
}

/////////////////////////////////////////////////
int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
