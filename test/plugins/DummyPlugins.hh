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


#ifndef IGNITION_COMMON_TEST_UTIL_DUMMY_PLUGINS_HH_
#define IGNITION_COMMON_TEST_UTIL_DUMMY_PLUGINS_HH_

#include <string>
#include <memory>

#include <ignition/common/Export.hh>
#include <ignition/common/PluginMacros.hh>

namespace test
{
namespace util
{

class DummyNameBase
{
  public: virtual std::string MyNameIs() const = 0;
};


class DummySinglePlugin : public DummyNameBase
{
  public: virtual std::string MyNameIs() const override;
};


class DummyDoubleBase
{
  public: virtual double MyDoubleValueIs() const = 0;
};

class DummyIntBase
{
  public: virtual int MyIntegerValueIs() const = 0;
  IGN_COMMON_SPECIALIZE_INTERFACE(test::util::DummyIntBase)
};

class DummySetterBase
{
  public: virtual void SetName(const std::string &_name) = 0;
  public: virtual void SetDoubleValue(const double _val) = 0;
  public: virtual void SetIntegerValue(const int _val) = 0;
  IGN_COMMON_SPECIALIZE_INTERFACE(test::util::DummySetterBase)
};

struct SomeObject
{
  int someInt;
  double someDouble;
};

class DummyGetSomeObjectBase
{
  public: virtual std::unique_ptr<SomeObject> GetSomeObject() const = 0;
  IGN_COMMON_SPECIALIZE_INTERFACE(test::util::DummyGetSomeObjectBase)
};

class DummyMultiPlugin
    : public DummyNameBase,
      public DummyDoubleBase,
      public DummyIntBase,
      public DummySetterBase,
      public DummyGetSomeObjectBase
{
  public: virtual std::string MyNameIs() const override;
  public: virtual double MyDoubleValueIs() const override;
  public: virtual int MyIntegerValueIs() const override;
  public: virtual std::unique_ptr<SomeObject> GetSomeObject() const override;

  public: virtual void SetName(const std::string &_name) override;
  public: virtual void SetDoubleValue(const double _val) override;
  public: virtual void SetIntegerValue(const int _val) override;

  public: DummyMultiPlugin();

  private: std::string name;
  private: double val;
  private: int intVal;
};

}
}


#endif
