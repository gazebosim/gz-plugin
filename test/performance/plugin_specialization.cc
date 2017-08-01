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

#include <chrono>
#include <iomanip>

#include <gtest/gtest.h>

#include "ignition/common/PluginLoader.hh"
#include "ignition/common/SystemPaths.hh"
#include "ignition/common/SpecializedPluginPtr.hh"
#include "ignition/common/Console.hh"

#include "test_config.h"
#include "util/DummyPlugins.hh"


#define IGN_CREATE_SPEC_INTERFACE(name)\
  class name { public: IGN_COMMON_SPECIALIZE_INTERFACE(name) };

IGN_CREATE_SPEC_INTERFACE(Interface1)
IGN_CREATE_SPEC_INTERFACE(Interface2)
IGN_CREATE_SPEC_INTERFACE(Interface3)
IGN_CREATE_SPEC_INTERFACE(Interface4)
IGN_CREATE_SPEC_INTERFACE(Interface5)
IGN_CREATE_SPEC_INTERFACE(Interface6)
IGN_CREATE_SPEC_INTERFACE(Interface7)
IGN_CREATE_SPEC_INTERFACE(Interface8)
IGN_CREATE_SPEC_INTERFACE(Interface9)
IGN_CREATE_SPEC_INTERFACE(Interface10)
IGN_CREATE_SPEC_INTERFACE(Interface11)
IGN_CREATE_SPEC_INTERFACE(Interface12)
IGN_CREATE_SPEC_INTERFACE(Interface13)


// Load this plugin type up with specialized interfaces to make sure that using
// many specializations does not have a noticeable impact on performance.
using SpecializedPluginPtr =
    ignition::common::SpecializedPluginPtr<
        Interface1,
        Interface2,
        Interface3,
        Interface4,
        Interface5,
        Interface6,
        Interface7,
        Interface8,
        Interface9,
        Interface10,
        Interface11,
        Interface12,
        Interface13,
        test::util::DummyIntBase,
        test::util::DummySetterBase>;

/////////////////////////////////////////////////
TEST(PluginSpecialization, AccessTime)
{
  std::string projectPath(PROJECT_BINARY_PATH);

  ignition::common::SystemPaths sp;
  sp.AddPluginPaths(projectPath + "/test/util");
  std::string path = sp.FindSharedLibrary("IGNDummyPlugins");
  ASSERT_FALSE(path.empty());

  ignition::common::PluginLoader pl;
  pl.LoadLibrary(path);

  // Load up the plugin
  SpecializedPluginPtr plugin =
      pl.Instantiate<SpecializedPluginPtr>("::test::util::DummyMultiPlugin");

  // Declare the variables that will be used to retrieve the interface pointers
  test::util::DummySetterBase *setterBase;
  test::util::DummyDoubleBase *doubleBase;

  const std::size_t NumTests = 10000;

  // Test the specialized case
  const auto spec_start = std::chrono::high_resolution_clock::now();
  for(std::size_t i=0; i < NumTests; ++i)
  {
    setterBase = plugin.GetInterface<test::util::DummySetterBase>();
  }
  const auto spec_finish = std::chrono::high_resolution_clock::now();

  // Test the normal case
  const auto norm_start = std::chrono::high_resolution_clock::now();
  for(std::size_t i=0; i < NumTests; ++i)
  {
    doubleBase = plugin.GetInterface<test::util::DummyDoubleBase>(
          "test::util::DummyDoubleBase");
  }
  const auto norm_finish = std::chrono::high_resolution_clock::now();

  const auto spec_time = std::chrono::duration_cast<std::chrono::nanoseconds>(
        spec_finish - spec_start).count();
  const auto norm_time = std::chrono::duration_cast<std::chrono::nanoseconds>(
        norm_finish - norm_start).count();

  const double spec_avg = (double)(spec_time)/(double)(NumTests);
  const double norm_avg = (double)(norm_time)/(double)(NumTests);

  std::cout << std::fixed;
  std::cout << std::setprecision(3);
  std::cout << std::right;

  std::cout << "Avg specialized time: "
         << std::setw(8) << std::right << spec_avg << " ns\n"
         << "Avg normal time:      "
         << std::setw(8) << std::right << norm_avg << " ns" << std::endl;

  // The time required for the specialized access should be much lower than the
  // time required by the normal access.
  EXPECT_LT(spec_time, norm_time);

  // Add this to make sure that the interfaces are working correctly, and to
  // prevent a warning about the pointers not being used.
  const double testDouble = 11.1;
  setterBase->SetDoubleValue(testDouble);
  EXPECT_NEAR(testDouble, doubleBase->MyDoubleValueIs(), 1e-8);
}


int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
