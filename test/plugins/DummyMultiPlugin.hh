#ifndef IGNITION_PLUGIN_TEST_PLUGINS_DUMMYMULTIPLUGIN_HH_
#define IGNITION_PLUGIN_TEST_PLUGINS_DUMMYMULTIPLUGIN_HH_

#include <ignition/plugin/EnablePluginFromThis.hh>

#include "DummyPlugins.hh"

namespace test
{
namespace util
{

class DummyMultiPlugin
    : public DummyNameBase,
      public DummyDoubleBase,
      public DummyIntBase,
      public DummySetterBase,
      public DummyGetSomeObjectBase,
      public ignition::plugin::EnablePluginFromThis
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

#endif // DUMMYMULTIPLUGIN_HH
