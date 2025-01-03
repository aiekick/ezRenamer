#pragma once

#include <map>
#include <unordered_map>
#include <apis/ezRenamerPluginApi.h>
#include <ezlibs/ezTools.hpp>

class LayoutNodeInterface {
public:
    class NodeLayout {
    private:
        ez::ivec2 m_cell = ez::ivec2(-1);  // layout x:column, y:row
        bool m_isLayouted = false;         // pour voir si il y a des doublon dasn des colonnes
        bool m_isRoot = false;             // ce node est le root
    };
    virtual bool isUsed() = 0;               // is used for hide unused nodes
    virtual bool isShown() = 0;              // is shown for not layout hidden nodes
    virtual NodeLayout& getLayoutRef() = 0;  // layout
    virtual ez::fvec2 getPos() = 0;          // get node pos
    virtual ez::fvec2 getSize() = 0;         // get node pos
};

class LayoutManagerInterface {
public:
    virtual bool applyLayout(BaseNodeWeak) = 0;
    virtual bool drawSettings() = 0;
};

class BaseLayoutManager {
public:
    void applyLayout(BaseNodeWeak vGraphNode);
    void clear();
    bool drawSettings();

    private:
    void setCurrentLayout();
};
