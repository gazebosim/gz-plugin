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
#include <sstream>

#include "ignition/common/plugin/PluginInfo.hh"
#include "ignition/common/plugin/PluginLoader.hh"

namespace ignition
{
namespace common
{
namespace plugin
{

/////////////////////////////////////////////////
class PluginLoaderPrivate
{
  public:
    /// \brief Directories that should be searched for plugins
    std::vector<std::string> searchPaths;

    /// \brief A list of known plugins
    std::vector<PluginInfo> plugins;

    /// \brief format the name to start with "::"
    std::string NormalizeName(std::string _name) const;

    /// \brief format the path to use "/" as a separator with "/" at the end
    std::string NormalizePath(std::string _path) const;

    /// \brief generates paths to try searching for the named library
    std::vector<std::string> GenerateSearchNames(std::string _libName) const;

    /// \brief attempt to load a library at the given path
    void* LoadLibrary(std::string _full_path) const;

    /// \brief get plugin info for a library that has only one plugin
    PluginInfo GetSinglePlugin(void *_dlHandle) const;

    /// \brief return true if string starts with another string
    bool StartsWith(std::string _s1, std::string _s2) const;

    /// \brief return true if string ends with another string
    bool EndsWith(std::string _s1, std::string _s2) const;
};

/////////////////////////////////////////////////
std::string PluginLoader::PrettyStr() const
{
  auto searchPaths = this->SearchPaths();
  auto interfaces = this->InterfacesImplemented();
  std::stringstream pretty;
  pretty << "PluginLoader State" << std::endl;
  pretty << "\tSearch Paths: " << searchPaths.size() << std::endl;
  for (auto path : searchPaths)
    pretty << "\t\t" << path << std::endl;
  pretty << "\tKnown Interfaces: " << interfaces.size() << std::endl;
  for (auto interface : interfaces)
    pretty << "\t\t" << interface << std::endl;
  pretty << "\tKnown Plugins: " << interfaces.size() << std::endl;
  for (auto interface : interfaces)
  {
    auto plugins = this->PluginsImplementing(interface);
    for (auto plugin : plugins)
      pretty << "\t\t" << plugin << " (" << interface << ")" << std::endl;
  }
  return pretty.str();
}

/////////////////////////////////////////////////
PluginLoader::PluginLoader()
{
  this->impl.reset(new PluginLoaderPrivate());
}

/////////////////////////////////////////////////
PluginLoader::~PluginLoader()
{
}

/////////////////////////////////////////////////
void PluginLoader::AddSearchPath(std::string _path)
{
  std::string path = this->impl->NormalizePath(_path);
  auto begin = this->impl->searchPaths.cbegin();
  auto end = this->impl->searchPaths.cend();
  if (end == std::find(begin, end, path))
  {
    this->impl->searchPaths.push_back(path);
  }
}

/////////////////////////////////////////////////
std::vector<std::string> PluginLoader::SearchPaths() const
{
  return this->impl->searchPaths;
}

/////////////////////////////////////////////////
bool PluginLoader::LoadLibrary(std::string _libName)
{
  bool loadedLibrary = false;
  std::vector<std::string> searchNames = this->impl->GenerateSearchNames(_libName);
  
  for (auto iter = searchNames.cbegin(); iter != searchNames.cend(); ++iter)
  {
    // Attempt to load the library at this path
    void *dlHandle = this->impl->LoadLibrary(*iter);
    if (nullptr != dlHandle)
    {
      // Found a shared library, does it have the symbols we're looking for?
      PluginInfo plugin = this->impl->GetSinglePlugin(dlHandle);
      if (plugin.name.size())
      {
        plugin.name = this->impl->NormalizeName(plugin.name);
        plugin.interface = this->impl->NormalizeName(plugin.interface);
        this->impl->plugins.push_back(plugin);
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
  auto begin = this->impl->plugins.cbegin();
  auto end = this->impl->plugins.cend();
  for (auto pluginIter = begin; pluginIter != end; ++pluginIter)
  {
    if (interfaces.cend() == std::find(
          interfaces.cbegin(), interfaces.cend(), pluginIter->interface))
    {
      interfaces.push_back(pluginIter->interface);
    }
  }
  return interfaces;
}

/////////////////////////////////////////////////
std::vector<std::string> PluginLoader::PluginsImplementing(std::string _interface) const
{
  std::string interface = this->impl->NormalizeName(_interface);
  std::vector<std::string> plugins;
  auto begin = this->impl->plugins.cbegin();
  auto end = this->impl->plugins.cend();
  for (auto pluginIter = begin; pluginIter != end; ++pluginIter)
  {
    if (pluginIter->interface == interface)
    {
      plugins.push_back(pluginIter->name);
    }
  }
  return plugins;
}

/////////////////////////////////////////////////
void* PluginLoader::Instantiate(std::string _name, std::size_t _baseId) const
{
  void *plugin = nullptr;
  std::string name = this->impl->NormalizeName(_name);
  std::vector<std::string> plugins;
  auto begin = this->impl->plugins.cbegin();
  auto end = this->impl->plugins.cend();
  for (auto pluginIter = begin; pluginIter != end; ++pluginIter)
  {
    if (pluginIter->baseClassHash == _baseId && pluginIter->name == name)
    {
      // Creates plugin on heap
      plugin = pluginIter->factory();
      break;
    }
  }
  return plugin;
}

/////////////////////////////////////////////////
std::string PluginLoaderPrivate::NormalizeName(std::string _name) const
{
  if (!this->StartsWith(_name, "::"))
  {
    _name = std::string("::") + _name;
  }
  return _name;
}

/////////////////////////////////////////////////
std::string PluginLoaderPrivate::NormalizePath(std::string _path) const
{
  // Use '/' because it works on Linux, OSX, and Windows
  std::replace(_path.begin(), _path.end(), '\\', '/');
  //Make last character '/'
  if (!this->EndsWith(_path, "/"))
  {
    _path += '/';
  }
  return _path;
}

/////////////////////////////////////////////////
bool PluginLoaderPrivate::StartsWith(std::string _s1, std::string _s2) const
{
  bool result = false;
  if (_s1.size() >= _s2.size())
  {
    if (0 == _s1.compare(0, _s2.size(), _s2))
    {
      result = true;
    }
  }
  return result;
}

/////////////////////////////////////////////////
bool PluginLoaderPrivate::EndsWith(std::string _s1, std::string _s2) const
{
  bool result = false;
  if (_s1.size() >= _s2.size())
  {
    if (0 == _s1.compare(_s1.size() - _s2.size(), _s2.size(), _s2))
    {
      result = true;
    }
  }
  return result;
}


/////////////////////////////////////////////////
std::vector<std::string> PluginLoaderPrivate::GenerateSearchNames(
    std::string _libName) const
{
  // figure out all possible prefixes or extensions the library name could have
  bool hasLib = this->StartsWith(_libName, "lib");
  bool hasDotSo = this->EndsWith(_libName, ".so");
  bool hasDotDll = this->EndsWith(_libName, ".dll");
  bool hasDotDylib = this->EndsWith(_libName, ".dylib");

  // Try removing non cross platform parts of names
  std::vector<std::string> initNames;
  initNames.push_back(_libName);
  if (hasLib && hasDotSo)
    initNames.push_back(_libName.substr(3, _libName.size() - 6));
  if (hasDotDll)
    initNames.push_back(_libName.substr(0, _libName.size() - 3));
  if (hasLib && hasDotDylib)
    initNames.push_back(_libName.substr(3, _libName.size() - 9));

  // Create possible basenames on different platforms
  std::vector<std::string> basenames;
  for (auto iter = initNames.cbegin(); iter != initNames.cend(); ++iter)
  {
    basenames.push_back(*iter);
    basenames.push_back("lib" + *iter + ".so");
    basenames.push_back(*iter + ".so");
    basenames.push_back(*iter + ".dll");
    basenames.push_back("lib" + *iter + ".dylib");
    basenames.push_back(*iter + ".dylib");
  }

  std::vector<std::string> searchNames;
  // Concatenate these possible basenames with the search paths
  auto pathsBegin = this->searchPaths.cbegin();
  auto pathsEnd = this->searchPaths.cend();
  auto namesBegin = basenames.cbegin();
  auto namesEnd = basenames.cend();
  for (auto pathsIter = pathsBegin; pathsIter != pathsEnd; ++pathsIter)
  {
    for (auto namesIter = namesBegin; namesIter != namesEnd; ++namesIter)
    {
      searchNames.push_back(*pathsIter + *namesIter);
    }
  }
  return searchNames;
}

/////////////////////////////////////////////////
void* PluginLoaderPrivate::LoadLibrary(std::string _full_path) const
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
}
