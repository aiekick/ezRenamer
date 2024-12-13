#pragma once

#include <set>
#include <map>
#include <array>
#include <vector>
#include <string>
#include <cstdint>

#include <apis/ezRenamerPluginApi.h>

typedef std::string PluginName;
typedef std::string PluginPath;
typedef std::map<PluginName, rnm::RenamerModulePtr> RenamerContainer;
