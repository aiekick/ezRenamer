#pragma once

#include <imGuiPack.h>
#include <cstdint>
#include <memory>
#include <string>

class ProjectFile;
class GraphPane : public AbstractPane {
private:

public:
    bool Init() override;
    void Unit() override;
    bool DrawWidgets(const uint32_t& vCurrentFrame, ImGuiContext* vContextPtr = nullptr, void* vUserDatas = nullptr) override;
    bool DrawOverlays(const uint32_t& vCurrentFrame, const ImRect& vRect, ImGuiContext* vContextPtr = nullptr, void* vUserDatas = nullptr) override;
    bool DrawPanes(const uint32_t& vCurrentFrame, bool* vOpened = nullptr, ImGuiContext* vContextPtr = nullptr, void* vUserDatas = nullptr) override;
    bool DrawDialogsAndPopups(const uint32_t& vCurrentFrame, const ImRect& vRect, ImGuiContext* vContextPtr = nullptr, void* vUserDatas = nullptr) override;

    void Load();

public:  // singleton
    static std::shared_ptr<GraphPane> Instance() {
        static auto _instance = std::make_shared<GraphPane>();
        return _instance;
    }

public:
    GraphPane();                             // Prevent construction
    GraphPane(const GraphPane&) = delete;  // Prevent construction by copying
    GraphPane& operator=(const GraphPane&) {
        return *this;
    };                      // Prevent assignment
    virtual ~GraphPane();  // Prevent unwanted destruction};
};
