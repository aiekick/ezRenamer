#pragma once

#include <imGuiPack.h>
#include <cstdint>
#include <memory>
#include <string>

class ProjectFile;
class PreviewPane : public AbstractPane {
public:
    bool Init() override;
    void Unit() override;
    bool DrawPanes(const uint32_t& vCurrentFrame, bool* vOpened, ImGuiContext* vContextPtr, void* vUserDatas) override;

public:  // singleton
    static std::shared_ptr<PreviewPane> Instance() {
        static auto _instance = std::make_shared<PreviewPane>();
        return _instance;
    }

public:
    PreviewPane();                              // Prevent construction
    PreviewPane(const PreviewPane&) = default;  // Prevent construction by copying
    PreviewPane& operator=(const PreviewPane&) {
        return *this;
    };                       // Prevent assignment
    virtual ~PreviewPane();  // Prevent unwanted destruction};
};
