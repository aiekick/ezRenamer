#pragma once

#include <string>
#include <memory>
#include <filesystem>

#include <ezlibs/ezApp.hpp>
#include <ezlibs/ezTools.hpp>
#include <ezlibs/ezPlugin.hpp>
#include <ezlibs/ezXmlConfig.hpp>

#include <imguipack.h>

#include <apis/ezRenamerPluginApi.h>

typedef ez::plugin::PluginInstance<rnm::PluginInterface> PluginInstance;
typedef std::shared_ptr<PluginInstance> PluginInstancePtr;
typedef std::weak_ptr<PluginInstance> PluginInstanceWeak;

class PluginManager {
private:
    std::map<std::string, PluginInstancePtr> m_Plugins;

public:
    void LoadPlugins(const ez::App& vApp);
    std::vector<rnm::PluginModuleInfos> GetPluginModulesInfos() const;
    rnm::PluginModulePtr CreatePluginModule(const std::string& vPluginNodeName);
    std::vector<rnm::PluginSettingsConfig> GetPluginSettings() const;
    void Clear();

private:
    void m_LoadPlugin(const std::filesystem::directory_entry& vEntry);
    void m_DisplayLoadedPlugins();

public:
    static PluginManager* Instance() {
        static PluginManager _instance;
        return &_instance;
    }

protected:
    PluginManager() = default;                      // Prevent construction
    PluginManager(const PluginManager&) = delete;  // Prevent construction by copying
    PluginManager& operator=(const PluginManager&) {
        return *this;
    };                           // Prevent assignment
    virtual ~PluginManager() = default;  // Prevent unwanted destruction
};