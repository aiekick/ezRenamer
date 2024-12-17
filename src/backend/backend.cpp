#include "backend.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <headers/ezRenamerBuild.h>

#define IMGUI_IMPL_API
#include <3rdparty/imgui_docking/backends/imgui_impl_opengl3.h>
#include <3rdparty/imgui_docking/backends/imgui_impl_glfw.h>

#include <cstdio>     // printf, fprintf
#include <chrono>     // timer
#include <cstdlib>    // abort
#include <fstream>    // std::ifstream
#include <iostream>   // std::cout
#include <algorithm>  // std::min, std::max
#include <stdexcept>  // std::exception

#include <project/projectFile.h>
#include <plugins/pluginManager.h>
#include <graph/nodeManager.h>
#include <core/controller.h>

#include <LayoutManager.h>

#include <ImGuiPack.h>

#include <ezlibs/ezTools.hpp>
#include <ezlibs/ezLog.hpp>
#include <ezlibs/ezFile.hpp>
#include <frontend/frontend.h>

#include <panes/consolePane.h>

#include <systems/settingsDialog.h>

// we include the cpp just for embedded fonts
#include <res/fontIcons.cpp>
#include <res/robotoMedium.cpp>

#define INITIAL_WIDTH 1700
#define INITIAL_HEIGHT 700

//////////////////////////////////////////////////////////////////////////////////
//// STATIC //////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

static void glfw_error_callback(int error, const char* description) {
    LogVarError("glfw error %i : %s", error, description);
}

static void glfw_window_close_callback(GLFWwindow* window) {
    glfwSetWindowShouldClose(window, GLFW_FALSE);  // block app closing
    Frontend::Instance()->Action_Window_CloseApp();
}

//////////////////////////////////////////////////////////////////////////////////
//// PUBLIC //////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

Backend::~Backend() = default;

void Backend::run(const ez::App& vApp) {
    if (init(vApp)) {
        m_MainLoop();
        unit();
    }
}

// todo : to refactor ! i dont like that
bool Backend::init(const ez::App& vApp) {
#ifdef _DEBUG
    SetConsoleVisibility(true);
#else
    SetConsoleVisibility(false);
#endif
    if (m_InitWindow()) {
        if (m_InitImGui()) {
            if (m_InitPlugins(vApp)) {
                if (m_InitCores()) {
                    if (m_InitSystems()) {
                        if (m_InitPanes()) {
                            if (m_InitSettings()) {
                                LoadConfigFile("config.xml", "app");
                                return true;
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}

// todo : to refactor ! i dont like that
void Backend::unit() {
    SaveConfigFile("config.xml", "app", "config");
    m_UnitSystems();
    m_UnitCores();
    m_UnitImGui();     // before plugins since containing weak ptr to plugins
    m_UnitSettings();  // after gui but before plugins since containing weak ptr to plugins
    m_UnitPlugins();
    m_UnitWindow();
}

bool Backend::isThereAnError() const {
    return false;
}

void Backend::NeedToNewProject(const std::string& vFilePathName) {
    m_NeedToNewProject = true;
    m_ProjectFileToLoad = vFilePathName;
}

void Backend::NeedToLoadProject(const std::string& vFilePathName) {
    m_NeedToLoadProject = true;
    m_ProjectFileToLoad = vFilePathName;
}

void Backend::NeedToCloseProject() {
    m_NeedToCloseProject = true;
}

bool Backend::SaveProject() {
    return ProjectFile::Instance()->Save();
}

void Backend::SaveAsProject(const std::string& vFilePathName) {
    ProjectFile::Instance()->SaveAs(vFilePathName);
}

// actions to do after rendering
void Backend::PostRenderingActions() {
    if (m_NeedToNewProject) {
        ProjectFile::Instance()->Clear();
        ProjectFile::Instance()->New(m_ProjectFileToLoad);
        m_ProjectFileToLoad.clear();
        m_NeedToNewProject = false;
    }

    if (m_NeedToLoadProject) {
        if (!m_ProjectFileToLoad.empty()) {
            if (ProjectFile::Instance()->LoadAs(m_ProjectFileToLoad)) {
                setAppTitle(m_ProjectFileToLoad);
                ProjectFile::Instance()->SetProjectChange(false);
            } else {
                LogVarError("Failed to load project %s", m_ProjectFileToLoad.c_str());
            }
        }

        m_ProjectFileToLoad.clear();
        m_NeedToLoadProject = false;
    }

    if (m_NeedToCloseProject) {
        ProjectFile::Instance()->Clear();
        m_NeedToCloseProject = false;
    }
}

bool Backend::IsNeedToCloseApp() {
    return m_NeedToCloseApp;
}

void Backend::NeedToCloseApp(const bool& vFlag) {
    m_NeedToCloseApp = vFlag;
}

void Backend::CloseApp() {
    // will escape the main loop
    glfwSetWindowShouldClose(m_MainWindowPtr, 1);
}

void Backend::setAppTitle(const std::string& vFilePathName) {
    auto ps = ez::file::parsePathFileName(vFilePathName);
    if (ps.isOk) {
        char bufTitle[1024];
        snprintf(bufTitle, 1023, "ezRenamer Beta %s - Project : %s.lum", ezRenamer_BuildId, ps.name.c_str());
        glfwSetWindowTitle(m_MainWindowPtr, bufTitle);
    } else {
        char bufTitle[1024];
        snprintf(bufTitle, 1023, "ezRenamer Beta %s", ezRenamer_BuildId);
        glfwSetWindowTitle(m_MainWindowPtr, bufTitle);
    }
}

ez::dvec2 Backend::GetMousePos() {
    ez::dvec2 mp;
    glfwGetCursorPos(m_MainWindowPtr, &mp.x, &mp.y);
    return mp;
}

int Backend::GetMouseButton(int vButton) {
    return glfwGetMouseButton(m_MainWindowPtr, vButton);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// CONSOLE ///////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Backend::SetConsoleVisibility(const bool& vFlag) {
    m_ConsoleVisiblity = vFlag;

    if (m_ConsoleVisiblity) {
        // on cache la console
        // on l'affichera au besoin comme blender fait
#ifdef WIN32
        ShowWindow(GetConsoleWindow(), SW_SHOW);
#endif
    } else {
        // on cache la console
        // on l'affichera au besoin comme blender fait
#ifdef WIN32
        ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif
    }
}

void Backend::SwitchConsoleVisibility() {
    m_ConsoleVisiblity = !m_ConsoleVisiblity;
    SetConsoleVisibility(m_ConsoleVisiblity);
}

bool Backend::GetConsoleVisibility() {
    return m_ConsoleVisiblity;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// RENDER ////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Backend::m_RenderOffScreen() {
    // m_DisplaySizeQuadRendererPtr->SetImageInfos(m_MergerRendererPtr->GetBackDescriptorImageInfo(0U));
}

void Backend::m_MainLoop() {
    int display_w, display_h;
    ImRect viewRect;
    while (!glfwWindowShouldClose(m_MainWindowPtr)) {
        ProjectFile::Instance()->NewFrame();

        // maintain active, prevent user change via imgui dialog
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;    // Enable Docking
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;  // Disable Viewport

        glfwPollEvents();

        glfwGetFramebufferSize(m_MainWindowPtr, &display_w, &display_h);

        m_Update();  // to do absolutly before imgui rendering

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGuiViewport* viewport = ImGui::GetMainViewport();
        if (viewport) {
            viewRect.Min = viewport->WorkPos;
            viewRect.Max = viewRect.Min + viewport->WorkSize;
        } else {
            viewRect.Max = ImVec2((float)display_w, (float)display_h);
        }

        Frontend::Instance()->Display(m_CurrentFrame, viewRect);

        ImGui::Render();

        glViewport(0, 0, display_w, display_h);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        auto* backup_current_context = glfwGetCurrentContext();

        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste
        // this code elsewhere.
        //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }
        glfwMakeContextCurrent(backup_current_context);

        glfwSwapBuffers(m_MainWindowPtr);

        // mainframe post actions
        PostRenderingActions();

        ++m_CurrentFrame;

        // will pause the view until we move the mouse or press keys
        // glfwWaitEvents();
    }
}

void Backend::m_Update() {

}

void Backend::m_IncFrame() {
    ++m_CurrentFrame;
}

///////////////////////////////////////////////////////
//// CONFIGURATION ////////////////////////////////////
///////////////////////////////////////////////////////

ez::xml::Nodes Backend::getXmlNodes(const std::string& vUserDatas) {
    ez::xml::Node node;
    node.addChilds(Frontend::Instance()->getXmlNodes(vUserDatas));
    node.addChilds(SettingsDialog::Instance()->getXmlNodes(vUserDatas));
    node.addChild("project").setContent(ProjectFile::Instance()->GetProjectFilepathName());
    return node.getChildren();
}

bool Backend::setFromXmlNodes(const ez::xml::Node& vNode, const ez::xml::Node& vParent, const std::string& vUserDatas) {
    const auto& strName = vNode.getName();
    const auto& strValue = vNode.getContent();
    const auto& strParentName = vParent.getName();
    if (strName == "project") {
        NeedToLoadProject(strValue);
    } 
    Frontend::Instance()->setFromXmlNodes(vNode, vParent, vUserDatas);
    SettingsDialog::Instance()->setFromXmlNodes(vNode, vParent, vUserDatas);
    return true;
}

//////////////////////////////////////////////////////////////////////////////////
//// PRIVATE /////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

bool Backend::m_InitWindow() {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return false;

    // GL 3.0 + GLSL 130
    m_GlslVersion = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_SAMPLES, 4);

    // Create window with graphics context
    m_MainWindowPtr = glfwCreateWindow(1280, 720, "ezRenamer", nullptr, nullptr);
    if (m_MainWindowPtr == nullptr) {
        std::cout << "Fail to create the window" << std::endl;
        return false;
    }
    glfwMakeContextCurrent(m_MainWindowPtr);
    glfwSwapInterval(1);  // Enable vsync

    if (gladLoadGL() == 0) {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return false;
    }

    glfwSetWindowCloseCallback(m_MainWindowPtr, glfw_window_close_callback);

    return true;
}

void Backend::m_UnitWindow() {
    glfwDestroyWindow(m_MainWindowPtr);
    glfwTerminate();
}

bool Backend::m_InitImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;    // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;  // Enable Viewport
    io.FontAllowUserScaling = true;                      // activate zoom feature with ctrl + mousewheel
#ifdef USE_DECORATIONS_FOR_RESIZE_CHILD_WINDOWS
    io.ConfigViewportsNoDecoration = false;  // toujours mettre une frame aux fenetres enfants
#endif

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // fonts
    {
        {  // main font
            if (ImGui::GetIO().Fonts->AddFontFromMemoryCompressedBase85TTF(FONT_ICON_BUFFER_NAME_RM, 15.0f) == nullptr) {
                assert(0);  // failed to load font
            }
        }
        {  // icon font
            static const ImWchar icons_ranges[] = {ICON_MIN_STRO, ICON_MAX_STRO, 0};
            ImFontConfig icons_config;
            icons_config.MergeMode = true;
            icons_config.PixelSnapH = true;
            if (ImGui::GetIO().Fonts->AddFontFromMemoryCompressedBase85TTF(FONT_ICON_BUFFER_NAME_STRO, 15.0f, &icons_config, icons_ranges) == nullptr) {
                assert(0);  // failed to load font
            }
        }
    }    

    // Setup Platform/Renderer bindings
    if (ImGui_ImplGlfw_InitForOpenGL(m_MainWindowPtr, true) &&  //
        ImGui_ImplOpenGL3_Init(m_GlslVersion)) {

        // ui init
        if (Frontend::Instance()->init()) {
            return true;
        }
    }
    return false;
}

bool Backend::m_InitPlugins(const ez::App& vApp) {
    PluginManager::Instance()->LoadPlugins(vApp);
    return true;
}

bool Backend::m_InitCores() {
    bool ret = true;
    ret &= NodeManager::initInstance();
    ret &= Controller::instance()->init();
    return ret;
}

void Backend::m_UnitCores() {
    NodeManager::unitInstance();
    Controller::instance()->unit();
}

void Backend::m_UnitPlugins() {
    PluginManager::Instance()->Clear();
}

bool Backend::m_InitSystems() {
    return true;    
}

void Backend::m_UnitSystems() {
}

bool Backend::m_InitPanes() {
    if (LayoutManager::Instance()->InitPanes()) {
        // a faire apres InitPanes() sinon ConsolePane::Instance()->paneFlag vaudra 0 et changeras apres InitPanes()
        Messaging::Instance()->sMessagePaneId = ConsolePane::Instance()->GetFlag();
        return true;
    }
    return false;
}

void Backend::m_UnitPanes() {
}

bool Backend::m_InitSettings() {
    SettingsDialog::Instance()->init();
    return true;
}

void Backend::m_UnitSettings() {
    SettingsDialog::Instance()->unit();
}

void Backend::m_UnitImGui() {
    Frontend::Instance()->unit();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();

    ImGui::DestroyContext();
}

