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

#include <set>
#include <array>
#include <memory>
#include <string>
#include <vector>
#include <cstdint>

#include "ILayoutPane.h"
#include <ezlibs/ezXml.hpp>

class BaseNode;
typedef std::weak_ptr<BaseNode> BaseNodeWeak;
typedef std::shared_ptr<BaseNode> BaseNodePtr;

class NodeSlot;
typedef std::weak_ptr<NodeSlot> NodeSlotWeak;
typedef std::shared_ptr<NodeSlot> NodeSlotPtr;

class NodeLink;
typedef std::weak_ptr<NodeLink> NodeLinkWeak;
typedef std::shared_ptr<NodeLink> NodeLinkPtr;

class NodeSlotInput;
typedef std::weak_ptr<NodeSlotInput> NodeSlotInputWeak;
typedef std::shared_ptr<NodeSlotInput> NodeSlotInputPtr;

class NodeSlotOutput;
typedef std::weak_ptr<NodeSlotOutput> NodeSlotOutputWeak;
typedef std::shared_ptr<NodeSlotOutput> NodeSlotOutputPtr;

typedef std::set<std::string> FilesSet;
typedef void* UserDatas;

namespace rnm {

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

typedef std::string PluginName;
typedef std::string PluginPath;
typedef std::map<PluginName, RenamerModulePtr> RenamerContainer;

enum class PluginModuleType { NONE = 0, RENAMER };

struct PluginModuleInfos {
    std::string path;
    std::string label;
    PluginModuleType type;
    std::array<float, 4> color{};
    PluginModuleInfos(const std::string& vPath, const std::string& vLabel, const PluginModuleType& vType, const std::array<float, 4>& vColor = {})
        : path(vPath), label(vLabel), type(vType), color(vColor) {}
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
    PluginSettingsConfig(ISettingsWeak vSertings) : settings(vSertings) {}
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

class NodeInterface {
protected:
    BaseNodeWeak m_ParentNode;  // node parent dans le cas d'un ndoe enfant d'un graph

public:
    void SetParentNode(BaseNodeWeak vBaseNodeWeak = BaseNodeWeak()) { m_ParentNode = vBaseNodeWeak; }
    BaseNodeWeak GetParentNode() { return m_ParentNode; }
    virtual bool DrawNodeWidget(const uint32_t& vFrame) = 0;
    virtual void BeforeNodeXmlLoading() {}
    virtual void AfterNodeXmlLoading() {}
};

template <typename T>
class WeakThisInterface {
protected:
    std::weak_ptr<T> m_This;

public:
    std::weak_ptr<T> getWeak() { return m_This; }
};

class FilesOutputInterface {
public:
    virtual FilesSet getFiles() const = 0;
};

class FilesInputInterface {
public:
    virtual bool setFiles(const FilesSet& vFiles) = 0;
};

struct ImGuiContext;
class GuiInterface {
public:
    virtual bool DrawWidgets(const uint32_t& vFrame, ImGuiContext* vpCtx) = 0;
};

typedef std::string NotifyEvent;
class NotifyInterface {
public:
    virtual void notify(const NotifyEvent& vEvent, const NodeSlotWeak& vEmitterSlot, const NodeSlotWeak& vReceiverSlot) = 0;
};

class NotifierInterface {
public:
    /// Treat an event (to be herited)
    virtual void treatNotification(const NotifyEvent& vEvent, const NodeSlotWeak& vEmitterSlot, const NodeSlotWeak& vReceiverSlot) = 0;

    /// Send a event in front (to be herited)
    virtual void sendFrontNotification(const NotifyEvent& vEvent) = 0;

    /// Send a event in back (to be herited)
    virtual void sendBackNotification(const NotifyEvent& vEvent) = 0;
};

class TaskInterface {
protected:
    // to compare to current frame
    // to know is the execution was already done
    uint32_t m_lastExecutedFrame = 0U;
    bool m_askForOneExecution = false;
    bool m_executionOnDemand = false;

protected:
    // only for execution on demand
    void m_needOneExecution() { m_askForOneExecution = true; }
    void m_setExecutionOnDemand(const bool vFlag) { m_askForOneExecution = vFlag; }

    // one execution per frame
    virtual bool m_execPerFrame(const uint32_t vFrame, UserDatas vUserDatas = nullptr) {
        (void)vFrame;
        assert(nullptr); // must be overriden
        return false;
    }

    // one execution on demand
    virtual bool m_execOnDemand(const uint32_t vFrame, UserDatas vUserDatas = nullptr) {
        (void)vFrame;
        assert(nullptr);  // must be overriden
        return false;
    }

public:
    // each derived class of TaskInterface must choose its execution mode
    bool exec(const uint32_t vFrame, UserDatas vUserDatas = nullptr) {
        if (m_lastExecutedFrame != vFrame) {
            m_lastExecutedFrame = vFrame;
            if (m_executionOnDemand) {
                if (m_askForOneExecution) {
                    m_askForOneExecution = false;
                    return m_execOnDemand(vFrame, vUserDatas);
                }
            } else {
                return m_execPerFrame(vFrame, vUserDatas);
            }
        }
        return false;
    }
};

}  // namespace rnm