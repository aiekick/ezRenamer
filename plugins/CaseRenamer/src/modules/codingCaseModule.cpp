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
    return true;
}

void CodingCaseModule::unit() {

}

// will rename paths. return true if successfull
bool CodingCaseModule::rename(const rnm::PathDatas& vPathToRename, rnm::PathDatas& vOutRenamedpaths) {
    return false;
}

// draw ImGui plugin controls. return true if changed
bool CodingCaseModule::drawControls() {
    return false;
}