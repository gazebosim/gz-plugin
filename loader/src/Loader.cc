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
#include <dlfcn.h>
#include <functional>
#include <locale>
#include <sstream>

#include "ignition/common/PluginInfo.hh"
#include "ignition/common/PluginLoader.hh"

namespace ignition
{
namespace common
{
/////////////////////////////////////////////////
class PluginLoaderPrivate
{
  /// \brief Directories that should be searched for plugins
  public: std::vector<std::string> searchPaths;

  /// \brief A list of known plugins
  public: std::vector<PluginInfo> plugins;

  /// \brief format the name to start with "::"
  public: std::string NormalizeName(const std::string &_name) const;

  /// \brief format the path to use "/" as a separator with "/" at the end
  public: std::string NormalizePath(const std::string &_path) const;

  /// \brief generates paths to try searching for the named library
  public: std::vector<std::string> GenerateSearchNames(
              const std::string &_libName) const;

  /// \brief attempt to load a library at the given path
  public: void *LoadLibrary(const std::string &_full_path) const;

  /// \brief get plugin info for a library that has only one plugin
  public: PluginInfo GetSinglePlugin(void *_dlHandle) const;

  /// \brief return true if string starts with another string
  public: bool StartsWith(const std::string &_s1, const std::string &_s2) const;

  /// \brief return true if string ends with another string
  public: bool EndsWith(const std::string &_s1, const std::string &_s2) const;
};

/////////////////////////////////////////////////
std::string PluginLoader::PrettyStr() const
{
  auto searchPaths = this->SearchPaths();
  auto interfaces = this->InterfacesImplemented();
  std::stringstream pretty;
  pretty << "PluginLoader State" << std::endl;
  pretty << "\tSearch Paths: " << searchPaths.size() << std::endl;
  for (auto const &path : searchPaths)
    pretty << "\t\t" << path << std::endl;
  pretty << "\tKnown Interfaces: " << interfaces.size() << std::endl;
  for (auto const &interface : interfaces)
    pretty << "\t\t" << interface << std::endl;
  pretty << "\tKnown Plugins: " << interfaces.size() << std::endl;
  for (auto const &interface : interfaces)
  {
    for (auto const &plugin : this->PluginsImplementing(interface))
      pretty << "\t\t" << plugin << " (" << interface << ")" << std::endl;
  }
  return pretty.str();
}

/////////////////////////////////////////////////
PluginLoader::PluginLoader()
{
  this->dataPtr.reset(new PluginLoaderPrivate());
}

/////////////////////////////////////////////////
PluginLoader::~PluginLoader()
{
}

/////////////////////////////////////////////////
void PluginLoader::AddSearchPath(const std::string &_path)
{
  std::string path = this->dataPtr->NormalizePath(_path);
  auto begin = this->dataPtr->searchPaths.cbegin();
  auto end = this->dataPtr->searchPaths.cend();
  if (end == std::find(begin, end, path))
  {
    this->dataPtr->searchPaths.push_back(path);
  }
}

/////////////////////////////////////////////////
std::vector<std::string> PluginLoader::SearchPaths() const
{
  return this->dataPtr->searchPaths;
}

/////////////////////////////////////////////////
bool PluginLoader::LoadLibrary(const std::string &_libName)
{
  bool loadedLibrary = false;
  std::vector<std::string> searchNames =
    this->dataPtr->GenerateSearchNames(_libName);

  for (auto const &possibleName : searchNames)
  {
    // Attempt to load the library at this path
    void *dlHandle = this->dataPtr->LoadLibrary(possibleName);
    if (nullptr != dlHandle)
    {
      // Found a shared library, does it have the symbols we're looking for?
      PluginInfo plugin = this->dataPtr->GetSinglePlugin(dlHandle);
      if (plugin.name.size())
      {
        plugin.name = this->dataPtr->NormalizeName(plugin.name);
        plugin.interface = this->dataPtr->NormalizeName(plugin.interface);
        this->dataPtr->plugins.push_back(plugin);
        loadedLibrary = true;
      }
      break;
    }
  }
  return loadedLibrary;
}

/////////////////////////////////////////////////
std::vector<std::string> PluginLoader::InterfacesImplemented() const
{
  std::vector<std::string> interfaces;
  for (auto const &plugin : this->dataPtr->plugins)
  {
    if (interfaces.cend() == std::find(
          interfaces.cbegin(), interfaces.cend(), plugin.interface))
    {
      interfaces.push_back(plugin.interface);
    }
  }
  return interfaces;
}

/////////////////////////////////////////////////
std::vector<std::string> PluginLoader::PluginsImplementing(
    const std::string &_interface) const
{
  std::string interface = this->dataPtr->NormalizeName(_interface);
  std::vector<std::string> plugins;
  for (auto const &plugin : this->dataPtr->plugins)
  {
    if (plugin.interface == interface)
    {
      plugins.push_back(plugin.name);
    }
  }
  return plugins;
}

/////////////////////////////////////////////////
void *PluginLoader::Instantiate(
    const std::string &_name, std::size_t _baseId) const
{
  void *instance = nullptr;
  std::string name = this->dataPtr->NormalizeName(_name);
  for (auto const &plugin : this->dataPtr->plugins)
  {
    if (plugin.baseClassHash == _baseId && plugin.name == name)
    {
      // Creates plugin on heap
      instance = plugin.factory();
      break;
    }
  }
  return instance;
}

/////////////////////////////////////////////////
std::string PluginLoaderPrivate::NormalizeName(const std::string &_name) const
{
  std::string name = _name;
  if (!this->StartsWith(_name, "::"))
  {
    name = std::string("::") + _name;
  }
  return name;
}

/////////////////////////////////////////////////
std::string PluginLoaderPrivate::NormalizePath(const std::string &_path) const
{
  std::string path = _path;
  // Use '/' because it works on Linux, OSX, and Windows
  std::replace(path.begin(), path.end(), '\\', '/');
  // Make last character '/'
  if (!this->EndsWith(path, "/"))
  {
    path += '/';
  }
  return path;
}

/////////////////////////////////////////////////
bool PluginLoaderPrivate::StartsWith(
    const std::string &_s1, const std::string &_s2) const
{
  if (_s1.size() >= _s2.size())
  {
    return 0 == _s1.compare(0, _s2.size(), _s2);
  }
  return false;
}

/////////////////////////////////////////////////
bool PluginLoaderPrivate::EndsWith(
    const std::string &_s1, const std::string &_s2) const
{
  if (_s1.size() >= _s2.size())
  {
    return 0 == _s1.compare(_s1.size() - _s2.size(), _s2.size(), _s2);
  }
  return false;
}


/////////////////////////////////////////////////
std::vector<std::string> PluginLoaderPrivate::GenerateSearchNames(
    const std::string &_libName) const
{
  std::string lowercaseLibName = _libName;
  for (int i = 0; i < _libName.size(); ++i)
    lowercaseLibName[i] = std::tolower(_libName[i], std::locale());
  // test for possible prefixes or extensions on the library name
  bool hasLib = this->StartsWith(_libName, "lib");
  bool hasDotSo = this->EndsWith(lowercaseLibName, ".so");
  bool hasDotDll = this->EndsWith(lowercaseLibName, ".dll");
  bool hasDotDylib = this->EndsWith(lowercaseLibName, ".dylib");

  // Try removing non cross platform parts of names
  std::vector<std::string> initNames;
  initNames.push_back(_libName);
  if (hasLib && hasDotSo)
    initNames.push_back(_libName.substr(3, _libName.size() - 6));
  if (hasDotDll)
    initNames.push_back(_libName.substr(0, _libName.size() - 4));
  if (hasLib && hasDotDylib)
    initNames.push_back(_libName.substr(3, _libName.size() - 9));

  // Create possible basenames on different platforms
  std::vector<std::string> basenames;
  for (auto const &name : initNames)
  {
    basenames.push_back(name);
    basenames.push_back("lib" + name + ".so");
    basenames.push_back(name + ".so");
    basenames.push_back(name + ".dll");
    basenames.push_back("lib" + name + ".dylib");
    basenames.push_back(name + ".dylib");
    basenames.push_back("lib" + name + ".SO");
    basenames.push_back(name + ".SO");
    basenames.push_back(name + ".DLL");
    basenames.push_back("lib" + name + ".DYLIB");
    basenames.push_back(name + ".DYLIB");
  }

  std::vector<std::string> searchNames;
  // Concatenate these possible basenames with the search paths
  for (auto const &path : this->searchPaths)
  {
    for (auto const &name : basenames)
    {
      searchNames.push_back(path + name);
    }
  }
  return searchNames;
}

/////////////////////////////////////////////////
void* PluginLoaderPrivate::LoadLibrary(const std::string &_full_path) const
{
  // Somehow this works on windows builds?
  return dlopen(_full_path.c_str(), RTLD_LAZY|RTLD_GLOBAL);
}

/////////////////////////////////////////////////
PluginInfo PluginLoaderPrivate::GetSinglePlugin(void *_dlHandle) const
{
  PluginInfo plugin;
  if (nullptr != _dlHandle)
  {
    std::string versionSymbol = "IGNCOMMONPluginAPIVersion";
    std::string sizeSymbol = "IGNCOMMONSinglePluginInfoSize";
    std::string infoSymbol = "IGNCOMMONSinglePluginInfo";
    void *versionPtr = dlsym(_dlHandle, versionSymbol.c_str());
    void *sizePtr = dlsym(_dlHandle, sizeSymbol.c_str());
    void *infoPtr = dlsym(_dlHandle, infoSymbol.c_str());

    // Does the library have the right symbols?
    if (nullptr != versionPtr && nullptr != sizePtr && nullptr != infoPtr)
    {
      // Check abi version, and also check size because bugs happen
      int version = *(static_cast<int*>(versionPtr));
      std::size_t size = *(static_cast<std::size_t*>(sizePtr));
      if (PLUGIN_API_VERSION == version && sizeof(PluginInfo) == size)
      {
        PluginInfo (*GetInfo)() = reinterpret_cast<PluginInfo(*)()>(infoPtr);
        plugin = GetInfo();
      }
    }
  }
  return plugin;
}
}
}
