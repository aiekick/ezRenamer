#include "pluginManager.h"

#include <imguipack.h>

#include <ezlibs/ezFile.hpp>

namespace fs = std::filesystem;

//////////////////////////////////////////////////////////////////////////////
////// PluginLoader //////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////


void PluginManager::Clear() {
    m_Plugins.clear();
}

void PluginManager::LoadPlugins(const ez::App& vApp) {
    std::cout << "-----------" << std::endl;
    auto plugin_directory = std::filesystem::path(vApp.getAppPath()).append("plugins");
    if (std::filesystem::exists(plugin_directory)) {
        const auto dir_iter = std::filesystem::directory_iterator(plugin_directory);
        for (const auto& file : dir_iter) {
            m_LoadPlugin(file);
        }
        m_DisplayLoadedPlugins();
    } else {
        LogVarLightInfo("Plugin directory %s not found !", plugin_directory.string().c_str());
    }
    std::cout << "-----------" << std::endl;
}

std::vector<rnm::PluginModuleInfos> PluginManager::GetPluginModulesInfos() const {
    std::vector<rnm::PluginModuleInfos> res;

    for (auto plugin : m_Plugins) {
        if (plugin.second) {
            auto pluginInstancePtr = plugin.second->get().lock();
            if (pluginInstancePtr != nullptr) {
                auto lib_entrys = pluginInstancePtr->getModulesInfos();
                if (!lib_entrys.empty()) {
                    res.insert(res.end(), lib_entrys.begin(), lib_entrys.end());
                }
            }
        }
    }

    return res;
}

rnm::PluginModulePtr PluginManager::CreatePluginModule(const std::string& vPluginNodeName) {
    if (!vPluginNodeName.empty()) {
        for (auto plugin : m_Plugins) {
            if (plugin.second) {
                auto pluginInstancePtr = plugin.second->get().lock();
                if (pluginInstancePtr != nullptr) {
                    auto ptr = pluginInstancePtr->createModule(vPluginNodeName);
                    if (ptr != nullptr) {
                        return ptr;
                    }
                }
            }
        }
    }
    return nullptr;
}

std::vector<rnm::PluginSettingsConfig> PluginManager::GetPluginSettings() const {
    std::vector<rnm::PluginSettingsConfig> pluginSettings;
    for (auto plugin : m_Plugins) {
        if (plugin.second) {
            auto pluginInstancePtr = plugin.second->get().lock();
            if (pluginInstancePtr) {
                auto _pluginSettings = pluginInstancePtr->getSettings();
                if (!_pluginSettings.empty()) {
                    pluginSettings.insert(pluginSettings.end(), _pluginSettings.begin(), _pluginSettings.end());
                }
            }
        }
    }
    return pluginSettings;
}

//////////////////////////////////////////////////////////////
//// PRIVATE /////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

void PluginManager::m_LoadPlugin(const fs::directory_entry& vEntry) {
    if (vEntry.is_directory()) {
        const auto dir_iter = std::filesystem::directory_iterator(vEntry);
        for (const auto& file : dir_iter) {
            m_LoadPlugin(file);
        }
    } else if (vEntry.is_regular_file()) {
        auto file_name = vEntry.path().filename().string();
        if (file_name.find(PLUGIN_NAME_PREFIX) == 0U) {
            if (file_name.find(PLUGIN_RUNTIME_CONFIG) != std::string::npos) {
                auto file_path_name = vEntry.path().string();
                if (file_path_name.find(ez::plugin::getDLLExtention()) != std::string::npos) {
                    auto ps = ez::file::parsePathFileName(file_path_name);
                    if (ps.isOk) {
                        auto resPtr = std::make_shared<PluginInstance>();
                        auto ret = resPtr->init(ps.name, ps.GetFPNE());
                        if (ret != PluginInstance::PluginReturnMsg::LOADING_SUCCEED) {
                            resPtr.reset();
                            if (ret == PluginInstance::PluginReturnMsg::LOADING_FAILED) {
                                LogVarDebugError("Plugin %s fail to load", ps.name.c_str());
                            }
                        } else {
                            auto ptr = resPtr->get().lock();
                            if (ptr != nullptr) {
                                m_Plugins[ps.name] = resPtr;
                            }
                        }
                    }
                }
            }
        }
    }
}

void PluginManager::m_DisplayLoadedPlugins() {
    if (!m_Plugins.empty()) {
        size_t max_name_size = 0U;
        size_t max_vers_size = 0U;
        const size_t& minimal_space = 2U;
        for (auto plugin : m_Plugins) {
            if (plugin.second != nullptr) {
                auto plugin_instance_ptr = plugin.second->get().lock();
                if (plugin_instance_ptr != nullptr) {
                    max_name_size = ez::maxi(max_name_size, plugin_instance_ptr->getName().size() + minimal_space);
                    max_vers_size = ez::maxi(max_vers_size, plugin_instance_ptr->getVersion().size() + minimal_space);
                }
            }
        }
        for (auto plugin : m_Plugins) {
            if (plugin.second != nullptr) {
                auto plugin_instance_ptr = plugin.second->get().lock();
                if (plugin_instance_ptr != nullptr) {
                    const auto& name = plugin_instance_ptr->getName();
                    const auto& name_space = std::string(max_name_size - name.size(), ' ');  // 32 is a space in ASCII
                    const auto& vers = plugin_instance_ptr->getVersion();
                    const auto& vers_space = std::string(max_vers_size - vers.size(), ' ');  // 32 is a space in ASCII
                    const auto& desc = plugin_instance_ptr->getDescription();
                    LogVarLightInfo("Plugin loaded : %s%sv%s%s(%s)",  //
                        name.c_str(), name_space.c_str(),             //
                        vers.c_str(), vers_space.c_str(),             //
                        desc.c_str());
                }
            }
        }
    }
}