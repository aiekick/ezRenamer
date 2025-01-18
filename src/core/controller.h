#pragma once

#include <headers/datasDef.h>
#include <imguipack/imGuiPack.h>

class Controller {
private:  // Static
    static std::unique_ptr<Controller> mp_singleton;

public:  // Static
    static Controller* Instance();
    static bool initInstance();
    static void unitInstance();

private:
    RenamerContainer m_renamers;
    rnm::RenamerModuleWeak m_selectedRenamer;

    struct Files {
        ImGuiListClipper listClipper;
        std::vector<std::pair<std::string,std::string>> files;
        int32_t selectedIndex = -1;
    } m_files;

public:
    bool init();
    void unit();
    bool drawMenu(float& vOutWidth);
    bool drawControl();
    bool drawFilesList();
    bool drawPreview();
    bool drawGraph();
    bool compileGraph();
    bool playGraph();
    bool debugGraph();
    void setInputFiles(const std::vector<std::string>& vFiles);

private:
    void m_getAvailableRenamers();
    void m_clearRenamers();
};
