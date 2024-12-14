// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "CaseRenamer.h"
#include <Headers/caseRenamerBuild.h>

#include <modules/codingCaseModule.h>

// needed for plugin creating / destroying
extern "C"  // needed for avoid renaming of funcs by the compiler
{
#ifdef WIN32
    #define PLUGIN_PREFIX __declspec(dllexport)
#else
    #define PLUGIN_PREFIX
#endif

PLUGIN_PREFIX CaseRenamer* allocator() {
    return new CaseRenamer();
}

PLUGIN_PREFIX void deleter(CaseRenamer* ptr) {
    delete ptr;
}
}

CaseRenamer::CaseRenamer() {
#ifdef _MSC_VER
    // active memory leak detector
    //_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
}

bool CaseRenamer::Init() {
    return true;
}

void CaseRenamer::Unit() {
}

uint32_t CaseRenamer::GetMinimalCashMeVersionSupported() const {
    return 0U;
}

uint32_t CaseRenamer::GetVersionMajor() const {
    return CaseRenamer_MinorNumber;
}

uint32_t CaseRenamer::GetVersionMinor() const {
    return CaseRenamer_MajorNumber;
}

uint32_t CaseRenamer::GetVersionBuild() const {
    return CaseRenamer_BuildNumber;
}

std::string CaseRenamer::GetName() const {
    return "CaseRenamer";
}

std::string CaseRenamer::GetAuthor() const {
    return "Stephane Cuillerdier";
}

std::string CaseRenamer::GetVersion() const {
    return CaseRenamer_BuildId;
}

std::string CaseRenamer::GetContact() const {
    return "ezRenamer@funparadigm.com";
}

std::string CaseRenamer::GetDescription() const {
    return "Cases Renamer";
}

std::vector<rnm::PluginModuleInfos> CaseRenamer::GetModulesInfos() const {
    std::vector<rnm::PluginModuleInfos> res;
    res.push_back(rnm::PluginModuleInfos("", "Case", rnm::PluginModuleType::RENAMER));
    return res;
}

rnm::PluginModulePtr CaseRenamer::CreateModule(const std::string& vPluginModuleName) {
    if (vPluginModuleName == "Case") {
        return CodingCaseModule::create();
    } 
    return nullptr;
}

std::vector<rnm::PluginSettingsConfig> CaseRenamer::GetSettings() const {
    return {};
}
