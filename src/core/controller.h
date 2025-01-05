#pragma once

#include <headers/datasDef.h>
#include <imguipack/ImGuiPack.h>

class Controller {
private:  // Static
    static std::unique_ptr<Controller> mp_singleton;

public:  // Static
    static Controller* instance();
    static bool initInstance();
    static void unitInstance();

private:
    RenamerContainer m_renamers;
    rnm::RenamerModuleWeak m_selectedRenamer;

public:
    bool init();
    void unit();
    bool drawMenu();
    bool drawControl();
    bool drawFilesList();
    bool drawPreview();
    bool drawGraph();
    bool compileGraph();

private:
    void m_getAvailableRenamers();
    void m_clearRenamers();
};
