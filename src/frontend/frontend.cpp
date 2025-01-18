/*
Copyright 2022-2023 Stephane Cuillerdier (aka aiekick)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "frontend.h"

#include <imguipack.h>

#include <backend/backend.h>
#include <project/projectFile.h>
#include <systems/settingsDialog.h>
#include <helpers/translationHelper.h>

#include <panes/consolePane.h>
#include <panes/controlPane.h>
#include <panes/previewPane.h>
#include <panes/pathsPane.h>
#include <panes/graphPane.h>
#include <panes/debugPane.h>

#include <res/fontIcons.h>

#include <core/controller.h>

using namespace std::placeholders;

//////////////////////////////////////////////////////////////////////////////////
//// STATIC //////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

bool Frontend::sCentralWindowHovered = false;

//////////////////////////////////////////////////////////////////////////////////
//// PUBLIC //////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

Frontend::~Frontend() = default;

bool Frontend::init() {
    m_build_themes();

    LayoutManager::Instance()->Init(ICON_FONT_MONITOR_DASHBOARD " Layouts", "Default Layout");

    LayoutManager::Instance()->SetPaneDisposalRatio("LEFT", 0.25f);
    LayoutManager::Instance()->SetPaneDisposalRatio("RIGHT", 0.25f);
    LayoutManager::Instance()->SetPaneDisposalRatio("BOTTOM", 0.25f);

#ifdef _DEBUG
    LayoutManager::Instance()->AddPane(DebugPane::Instance(), "Debug", "", "LEFT", 0.25f, false, false);
#endif

    LayoutManager::Instance()->AddPane(ConsolePane::Instance(), "Console", "", "BOTTOM", 0.25f, false, false);

    LayoutManager::Instance()->AddPane(ControlPane::Instance(), "Tuning", "", "RIGHT", 0.25f, false, false);
    //LayoutManager::Instance()->AddPane(PreviewPane::Instance(), "Preview", "", "BOTTOM", 0.0f, true, false);
    LayoutManager::Instance()->AddPane(PathsPane::Instance(), "Paths", "", "BOTTOM", 0.3f, false, false);
    LayoutManager::Instance()->AddPane(GraphPane::Instance(), "Graph", "", "CENTRAL", 0.0f, true, false);
    
    // InitPanes is done in m_InitPanes, because a specific order is needed

    return m_build();
}

void Frontend::unit() {
    LayoutManager::Instance()->UnitPanes();
}

bool Frontend::isValid() const {
    return false;
}

bool Frontend::isThereAnError() const {
    return false;
}

void Frontend::Display(const uint32_t& vCurrentFrame, const ImRect& vRect) {
    const auto context_ptr = ImGui::GetCurrentContext();
    if (context_ptr != nullptr) {
        const auto& io = ImGui::GetIO();

        m_DisplayRect = vRect;

        Frontend::sCentralWindowHovered = (ImGui::GetCurrentContext()->HoveredWindow == nullptr);
        ImGui::CustomStyle::ResetCustomId();

        m_drawMainMenuBar();
        m_drawMainStatusBar();

        if (LayoutManager::Instance()->BeginDockSpace(ImGuiDockNodeFlags_PassthruCentralNode)) {
            /*if (Backend::Instance()->GetBackendDatasRef().canWeTuneGizmo) {
                const auto viewport = ImGui::GetMainViewport();
                ImGuizmo::SetDrawlist(ImGui::GetCurrentWindow()->DrawList);
                ImGuizmo::SetRect(viewport->Pos.x, viewport->Pos.y, viewport->Size.x, viewport->Size.y);
                ImRect rc(viewport->Pos.x, viewport->Pos.y, viewport->Size.x, viewport->Size.y);
                DrawOverlays(vCurrentFrame, rc, context_ptr, {});
            }*/
            LayoutManager::Instance()->EndDockSpace();
        }

        if (LayoutManager::Instance()->DrawPanes(vCurrentFrame, context_ptr, {})) {
            ProjectFile::Instance()->SetProjectChange();
        }

        DrawDialogsAndPopups(vCurrentFrame, m_DisplayRect, context_ptr, {});

        ImGuiThemeHelper::Instance()->Draw();
        LayoutManager::Instance()->InitAfterFirstDisplay(io.DisplaySize);
    }
}

bool Frontend::DrawWidgets(const uint32_t& vCurrentFrame, ImGuiContext* vContextPtr, void* vUserDatas) {
    bool res = false;
    return res;
}

bool Frontend::DrawOverlays(const uint32_t& vCurrentFrame, const ImRect& vRect, ImGuiContext* vContextPtr, void* vUserDatas) {
    bool res = false;
    return res;
}

bool Frontend::DrawDialogsAndPopups(
    const uint32_t& vCurrentFrame, const ImRect& vRect, ImGuiContext* vContextPtr, void* vUserDatas) {
    m_ActionSystem.RunActions();
    LayoutManager::Instance()->DrawDialogsAndPopups(vCurrentFrame, vRect, vContextPtr, vUserDatas);
    if (m_ShowImGui) {
        ImGui::ShowDemoWindow(&m_ShowImGui);
    }
    if (m_ShowMetric) {
        ImGui::ShowMetricsWindow(&m_ShowMetric);
    }
    SettingsDialog::Instance()->Draw();
    return false;
}

void Frontend::OpenAboutDialog() {
    m_ShowAboutDialog = true;
}

void Frontend::m_drawMainMenuBar() {
    static float s_controller_menu_size = 0.0f;
    static float s_translation_menu_size = 0.0f;
    if (ImGui::BeginMainMenuBar()) {
        float full_width = ImGui::GetContentRegionAvail().x;
        if (ImGui::BeginMenu(ICON_FONT_ARCHIVE " Project")) {
            if (ImGui::MenuItem(" New")) {
                Action_Menu_NewProject();
            }

            if (ImGui::MenuItem(" Open")) {
                Action_Menu_OpenProject();
            }

            if (ProjectFile::Instance()->IsProjectLoaded()) {
                ImGui::Separator();

                if (ImGui::MenuItem(" Re Open")) {
                    Action_Menu_ReOpenProject();
                }

                ImGui::Separator();

                if (ImGui::MenuItem(" Save")) {
                    Action_Menu_SaveProject();
                }

                if (ImGui::MenuItem(" Save As")) {
                    Action_Menu_SaveAsProject();
                }

                ImGui::Separator();

                if (ImGui::MenuItem(" Close")) {
                    Action_Menu_CloseProject();
                }
            }

            ImGui::Separator();

            if (ImGui::MenuItem(" About")) {
                OpenAboutDialog();
            }

            ImGui::EndMenu();
        }
        LayoutManager::Instance()->DisplayMenu(ImGui::GetIO().DisplaySize);
        if (ImGui::BeginMenu(ICON_FONT_SETTINGS " Tools")) {
            if (ImGui::MenuItem("Settings")) {
                SettingsDialog::Instance()->OpenDialog();
            }
            ImGui::Separator();
            if (ImGui::BeginMenu("Styles")) {
                ImGuiThemeHelper::Instance()->DrawMenu();

#ifdef _DEBUG
                ImGui::Separator();
                ImGui::MenuItem("Show ImGui", "", &m_ShowImGui);
                ImGui::MenuItem("Show ImGui Metric/Debug", "", &m_ShowMetric);
#endif

                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }
        if (ProjectFile::Instance()->IsThereAnyProjectChanges()) {
            ImGui::Spacing(20.0f);
            if (ImGui::MenuItem(" Save")) {
                Action_Menu_SaveProject();
            }
        }

        ImGui::SpacingFromStart((full_width - s_controller_menu_size) * 0.5f);
        Controller::Instance()->drawMenu(s_controller_menu_size);

#ifdef _DEBUG
        const auto label = ez::str::toStr("Dear ImGui %s (Docking)", ImGui::GetVersion());
        const auto size = ImGui::CalcTextSize(label.c_str());
        ImGui::Spacing(ImGui::GetContentRegionAvail().x - size.x - s_translation_menu_size - ImGui::GetStyle().FramePadding.x * 2.0f);
        ImGui::Text("%s", label.c_str());
#else
        ImGui::SpacingFromStart(full_width - s_translation_menu_size);
#endif
        s_translation_menu_size = TranslationHelper::Instance()->DrawMenu();

        ImGui::EndMainMenuBar();
    }
}

void Frontend::m_drawMainStatusBar() {
    if (ImGui::BeginMainStatusBar()) {
        Messaging::Instance()->DrawStatusBar();

#ifdef _DEBUG
        const auto& io = ImGui::GetIO();
        const auto fps = ez::str::toStr("%.1f ms/frame (%.1f fps)", 1000.0f / io.Framerate, io.Framerate);
        const auto size = ImGui::CalcTextSize(fps.c_str());
        ImGui::Spacing(ImGui::GetContentRegionAvail().x - size.x - ImGui::GetStyle().FramePadding.x * 2.0f);
        ImGui::Text("%s", fps.c_str());
#endif

        ImGui::EndMainStatusBar();
    }
}

///////////////////////////////////////////////////////
//// SAVE DIALOG WHEN UN SAVED CHANGES ////////////////
///////////////////////////////////////////////////////

void Frontend::OpenUnSavedDialog() {
    // force close dialog if any dialog is opened
    ImGuiFileDialog::Instance()->Close();

    m_SaveDialogIfRequired = true;
}
void Frontend::CloseUnSavedDialog() {
    m_SaveDialogIfRequired = false;
}

bool Frontend::ShowUnSavedDialog() {
    bool res = false;

    if (m_SaveDialogIfRequired) {
        if (ProjectFile::Instance()->IsProjectLoaded()) {
            if (ProjectFile::Instance()->IsThereAnyProjectChanges()) {
                /*
                Unsaved dialog behavior :
                -	save :
                    -	insert action : save project
                -	save as :
                    -	insert action : save as project
                -	continue without saving :
                    -	quit unsaved dialog
                -	cancel :
                    -	clear actions
                */

                ImGui::CloseCurrentPopup();
                const char* label = "Save before closing ?";
                ImGui::OpenPopup(label);
                ImGui::SetNextWindowPos(m_DisplayRect.GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
                if (ImGui::BeginPopupModal(label, (bool*)0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDocking)) {
                    const auto& width = ImGui::CalcTextSize("Continue without saving").x + ImGui::GetStyle().ItemInnerSpacing.x;
                    
                    if (ImGui::ContrastedButton("Save", nullptr, nullptr, width * 0.5f)) {
                        res = Action_UnSavedDialog_SaveProject();
                    }
                    ImGui::SameLine();
                    if (ImGui::ContrastedButton("Save As", nullptr, nullptr, width * 0.5f)) {
                        Action_UnSavedDialog_SaveAsProject();
                    }

                    if (ImGui::ContrastedButton("Continue without saving")) {
                        res = true;  // quit the action
                    }

                    if (ImGui::ContrastedButton("Cancel", nullptr, nullptr, width + ImGui::GetStyle().FramePadding.x)) {
                        Action_Cancel();
                    }

                    ImGui::EndPopup();
                }
            }
        }

        return res;  // quit if true, else continue on the next frame
    }

    return true;  // quit the action
}

///////////////////////////////////////////////////////
//// ACTIONS //////////////////////////////////////////
///////////////////////////////////////////////////////

void Frontend::Action_Menu_NewProject() {
    /*
    new project :
    -	unsaved :
        -	add action : show unsaved dialog
        -	add action : open dialog for new project file name
    -	saved :
        -	add action : open dialog for new project file name
    */
    m_ActionSystem.Clear();
    Action_OpenUnSavedDialog_IfNeeded();
    m_ActionSystem.Add([this]() {
        CloseUnSavedDialog();
        IGFD::FileDialogConfig config;
        config.countSelectionMax = 1;
        config.flags = ImGuiFileDialogFlags_Modal;
        ImGuiFileDialog::Instance()->OpenDialog("NewProjectDlg", "New Project File", PROJECT_EXT, config);
        return true;
    });
    m_ActionSystem.Add([this]() { return Display_NewProjectDialog(); });
}

void Frontend::Action_Menu_OpenProject() {
    /*
    open project :
    -	unsaved :
        -	add action : show unsaved dialog
        -	add action : open project
    -	saved :
        -	add action : open project
    */
    m_ActionSystem.Clear();
    Action_OpenUnSavedDialog_IfNeeded();
    m_ActionSystem.Add([this]() {
        CloseUnSavedDialog();
        IGFD::FileDialogConfig config;
        config.countSelectionMax = 1;
        config.flags = ImGuiFileDialogFlags_Modal;
        ImGuiFileDialog::Instance()->OpenDialog("OpenProjectDlg", "Open Project File", PROJECT_EXT, config);
        return true;
    });
    m_ActionSystem.Add([this]() { return Display_OpenProjectDialog(); });
}

void Frontend::Action_Menu_ImportDatas() {
    m_ActionSystem.Clear();
    m_ActionSystem.Add([this]() {
        CloseUnSavedDialog();
        IGFD::FileDialogConfig config;
        config.countSelectionMax = 0;
        config.flags = ImGuiFileDialogFlags_Modal;
        ImGuiFileDialog::Instance()->OpenDialog("Import Datas", "Import Datas from File", ".csv", config);
        return true;
    });
    m_ActionSystem.Add([this]() { return Display_OpenProjectDialog(); });
}

void Frontend::Action_Menu_ReOpenProject() {
    /*
    re open project :
    -	unsaved :
        -	add action : show unsaved dialog
        -	add action : re open project
    -	saved :
        -	add action : re open project
    */
    m_ActionSystem.Clear();
    Action_OpenUnSavedDialog_IfNeeded();
    m_ActionSystem.Add([]() {
        Backend::Instance()->NeedToLoadProject(ProjectFile::Instance()->GetProjectFilepathName());
        return true;
    });
}

void Frontend::Action_Menu_SaveProject() {
    /*
    save project :
    -	never saved :
        -	add action : save as project
    -	saved in a file beofre :
        -	add action : save project
    */
    m_ActionSystem.Clear();
    m_ActionSystem.Add([this]() {
        if (!Backend::Instance()->SaveProject()) {
            CloseUnSavedDialog();
        }
        return true;
    });
    m_ActionSystem.Add([this]() { return Display_SaveProjectDialog(); });
}

void Frontend::Action_Menu_SaveAsProject() {
    /*
    save as project :
    -	add action : save as project
    */
    m_ActionSystem.Clear();
    m_ActionSystem.Add([this]() {
        CloseUnSavedDialog();
        IGFD::FileDialogConfig config;
        config.countSelectionMax = 1;
        config.flags = ImGuiFileDialogFlags_ConfirmOverwrite | ImGuiFileDialogFlags_Modal;
        ImGuiFileDialog::Instance()->OpenDialog("SaveProjectDlg", "Save Project File", PROJECT_EXT, config);
        return true;
    });
    m_ActionSystem.Add([this]() { return Display_SaveProjectDialog(); });
}

void Frontend::Action_Menu_CloseProject() {
    /*
    Close project :
    -	unsaved :
        -	add action : show unsaved dialog
        -	add action : Close project
    -	saved :
        -	add action : Close project
    */
    m_ActionSystem.Clear();
    Action_OpenUnSavedDialog_IfNeeded();
    m_ActionSystem.Add([]() {
        Backend::Instance()->NeedToCloseProject();
        return true;
    });
}

void Frontend::Action_Window_CloseApp() {
    if (Backend::Instance()->IsNeedToCloseApp())
        return;  // block next call to close app when running
    /*
    Close app :
    -	unsaved :
        -	add action : show unsaved dialog
        -	add action : Close app
    -	saved :
        -	add action : Close app
    */

    m_ActionSystem.Clear();
    Action_OpenUnSavedDialog_IfNeeded();
    m_ActionSystem.Add([]() {
        Backend::Instance()->CloseApp();
        return true;
    });
}

void Frontend::Action_OpenUnSavedDialog_IfNeeded() {
    if (ProjectFile::Instance()->IsProjectLoaded() && ProjectFile::Instance()->IsThereAnyProjectChanges()) {
        OpenUnSavedDialog();
        m_ActionSystem.Add([this]() { return ShowUnSavedDialog(); });
    }
}

void Frontend::Action_Cancel() {
    /*
    -	cancel :
        -	clear actions
    */
    CloseUnSavedDialog();
    m_ActionSystem.Clear();
    Backend::Instance()->NeedToCloseApp(false);
}

bool Frontend::Action_UnSavedDialog_SaveProject() {
    bool res = Backend::Instance()->SaveProject();
    if (!res) {
        m_ActionSystem.Insert([this]() { return Display_SaveProjectDialog(); });
        m_ActionSystem.Insert([this]() {
            CloseUnSavedDialog();
            IGFD::FileDialogConfig config;
            config.countSelectionMax = 1;
            config.flags = ImGuiFileDialogFlags_ConfirmOverwrite | ImGuiFileDialogFlags_Modal;
            config.path = ".";
            ImGuiFileDialog::Instance()->OpenDialog("SaveProjectDlg", "Save Project File", PROJECT_EXT, config);
            return true;
        });
    }
    return res;
}

void Frontend::Action_UnSavedDialog_SaveAsProject() {
    m_ActionSystem.Insert([this]() { return Display_SaveProjectDialog(); });
    m_ActionSystem.Insert([this]() {
        CloseUnSavedDialog();
        IGFD::FileDialogConfig config;
        config.countSelectionMax = 1;
        config.flags = ImGuiFileDialogFlags_ConfirmOverwrite | ImGuiFileDialogFlags_Modal;
        config.path = ".";
        ImGuiFileDialog::Instance()->OpenDialog("SaveProjectDlg", "Save Project File", PROJECT_EXT, config);
        return true;
    });
}

void Frontend::Action_UnSavedDialog_Cancel() {
    Action_Cancel();
}

///////////////////////////////////////////////////////
//// DIALOG FUNCS /////////////////////////////////////
///////////////////////////////////////////////////////

bool Frontend::Display_NewProjectDialog() {
    // need to return false to continue to be displayed next frame

    ImVec2 max = m_DisplayRect.GetSize();
    ImVec2 min = max * 0.5f;

    if (ImGuiFileDialog::Instance()->Display("NewProjectDlg", ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking, min, max)) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            CloseUnSavedDialog();
            auto file = ImGuiFileDialog::Instance()->GetFilePathName();
            Backend::Instance()->NeedToNewProject(file);
        } else  // cancel
        {
            Action_Cancel();  // we interrupts all actions
        }

        ImGuiFileDialog::Instance()->Close();

        return true;
    }

    return false;
}

bool Frontend::Display_OpenProjectDialog() {
    // need to return false to continue to be displayed next frame

    ImVec2 max = m_DisplayRect.GetSize();
    ImVec2 min = max * 0.5f;

    if (ImGuiFileDialog::Instance()->Display("OpenProjectDlg", ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking, min, max)) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            CloseUnSavedDialog();
            Backend::Instance()->NeedToLoadProject(ImGuiFileDialog::Instance()->GetFilePathName());
        } else  // cancel
        {
            Action_Cancel();  // we interrupts all actions
        }

        ImGuiFileDialog::Instance()->Close();

        return true;
    }

    return false;
}

bool Frontend::Display_SaveProjectDialog() {
    // need to return false to continue to be displayed next frame

    ImVec2 max = m_DisplayRect.GetSize();
    ImVec2 min = max * 0.5f;

    if (ImGuiFileDialog::Instance()->Display("SaveProjectDlg", ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking, min, max)) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            CloseUnSavedDialog();
            Backend::Instance()->SaveAsProject(ImGuiFileDialog::Instance()->GetFilePathName());
        } else  // cancel
        {
            Action_Cancel();  // we interrupts all actions
        }

        ImGuiFileDialog::Instance()->Close();

        return true;
    }

    return false;
}

///////////////////////////////////////////////////////
//// APP CLOSING //////////////////////////////////////
///////////////////////////////////////////////////////

void Frontend::IWantToCloseTheApp() {
    Action_Window_CloseApp();
}

///////////////////////////////////////////////////////
//// DROP /////////////////////////////////////////////
///////////////////////////////////////////////////////

void Frontend::JustDropFiles(int count, const char** paths) {
    assert(0);

    std::map<std::string, std::string> dicoFont;
    std::string prj;

    for (int i = 0; i < count; ++i) {
        // file
        auto f = std::string(paths[i]);

        // lower case
        auto f_opt = f;
        for (auto& c : f_opt)
            c = (char)std::tolower((int)c);

        // well known extention
        if (f_opt.find(".ttf") != std::string::npos     // truetype (.ttf)
            || f_opt.find(".otf") != std::string::npos  // opentype (.otf)
            //||	f_opt.find(".ttc") != std::string::npos		// ttf/otf collection for futur (.ttc)
        ) {
            dicoFont[f] = f;
        }
        if (f_opt.find(PROJECT_EXT) != std::string::npos) {
            prj = f;
        }
    }

    // priority to project file
    if (!prj.empty()) {
        Backend::Instance()->NeedToLoadProject(prj);
    }
}

//////////////////////////////////////////////////////////////////////////////////
//// PRIVATE /////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

bool Frontend::m_build() {
    // toolbar
    /*static ImFontConfig icons_config3;
    icons_config3.MergeMode = false;
    icons_config3.PixelSnapH = true;
    static const ImWchar icons_ranges3[] = {ICON_MIN_SDFMT, ICON_MAX_SDFMT, 0};
    const float& font_size = 20.0f / font_scale_ratio;
    m_ToolbarFontPtr =
        ImGui::GetIO().Fonts->AddFontFromMemoryCompressedBase85TTF(FONT_ICON_BUFFER_NAME_SDFMT, font_size, &icons_config3, icons_ranges3);
    if (m_ToolbarFontPtr != nullptr) {
        m_ToolbarFontPtr->Scale = font_scale_ratio;
        return true;
    }*/

    return LayoutManager::Instance()->InitPanes();
}

///////////////////////////////////////////////////////
//// CONFIGURATION ////////////////////////////////////
///////////////////////////////////////////////////////

ez::xml::Nodes Frontend::getXmlNodes(const std::string& vUserDatas) {
    ez::xml::Node node;
    node.addChilds(ImGuiThemeHelper::Instance()->getXmlNodes("app"));
    node.addChilds(LayoutManager::Instance()->getXmlNodes("app"));
#ifdef USE_PLACES_FEATURE
    node.addChild("places").setContent(ImGuiFileDialog::Instance()->SerializePlaces());
#endif
    node.addChild("showaboutdialog").setContent(m_ShowAboutDialog);
    node.addChild("showimgui").setContent(m_ShowImGui);
    node.addChild("showmetric").setContent(m_ShowMetric);
    return node.getChildren();
}

bool Frontend::setFromXmlNodes(const ez::xml::Node& vNode, const ez::xml::Node& vParent, const std::string& vUserDatas) {
    const auto& strName = vNode.getName();
    const auto& strValue = vNode.getContent();
    const auto& strParentName = vParent.getName();

    if (strName == "places") {
#ifdef USE_PLACES_FEATURE
        ImGuiFileDialog::Instance()->DeserializePlaces(strValue);
#endif
    } else if (strName == "showaboutdialog") {
        m_ShowAboutDialog = ez::ivariant(strValue).GetB();
    } else if (strName == "showimgui") {
        m_ShowImGui = ez::ivariant(strValue).GetB();
    } else if (strName == "showmetric") {
        m_ShowMetric = ez::ivariant(strValue).GetB();
    }

    ImGuiThemeHelper::Instance()->setFromXmlNodes(vNode, vParent, "app");
    LayoutManager::Instance()->setFromXmlNodes(vNode, vParent, "app");

    return true;
}
