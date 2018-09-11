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

#include "../plugins/FactoryPlugins.hh"

using namespace test::util;

/////////////////////////////////////////////////
TEST(Factory, Inspect)
{
  ignition::plugin::Loader pl;
  pl.LoadLibrary(IGNFactoryPlugins_LIB);

  std::cout << pl.PrettyStr() << std::endl;

  EXPECT_EQ(2u, pl.PluginsImplementing<NameFactory>().size());
  EXPECT_EQ(2u, pl.PluginsImplementing<DoubleFactory>().size());
  EXPECT_EQ(2u, pl.PluginsImplementing<IntFactory>().size());
  EXPECT_EQ(2u, pl.PluginsImplementing<SomeObjectFactory>().size());
}

/////////////////////////////////////////////////
TEST(Factory, Construct)
{
  ignition::plugin::Loader pl;
  pl.LoadLibrary(IGNFactoryPlugins_LIB);

  auto nameFactory = pl.Factory<NameFactory>("test::util::DummyNameForward");
  ASSERT_NE(nullptr, nameFactory);
  EXPECT_EQ("John Doe", nameFactory->Construct("John Doe")->MyNameIs());

  nameFactory = pl.Factory<NameFactory>("test::util::DummyNameSayHello");
  ASSERT_NE(nullptr, nameFactory);
  EXPECT_EQ("Hello, John Doe!", nameFactory->Construct("John Doe")->MyNameIs());

  auto doubleFactory =
      pl.Factory<DoubleFactory>("test::util::DummyDoubleForward");
  ASSERT_NE(nullptr, doubleFactory);
  EXPECT_DOUBLE_EQ(5.3, doubleFactory->Construct(5.3)->MyDoubleValueIs());

  doubleFactory =
      pl.Factory<DoubleFactory>("test::util::DummyDoubleAddOneHalf");
  ASSERT_NE(nullptr, doubleFactory);
  EXPECT_DOUBLE_EQ(5.8, doubleFactory->Construct(5.3)->MyDoubleValueIs());

  auto intFactory =
      pl.Factory<IntFactory>("test::util::DummyIntForward");
  ASSERT_NE(nullptr, intFactory);
  EXPECT_EQ(68, intFactory->Construct(68)->MyIntegerValueIs());

  intFactory =
      pl.Factory<IntFactory>("test::util::DummyIntAddOne");
  ASSERT_NE(nullptr, intFactory);
  EXPECT_EQ(69, intFactory->Construct(68)->MyIntegerValueIs());

  auto someObjectFactory =
      pl.Factory<SomeObjectFactory>("test::util::SomeObjectForward");
  ASSERT_NE(nullptr, someObjectFactory);
  std::unique_ptr<SomeObject> someObject = someObjectFactory->Construct(7, 6.5);
  ASSERT_NE(nullptr, someObject);
  EXPECT_EQ(7, someObject->someInt);
  EXPECT_DOUBLE_EQ(6.5, someObject->someDouble);

  someObjectFactory =
      pl.Factory<SomeObjectFactory>("test::util::SomeObjectAddTwo");
  ASSERT_NE(nullptr, someObjectFactory);
  someObject = someObjectFactory->Construct(7, 6.5);
  ASSERT_NE(nullptr, someObject);
  EXPECT_EQ(9, someObject->someInt);
  EXPECT_DOUBLE_EQ(8.5, someObject->someDouble);

  auto nullFactory =
      pl.Factory<SomeObjectFactory>("not a real factory");
  EXPECT_EQ(nullptr, nullFactory);
}

/////////////////////////////////////////////////
TEST(Factory, Alias)
{
  ignition::plugin::Loader pl;
  pl.LoadLibrary(IGNFactoryPlugins_LIB);

  std::string symbolName;
  for (const std::string &name : pl.AllPlugins())
  {
    if (name.find("ignition::plugin::Factory") != std::string::npos &&
        name.find("test::util::SomeObjectAddTwo") != std::string::npos)
    {
      symbolName = name;
      break;
    }
  }

  EXPECT_FALSE(symbolName.empty());

  for (const std::string &alias : {
       symbolName.c_str(),
       "test::util::SomeObjectAddTwo",
       "This factory has an alias",
       "and also a second alias"})
  {
    std::shared_ptr<SomeObjectFactory> factory =
        pl.Factory<SomeObjectFactory>(alias);
    EXPECT_NE(nullptr, factory) << " failed on name: " << alias;
    if (!factory)
      continue;

    std::unique_ptr<SomeObject> object = factory->Construct(110, 2.25);
    ASSERT_NE(nullptr, object);
    EXPECT_EQ(112, object->someInt);
    EXPECT_DOUBLE_EQ(4.25, object->someDouble);
  }
}

/////////////////////////////////////////////////
int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
