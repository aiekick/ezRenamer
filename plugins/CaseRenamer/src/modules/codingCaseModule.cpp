#include <modules/codingCaseModule.h>

rnm::PluginModulePtr CodingCaseModule::create() {
    auto ret = std::make_shared<CodingCaseModule>();
    if (!ret->init()) {
        ret->unit();
        ret.reset();
    }
    return ret;
}

bool CodingCaseModule::init() {
    m_SrcModesCombo.init(0, m_Modes);
    m_TgtModesCombo.init(0, m_Modes);
    return true;
}

void CodingCaseModule::unit() {
    m_SrcModesCombo.clear();
    m_TgtModesCombo.clear();
}

// will rename paths. return true if successfull
bool CodingCaseModule::rename(const rnm::PathDatas& vPathToRename, rnm::PathDatas& vOutRenamedpaths) {
    return false;
}

// draw ImGui plugin controls. return true if changed
bool CodingCaseModule::drawControls() {
    bool change = false;
    ImGui::Text("Source");
    change |= m_SrcModesCombo.display(150.0f, "##Source");
    ImGui::Text("Target");
    change |= m_TgtModesCombo.display(150.0f, "##Target");
    return change;
}