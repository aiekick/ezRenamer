#pragma once

#include <imguipack/ImGuiPack.h>
#include <graph/baseNode.h>

#include <memory>
#include <functional>

class StringNode : public ImFlow::BaseNode {
public:
    StringNode() {
        setTitle("String");
        setStyle(ImFlow::NodeStyle::green());
        addOUT<std::string>("", ImFlow::ConnectionFilter_String)->behaviour([this]() { return m_buffer; });
    }

    void draw() override {
        ImGui::SetNextItemWidth(100.f);
        ImGui::InputText("##buffer", m_buffer, 256);
    }

private:
    char m_buffer[256 + 1] = "\0";
};

class PrintNode : public ImFlow::BaseNode {
public:
    PrintNode() {
        setTitle("Print");
        setStyle(ImFlow::NodeStyle::red());
        addIN<std::string>("str", "", ImFlow::ConnectionFilter_String);
    }

    void draw() override {
        ImGui::SetNextItemWidth(100.f);
        m_input = getInVal<std::string>("str");
        if (m_input != m_lastValue) {
            m_lastValue = m_input;
        }
        ImGui::Text("%s", m_lastValue.c_str());
    }

private:
    std::string m_input;
    std::string m_lastValue;
};

class NodeManager {
private: // Static
    static std::unique_ptr<NodeManager> m_singleton;

private: // Normal
    ImFlow::ImNodeFlow m_nodeEditor;
    //BaseNodePtr m_graphPtr = nullptr;

public:
    static NodeManager* instance();
    static bool initInstance();
    static void unitInstance();

public:
    bool init();
    void unit();
    bool drawGraph();

private:
    void m_displayBlueprintNodesMenu();
};
