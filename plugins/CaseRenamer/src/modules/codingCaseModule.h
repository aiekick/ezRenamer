#pragma once

#include <apis/ezRenamerPluginApi.h>
#include <ImGuiPack.h>
#include <string>
#include <array>

class CodingCaseModule : public rnm::RenamerModule {
public:
    static rnm::PluginModulePtr create();

private:
    enum class Mode { Unknown = 0,CamelCase, PascalCase, SnakeCase, KebabCase, UpperCase, LowerCase, Count };
    std::vector<std::string> m_Modes{"Unknown", "camelCase", "PascalCase", "snake_case", "kebab-case", "UPPERCASE", "lowercase"};
    ImWidgets::QuickStringCombo m_SrcModesCombo;
    ImWidgets::QuickStringCombo m_TgtModesCombo;

public:
    ~CodingCaseModule() final = default;
    bool init() final;
    void unit() final;
    // will rename paths. return true if successfull
    bool rename(const rnm::PathDatas& vPathToRename, rnm::PathDatas& vOutRenamedpaths) final;
    // draw ImGui plugin controls. return true if changed
    bool drawControls() final;
};