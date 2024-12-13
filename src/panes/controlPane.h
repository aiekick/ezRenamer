#pragma once

#include <ImGuiPack.h>
#include <cstdint>
#include <memory>
#include <string>

class ProjectFile;
class ControlPane : public AbstractPane {
public:
    bool Init() override;
    void Unit() override;
    bool DrawPanes(const uint32_t& vCurrentFrame, bool* vOpened, ImGuiContext* vContextPtr, void* vUserDatas) override;

public:  // singleton
    static std::shared_ptr<ControlPane> Instance() {
        static std::shared_ptr<ControlPane> _instance = std::make_shared<ControlPane>();
        return _instance;
    }

public:
    ControlPane();                                               // Prevent construction
    ControlPane(const ControlPane&) = default;                     // Prevent construction by copying
    ControlPane& operator=(const ControlPane&) { return *this; };  // Prevent assignment
    virtual ~ControlPane();                                      // Prevent unwanted destruction};
};
