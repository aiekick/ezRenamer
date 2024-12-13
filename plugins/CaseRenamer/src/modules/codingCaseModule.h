#pragma once

#include <apis/ezRenamerPluginApi.h>

class CodingCaseModule : public rnm::RenamerModule {
public:
    static rnm::PluginModulePtr create();

public:
    ~CodingCaseModule() final = default;
    bool init() final;
    void unit() final;
    // will rename paths. return true if successfull
    bool rename(const rnm::PathDatas& vPathToRename, rnm::PathDatas& vOutRenamedpaths) final;
    // draw ImGui plugin controls. return true if changed
    bool drawControls() final;
};