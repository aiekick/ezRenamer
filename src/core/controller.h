#pragma once

#include <headers/datasDef.h>
#include <imguipack/ImGuiPack.h>

class Controller {
public:
    static Controller* instance() {
        static auto _instancePtr = std::make_unique<Controller>();
        return _instancePtr.get();
    }

private:
    RenamerContainer m_renamers;
    rnm::RenamerModuleWeak m_selectedRenamer;

public:
    bool init();
    void unit();
    bool drawControl();
    bool drawFilesList();
    bool drawPreview();

private:
    void m_getAvailableRenamers();
    void m_clearRenamers();
};
