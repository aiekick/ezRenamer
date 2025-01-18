#pragma once

#include <imguipack.h>
#include <cstdint>
#include <memory>
#include <string>

class ProjectFile;
class PathsPane : public AbstractPane {
public:
    bool Init() override;
    void Unit() override;
    bool DrawPanes(const uint32_t& vCurrentFrame, bool* vOpened, ImGuiContext* vContextPtr, void* vUserDatas) override;

public:  // singleton
    static std::shared_ptr<PathsPane> Instance() {
        static auto _instance = std::make_shared<PathsPane>();
        return _instance;
    }

public:
    PathsPane();                              // Prevent construction
    PathsPane(const PathsPane&) = default;  // Prevent construction by copying
    PathsPane& operator=(const PathsPane&) {
        return *this;
    };                       // Prevent assignment
    virtual ~PathsPane();  // Prevent unwanted destruction};
};
