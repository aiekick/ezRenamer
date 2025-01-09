#include <core/controller.h>
#include <plugins/pluginManager.h>
#include <graph/manager/nodeManager.h>
#include <graph/exec/manager/ExecManager.h>
#include <res/fontIcons.h>

std::unique_ptr<Controller> Controller::mp_singleton = nullptr;

Controller* Controller::Instance() {
    assert(mp_singleton != nullptr);
    return mp_singleton.get();
}

bool Controller::initInstance() {
    mp_singleton = std::make_unique<Controller>();
    return Instance()->init();
}

void Controller::unitInstance() {
    Instance()->unit();
    mp_singleton.reset();
}

bool Controller::init() {
    m_getAvailableRenamers();
    return true;
}

void Controller::unit() {
    m_clearRenamers();
}

bool Controller::drawMenu(float& vOutWidth) {
    bool ret = false;
    float last_cur_pos = ImGui::GetCursorPosX();
    if (ImGui::MenuItem(ICON_FONT_HAMMER "##Compile", "Compile graph")) {
        compileGraph();
        ret = true;
    }
    if (ImGui::MenuItem(ICON_FONT_PLAY "##Play", "Play graph")) {
        playGraph();
        ret = true;
    }
    if (ImGui::MenuItem(ICON_FONT_BUG "##Debug", "Debug graph")) {
        debugGraph();
        ret = true;
    }
    vOutWidth = ImGui::GetCursorPosX() - last_cur_pos + ImGui::GetStyle().FramePadding.x;
    return ret;
}

bool Controller::drawControl() {
    /*ImGui::Header("Renamers");
    const auto w = ImGui::GetContentRegionAvail().x;
    auto selectedRenamerPtr = m_selectedRenamer.lock();
    if (ImGui::BeginChild("##Renamers", ImVec2(w, -1), ImGuiChildFlags_FrameStyle | ImGuiChildFlags_Border)) {
        for (auto& plugin : m_renamers) {
            if (ImGui::Selectable(plugin.first.c_str(), selectedRenamerPtr == plugin.second)) {
                m_selectedRenamer = plugin.second;
            }
        }
        ImGui::EndChild(); 
    }*/

    return false;
}

bool Controller::drawFilesList() {
    bool change = false;
    static ImGuiTableFlags flags =        //
        ImGuiTableFlags_SizingFixedFit |  //
        ImGuiTableFlags_RowBg |           //
        ImGuiTableFlags_Hideable |        //
        ImGuiTableFlags_ScrollY |         //
        ImGuiTableFlags_NoHostExtendY;
    ImGui::PushID(this);
    if (ImGui::BeginTable("##FilesList", 2, flags)) {
        ImGui::TableSetupScrollFreeze(0, 1);  // Make header always visible
        ImGui::TableSetupColumn("Inputs files", ImGuiTableColumnFlags_WidthFixed, -1, 0);
        ImGui::TableSetupColumn("Outputs files", ImGuiTableColumnFlags_WidthFixed, -1, 0);
        ImGui::TableHeadersRow();
        m_files.listClipper.Begin(static_cast<int32_t>(m_files.files.size()), ImGui::GetTextLineHeightWithSpacing());
        while (m_files.listClipper.Step()) {
            for (int32_t idx = m_files.listClipper.DisplayStart; idx < m_files.listClipper.DisplayEnd; ++idx) {
                if (idx < 0) {
                    continue;
                }
                const auto& file = m_files.files.at(idx);
                ImGui::TableNextRow();
                if (ImGui::TableNextColumn()) {  // inputs
                    if (ImGui::Selectable(
                            file.first.c_str(),
                            m_files.selectedIndex > -1 && m_files.selectedIndex == idx,  //
                            ImGuiSelectableFlags_SpanAllColumns)) {
                        change = true;
                        m_files.selectedIndex = idx;
                    }
                }
                if (ImGui::TableNextColumn()) {  // outputs
                    ImGui::Text("%s", file.second.c_str());
                }
            }
        }
        m_files.listClipper.End();
        ImGui::EndTable();
    }
    ImGui::PopID();
    return false;
}

bool Controller::drawPreview() {
    bool change = false;
    auto ptr = m_selectedRenamer.lock();
    if (ptr != nullptr) {
        change |= ptr->drawControls();
    }
    return change;
}

bool Controller::drawGraph() {
    return NodeManager::Instance()->drawGraph();
}

bool Controller::compileGraph() {
    return ExecManager::Instance()->compileGraph(NodeManager::Instance()->getGraph());
}

bool Controller::playGraph() {
    return false;
}

bool Controller::debugGraph() {
    return false;
}

void Controller::setInputFiles(const std::vector<std::string>& vFiles) {
    m_files.files.clear();
    m_files.selectedIndex = -1;
    for (const auto& file : vFiles) {
        m_files.files.push_back(std::make_pair(file, ""));
    }
}

void Controller::m_getAvailableRenamers() {
    m_clearRenamers();
    auto modules = PluginManager::Instance()->GetPluginModulesInfos();
    for (const auto& mod : modules) {
        if (mod.type == rnm::PluginModuleType::RENAMER) {
            auto ptr = std::dynamic_pointer_cast<rnm::RenamerModule>(PluginManager::Instance()->CreatePluginModule(mod.label));
            if (ptr != nullptr) {
                auto path = mod.label;
                if (!mod.path.empty()) {
                    path = mod.path + "/" + path;
                }
                m_renamers[path] = ptr;
            }
        }
    }
}

void Controller::m_clearRenamers() {
    m_selectedRenamer.reset();  // must be reset before quit since pointing on the memory of a plugin
    m_renamers.clear();
}
