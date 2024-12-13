#pragma once

#include <ezlibs/ezXmlConfig.hpp>

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

class SettingsDialog : public ez::xml::Config {
private:
    std::map<std::string, ISettingsWeak> m_SettingsPerCategoryPath;
    bool m_ShowDialog = false;
    std::string m_SelectedCategoryPath;
    bool m_IsHiddenMode = false;

public:
    bool init();
    void unit();

    void OpenDialog();
    void CloseDialog();

    bool Draw();

    ez::xml::Nodes getXmlNodes(const std::string& vUserDatas = "") override;
    bool setFromXmlNodes(const ez::xml::Node& vNode, const ez::xml::Node& vParent, const std::string& vUserDatas) override;

    const bool& isHiddenMode();

private:
    void m_DrawCategoryPanes();
    void m_DrawContentPane();
    void m_DrawButtonsPane();
    bool m_Load();
    bool m_Save();

public:  // singleton
    static SettingsDialog* Instance() {
        static SettingsDialog _instance;
        return &_instance;
    }
};
