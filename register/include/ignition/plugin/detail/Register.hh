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


#ifndef IGNITION_COMMON_DETAIL_REGISTERPLUGIN_HH_
#define IGNITION_COMMON_DETAIL_REGISTERPLUGIN_HH_

#include <set>
#include <string>
#include <typeinfo>
#include <type_traits>
#include <ignition/common/PluginInfo.hh>
#include <ignition/common/SuppressWarning.hh>


#if defined _WIN32 || defined __CYGWIN__
  #ifdef __GNUC__
    #define DETAIL_IGN_PLUGIN_VISIBLE __attribute__ ((dllexport))
  #else
    #define DETAIL_IGN_PLUGIN_VISIBLE __declspec(dllexport)
  #endif
#else
  #if __GNUC__ >= 4
    #define DETAIL_IGN_PLUGIN_VISIBLE __attribute__ ((visibility ("default")))
  #else
    #define DETAIL_IGN_PLUGIN_VISIBLE
  #endif
#endif

// extern "C" ensures that the symbol name of IGNCOMMONInputOrOutputPluginInfo
// does not get mangled by the compiler, so we can easily use dlsym(~) to
// retrieve it.
extern "C"
{
  /// \brief IGNCOMMONInputOrOutputPluginInfo is the hook that's used by the
  /// PluginLoader to retrieve PluginInfo from a shared library that provides
  /// plugins.
  ///
  /// It is declared inline so that the plugin registration macros can
  /// be used in arbitrarily many translation units without violating the one-
  /// definition rule.
  ///
  /// The symbol is explicitly exported (visibility is turned on) using
  /// DETAIL_IGN_PLUGIN_VISIBLE to ensure that dlsym(~) is able to find it.
  DETAIL_IGN_PLUGIN_VISIBLE inline void IGNCOMMONInputOrOutputPluginInfo(
      const void *_inputSingleInfo,
      const void ** const _outputAllInfo,
      int *_inputAndOutputAPIVersion,
      std::size_t *_inputAndOutputPluginInfoSize,
      std::size_t *_inputAndOutputPluginInfoAlign)
  {
    using PluginInfoMap = ignition::common::PluginInfoMap;
    // We use a static variable here so that we can accumulate multiple
    // PluginInfo objects from multiple plugin registration calls within one
    // shared library, and then provide it all to the PluginLoader through this
    // single hook. We store this static variable inside of an inlined function
    // in order to satisfy two requirements:
    //
    // 1. Each shared library that provides one or more plugins must have its
    //    own unique pluginMap object. That object will be initialized when the
    //    library is loaded and then destructed when the library is unloaded.
    //
    // 2. The pluginMap object can only be defined once per shared library (to
    //    satisfy the One-Definition Rule), but for user convenience we want its
    //    declaration and definition to all be provided in a header so that a
    //    simple macro call is enough to ensure that it works as intended.
    //
    // Due to some controversial idiosyncracies of GCC, this implementation
    // means that shared libraries which provide plugins should compile with the
    // flags -fkeep-inline-functions and -fno-gnu-unique.
    //
    //   -fkeep-inline-functions: Modern versions of GCC are supposed to have
    //       this option turned on by default, but explicitly specifying it
    //       seems to be more reliable.
    //
    //   -fno-gnu-unique: GCC has some irritating and controversial behavior
    //       where static variables in inline functions will use STB_GNU_UNIQUE
    //       binding. This binding tends to be problematic for plugin framework
    //       implementations, including ours. The most noticeable impact is that
    //       any library containing an object with STB_GNU_UNIQUE binding can
    //       never be unloaded until the program that's using it is terminated.
    //       In even worse cases, this binding could result in confusion between
    //       object symbols when multiple plugin libraries are loaded at once.
    //       Passing the -fno-gnu-unique flag tells the linker to never use the
    //       problematic STB_GNU_UNIQUE binding. Note that clang does not have
    //       this issue.
    //
    static PluginInfoMap pluginMap;

    if (_inputSingleInfo)
    {
      // When _inputSingleInfo is not a nullptr, it means that one of the plugin
      // registration macros is providing us with some PluginInfo.
      const ignition::common::PluginInfo *input =
          static_cast<const ignition::common::PluginInfo*>(_inputSingleInfo);

      PluginInfoMap::iterator it;
      bool inserted;

      // We use insert(~) to ensure that we do not accidentally overwrite some
      // existing information for the plugin that has this name.
      std::tie(it, inserted) =
          pluginMap.insert(std::make_pair(input->name, *input));

      if (!inserted)
      {
        // If the object was not inserted, then an entry already existed for
        // this plugin type. We should still insert each of the interface map
        // entries and aliases provided by the input info, just in case any of
        // them are missing from the currently existing entry. This allows the
        // user to specify different interfaces and aliases for the same plugin
        // type using different macros in different locations or across multiple
        // translation units.
        ignition::common::PluginInfo &entry = it->second;

        for (const auto &interfaceMapEntry : input->interfaces)
          entry.interfaces.insert(interfaceMapEntry);

        for (const auto &aliasSetEntry : input->aliases)
          entry.aliases.insert(aliasSetEntry);
      }
    }

    if (_outputAllInfo)
    {
      // When _outputAllInfo is not a nullptr, it means that a PluginLoader is
      // trying to retrieve PluginInfo from us.

      // The PluginLoader should provide valid pointers to these fields as part
      // of a handshake procedure.
      if (nullptr == _inputAndOutputAPIVersion ||
          nullptr == _inputAndOutputPluginInfoSize ||
          nullptr == _inputAndOutputPluginInfoAlign)
      {
        // This should never happen, or else the function is being misused.
        return;
      }

      bool agreement = true;

      if (ignition::common::PLUGIN_API_VERSION
          != *_inputAndOutputAPIVersion)
      {
        agreement = false;
      }

      if (sizeof(ignition::common::PluginInfo)
          != *_inputAndOutputPluginInfoSize)
      {
        agreement = false;
      }

      if (alignof(ignition::common::PluginInfo)
          != *_inputAndOutputPluginInfoAlign)
      {
        agreement = false;
      }

      // The handshake parameters that were passed into us are overwritten with
      // the values that we have on our end. That way, if our PluginInfo API is
      // lower than that of the PluginLoader, then the PluginLoader will know
      // to call this function using an older version of PluginInfo, and then
      // convert it to the newer version on the loader side.
      *_inputAndOutputAPIVersion = ignition::common::PLUGIN_API_VERSION;
      *_inputAndOutputPluginInfoSize = sizeof(ignition::common::PluginInfo);
      *_inputAndOutputPluginInfoAlign = alignof(ignition::common::PluginInfo);

      // If the size, alignment, or API do not agree, we should return without
      // outputting any of the plugin info; otherwise, we could get a
      // segmentation fault.
      //
      // We will return the current API version to the PluginLoader, and it may
      // then decide to attempt the call to this function again with the correct
      // API version if it supports backwards/forwards compatibility.
      if (!agreement)
        return;

      *_outputAllInfo = &pluginMap;
    }
  }
}

namespace ignition
{
  namespace common
  {
    namespace detail
    {
      //////////////////////////////////////////////////
      /// \brief This default will be called when NoMoreInterfaces is an empty
      /// parameter pack. When one or more Interfaces are provided, the other
      /// template specialization of this class will be called.
      template <typename PluginClass, typename... NoMoreInterfaces>
      struct InterfaceHelper
      {
        public: static void InsertInterfaces(PluginInfo::InterfaceCastingMap &)
        {
          // Do nothing. This is the terminal specialization of the variadic
          // template class member function.
        }
      };

      //////////////////////////////////////////////////
      /// \brief This specialization will be called when one or more Interfaces
      /// are specified.
      template <typename PluginClass, typename Interface,
                typename... RemainingInterfaces>
      struct InterfaceHelper<PluginClass, Interface, RemainingInterfaces...>
      {
        public: static void InsertInterfaces(
          PluginInfo::InterfaceCastingMap &interfaces)
        {
          // READ ME: If you get a compilation error here, then one of the
          // interfaces that you tried to register for your plugin is not
          // actually a base class of the plugin class. This is not allowed. A
          // plugin class must inherit every interface class that you want it to
          // provide.
          static_assert(std::is_base_of<Interface, PluginClass>::value,
                        "YOU ARE ATTEMPTING TO REGISTER AN INTERFACE FOR A "
                        "PLUGIN, BUT THE INTERFACE IS NOT A BASE CLASS OF THE "
                        "PLUGIN.");

          interfaces.insert(std::make_pair(
                typeid(Interface).name(),
                [=](void* v_ptr)
                {
                    PluginClass *d_ptr = static_cast<PluginClass*>(v_ptr);
                    return static_cast<Interface*>(d_ptr);
                }));

          InterfaceHelper<PluginClass, RemainingInterfaces...>
              ::InsertInterfaces(interfaces);
        }
      };

      //////////////////////////////////////////////////
      /// \brief This overload will be called when no more aliases remain to be
      /// inserted. If one or more aliases still need to be inserted, then the
      /// overload below this one will be called instead.
      inline void InsertAlias(std::set<std::string> &/*aliases*/)
      {
        // Do nothing. This is the terminal overload of the variadic template
        // function.
      }

      template <typename... Aliases>
      void InsertAlias(std::set<std::string> &aliases,
                       const std::string &nextAlias,
                       Aliases&&... remainingAliases)
      {
        aliases.insert(nextAlias);
        InsertAlias(aliases, std::forward<Aliases>(remainingAliases)...);
      }

      //////////////////////////////////////////////////
      /// \brief This default specialization of the Registrar class will be
      /// called when no arguments are provided to the IGN_COMMON_ADD_PLUGIN()
      /// macro. This is not allowed and will result in a compilation error.
      template <typename... NoArguments>
      struct Registrar
      {
        public: static void RegisterPlugin()
        {
          // READ ME: If you get a compilation error here, then you have
          // attempted to call IGN_COMMON_ADD_PLUGIN() with no arguments. This
          // is both pointless and not permitted. Either specify a plugin class
          // to register, or else do not call the macro.
          static_assert(sizeof...(NoArguments) > 0,
                        "YOU ARE ATTEMPTING TO CALL IGN_COMMON_ADD_PLUGIN "
                        "WITHOUT SPECIFYING A PLUGIN CLASS");



          // --------------------------------------------------------------- //
          // Dev Note (MXG): The following static assert should never fail, or
          // else there is a bug in our variadic template implementation. If a
          // compilation failure occurs in this function, it should happen at
          // the previous static_assert. If the parameter pack `NoArguments`
          // contains one or more types, then the other template specialization
          // of the Registrar class should be chosen, instead of this default
          // one. This static_assert is only here as reassurance that the
          // implementation is correct.
          static_assert(sizeof...(NoArguments) == 0,
                        "THERE IS A BUG IN THE PLUGIN REGISTRATION "
                        "IMPLEMENTATION! PLEASE REPORT THIS!");
          // --------------------------------------------------------------- //
        }
      };

      //////////////////////////////////////////////////
      /// \brief This specialization of the Register class will be called when
      /// one or more arguments are provided to the IGN_COMMON_ADD_PLUGIN(~)
      /// macro. This is the only version of the Registrar class that is allowed
      /// to compile.
      template <typename PluginClass, typename... Interfaces>
      struct Registrar<PluginClass, Interfaces...>
      {
        // READ ME: For a plugin to be registered and loadable, it must be a
        // concrete class (as opposed to an abstract class). This means the
        // plugin class must provide an implementation (or inherit an
        // implementation) for each pure virtual function that it inherits from
        // its parent classes, and it must not declare any pure virtual
        // functions itself. Abstract classes can never be instantiated, per the
        // rules of the C++ language.
        static_assert(!std::is_abstract<PluginClass>::value,
                      "YOU ARE ATTEMPTING TO REGISTER A PURE ABSTRACT CLASS "
                      "AS A PLUGIN. THIS IS NOT ALLOWED.");

        public: static PluginInfo MakePluginInfo()
        {
          PluginInfo info;

          // Set the name of the plugin
          info.name = typeid(PluginClass).name();

          // Create a factory for generating new plugin instances
          info.factory = [=]()
          {
            return static_cast<void*>(new PluginClass);
          };

IGN_COMMON_WARN_IGNORE__DELETE_NON_VIRTUAL_DESTRUCTOR
          // Create a deleter to clean up destroyed instances
          info.deleter = [=](void *ptr)
          {
            delete static_cast<PluginClass*>(ptr);
          };
IGN_COMMON_WARN_RESUME__DELETE_NON_VIRTUAL_DESTRUCTOR

          // Construct a map from the plugin to its interfaces
          InterfaceHelper<PluginClass, Interfaces...>
              ::InsertInterfaces(info.interfaces);

          return info;
        }

        /// \brief This function registers a plugin along with a set of
        /// interfaces that it provides.
        public: static void RegisterPlugin()
        {
          PluginInfo info = MakePluginInfo();

          // Send this information as input to this library's global repository
          // of plugins.
          IGNCOMMONInputOrOutputPluginInfo(
                &info, nullptr, nullptr, nullptr, nullptr);
        }


        public: template <typename... Aliases>
        static void RegisterAlias(Aliases&&... aliases)
        {
          // Dev note (MXG): We expect the RegisterAlias function to be called
          // using the IGN_COMMON_ADD_ALIAS(~) macro, which should never contain
          // any interfaces. Therefore, this parameter pack should be empty.
          //
          // In the future, we could allow Interfaces and Aliases to be
          // specified simultaneously, but that would be very tricky to do with
          // macros, so for now we will enforce this assumption to make sure
          // that the implementation is working as expected.
          static_assert(sizeof...(Interfaces) == 0,
                        "THERE IS A BUG IN THE ALIAS REGISTRATION "
                        "IMPLEMENTATION! PLEASE REPORT THIS!");

          PluginInfo info = MakePluginInfo();

          // Gather up all the aliases that have been specified for this plugin.
          InsertAlias(info.aliases, std::forward<Aliases>(aliases)...);

          // Send this information as input to this library's global repository
          // of plugins.
          IGNCOMMONInputOrOutputPluginInfo(
                &info, nullptr, nullptr, nullptr, nullptr);
        }
      };
    }
  }
}

//////////////////////////////////////////////////
/// This macro creates a uniquely-named class whose constructor calls the
/// ignition::common::detail::RegisterPlugin function. It then declares a
/// uniquely-named instance of the class with static lifetime. Since the class
/// instance has a static lifetime, it will be constructed when the shared
/// library is loaded. When it is constructed, the RegisterPlugin function will
/// be called
#define DETAIL_IGN_COMMON_ADD_PLUGIN_HELPER(UniqueID, ...) \
  namespace ignition \
  { \
    namespace common \
    { \
      namespace \
      { \
        struct ExecuteWhenLoadingLibrary##UniqueID \
        { \
          ExecuteWhenLoadingLibrary##UniqueID() \
          { \
            ::ignition::common::detail::Registrar<__VA_ARGS__>:: \
                RegisterPlugin(); \
          } \
        }; \
  \
        static ExecuteWhenLoadingLibrary##UniqueID execute##UniqueID; \
      } \
    } \
  }


//////////////////////////////////////////////////
/// This macro is needed to force the __COUNTER__ macro to expand to a value
/// before being passed to the *_HELPER macro.
#define DETAIL_IGN_COMMON_ADD_PLUGIN_WITH_COUNTER(UniqueID, ...) \
  DETAIL_IGN_COMMON_ADD_PLUGIN_HELPER(UniqueID, __VA_ARGS__)


//////////////////////////////////////////////////
/// We use the __COUNTER__ here to give each plugin instantiation its own unique
/// name, which is required in order to statically initialize each one.
#define DETAIL_IGN_COMMON_ADD_PLUGIN(...) \
  DETAIL_IGN_COMMON_ADD_PLUGIN_WITH_COUNTER(__COUNTER__, __VA_ARGS__)


#endif
