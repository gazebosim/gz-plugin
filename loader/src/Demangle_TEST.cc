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

#include <gtest/gtest.h>

#include "Demangle.hh"

using namespace ignition::plugin;

class SomeSymbol { };

/////////////////////////////////////////////////
TEST(Demangle, RealSymbol)
{
  EXPECT_EQ("SomeSymbol", Demangle(typeid(SomeSymbol).name()));
}

template <typename T>
class SomeTemplate { };

/////////////////////////////////////////////////
TEST(Demangle, TemplatedSymbol)
{
  EXPECT_EQ("SomeTemplate<SomeSymbol>",
            Demangle(typeid(SomeTemplate<SomeSymbol>).name()));
}

/////////////////////////////////////////////////
TEST(Demangle, FakeSymbol)
{
#ifdef NDEBUG
  // When a fake symbol is passed in, we expect it to be returned exactly as it
  // was provided.
  EXPECT_EQ("NotReallyASymbol!@#$", Demangle("NotReallyASymbol!@#$"));
#else
  // If NDEBUG is not defined, we expect an assertion to be triggered, so we
  // will skip this test.
#endif
}

/////////////////////////////////////////////////
int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
