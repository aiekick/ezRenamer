/*
Copyright 2024 Stephane Cuillerdier (aka aiekick)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#pragma once
#pragma warning(disable : 4251)

#include <memory>
#include <cstdint>
#include <string>
#include <vector>
#include <array>

#include "ILayoutPane.h"
#include <ezlibs/ezXml.hpp>

namespace rnm {

struct PluginBridge {};

struct PluginModule {
    virtual ~PluginModule() = default;
    virtual bool init() = 0;
    virtual void unit() = 0;
};

typedef std::shared_ptr<PluginModule> PluginModulePtr;
typedef std::weak_ptr<PluginModule> PluginModuleWeak;

typedef std::string SettingsCategoryPath;
enum class ISettingsType {
    NONE = 0,
    APP,     // common for all users
    PROJECT  // user specific
};

struct IXmlSettings {
    // will be called by the saver
    virtual ez::xml::Nodes getXmlSettings(const ISettingsType& vType) const = 0;
    // will be called by the loader
    virtual void setXmlSettings(const ez::xml::Node& vName, const ez::xml::Node& vParent, const std::string& vValue, const ISettingsType& vType) = 0;
};

struct PathDatas {
	std::vector<std::string> paths;
};

struct RenamerModule : PluginModule {
    virtual ~RenamerModule() = default;
    // will rename paths. return true if successfull
    virtual bool rename(const PathDatas& vPathToRename, PathDatas& vOutRenamedpaths) = 0;
    // draw ImGui plugin controls. return true if changed
    virtual bool drawControls() = 0;
};

typedef std::shared_ptr<RenamerModule> RenamerModulePtr;
typedef std::weak_ptr<RenamerModule> RenamerModuleWeak;

enum class PluginModuleType { NONE = 0, RENAMER };

struct PluginModuleInfos {
    std::string path;
    std::string label;
    PluginModuleType type;
    std::array<float, 4> color{};
    PluginModuleInfos(const std::string& vPath, const std::string& vLabel, const PluginModuleType& vType, const std::array<float, 4>& vColor = {})
        : path(vPath), label(vLabel), type(vType), color(vColor) {
    }
};

struct ISettings : public IXmlSettings {
public:
    // get the categroy path of the settings for the mebnu display. ex: "plugins/apis"
    virtual SettingsCategoryPath GetCategory() const = 0;
    // will be called by the loader for inform the pluign than he must load somethings if any
    virtual bool LoadSettings() = 0;
    // will be called by the saver for inform the pluign than he must save somethings if any, by ex: temporary vars
    virtual bool SaveSettings() = 0;
    // will draw custom settings via imgui
    virtual bool DrawSettings() = 0;
};

typedef std::shared_ptr<ISettings> ISettingsPtr;
typedef std::weak_ptr<ISettings> ISettingsWeak;

struct PluginSettingsConfig {
    ISettingsWeak settings;
    PluginSettingsConfig(ISettingsWeak vSertings) : settings(vSertings) {
    }
};

struct PluginInterface {
    PluginInterface() = default;
    virtual ~PluginInterface() = default;
    virtual bool Init() = 0;
    virtual void Unit() = 0;
    virtual uint32_t GetMinimalCashMeVersionSupported() const = 0;
    virtual uint32_t GetVersionMajor() const = 0;
    virtual uint32_t GetVersionMinor() const = 0;
    virtual uint32_t GetVersionBuild() const = 0;
    virtual std::string GetName() const = 0;
    virtual std::string GetAuthor() const = 0;
    virtual std::string GetVersion() const = 0;
    virtual std::string GetContact() const = 0;
    virtual std::string GetDescription() const = 0;
    virtual std::vector<PluginModuleInfos> GetModulesInfos() const = 0;
    virtual PluginModulePtr CreateModule(const std::string& vPluginModuleName) = 0;
    virtual std::vector<PluginSettingsConfig> GetSettings() const = 0;
};

}