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

#include "ignition/plugin/Register.hh"
#include "DummyPlugins.hh"
#include "DummyMultiPlugin.hh"

namespace test
{
namespace util
{

class DummyNoAliasPlugin : public DummyNameBase
{
  public: std::string MyNameIs() const override
  {
    return std::string("DummyNoAliasPlugin");
  }
};
IGNITION_ADD_PLUGIN(DummyNoAliasPlugin, DummyNameBase)

std::string DummySinglePlugin::MyNameIs() const
{
  return std::string("DummySinglePlugin");
}

std::string DummyMultiPlugin::MyNameIs() const
{
  return name;
}

double DummyMultiPlugin::MyDoubleValueIs() const
{
  return val;
}

int DummyMultiPlugin::MyIntegerValueIs() const
{
  return intVal;
}

std::unique_ptr<SomeObject> DummyMultiPlugin::GetSomeObject() const
{
  std::unique_ptr<SomeObject> object(new SomeObject);
  object->someInt = this->MyIntegerValueIs();
  object->someDouble = this->MyDoubleValueIs();

  return object;
}

void DummyMultiPlugin::SetName(const std::string &_name)
{
  name = _name;
}

void DummyMultiPlugin::SetDoubleValue(const double _val)
{
  val = _val;
}

void DummyMultiPlugin::SetIntegerValue(const int _val)
{
  intVal = _val;
}

std::shared_ptr<void> DummyMultiPlugin::PluginInstancePtr() const
{
  return this->PluginInstancePtrFromThis();
}

DummyMultiPlugin::DummyMultiPlugin()
  : name("DummyMultiPlugin"),
    val(3.14159),
    intVal(5)
{
  // Do nothing
}

// Show that we can add plugins from within a namespace
IGNITION_ADD_PLUGIN(
    DummyMultiPlugin,
    DummyGetSomeObjectBase,
    DummyGetPluginInstancePtr)

}
}

IGNITION_ADD_PLUGIN(test::util::DummySinglePlugin, test::util::DummyNameBase)
IGNITION_ADD_PLUGIN_ALIAS(test::util::DummySinglePlugin, "Alternative name")
IGNITION_ADD_PLUGIN_ALIAS(test::util::DummySinglePlugin, "Bar", "Baz")

IGNITION_ADD_PLUGIN(test::util::DummyMultiPlugin, test::util::DummyNameBase)
IGNITION_ADD_PLUGIN_ALIAS(test::util::DummyMultiPlugin, "Foo", "Bar", "Baz")
