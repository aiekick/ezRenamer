#pragma once

#include <Graph/Base/NodeSlot.h>
#include <unordered_map>
#include <map>
#include <set>
#include <vector>
#include <string>
#include <memory>

// shader type, shader filepathname

class LibraryItem
{
public:
	enum class LibraryItemTypeEnum : uint8_t
	{
		LIBRARY_ITEM_TYPE_SHADER = 0,
		LIBRARY_ITEM_TYPE_BLUEPRINT,
		LIBRARY_ITEM_TYPE_INTERNAL,
		LIBRARY_ITEM_TYPE_Count
	} type = LibraryItemTypeEnum::LIBRARY_ITEM_TYPE_SHADER;

public:
	// SHADER
	std::string shaderpath;
	
	// INTERNAL
	std::string nodeLabel;
	std::string nodeType;

	// func from slot
	NodeSlot nodeSlot;
};

typedef std::pair<std::string, LibraryItem> LibraryEntry;

class BaseNode;
struct BaseNodeStateStruct;
class LibraryCategory
{
public:
	std::string m_CategoryName;

private:
	// shader name, ShaderEntry
	std::map<std::string, LibraryEntry> m_LibraryItems;
	// category name, sub category
	std::map<std::string, LibraryCategory> m_SubCategories;

private:
	LibraryEntry ShowMenu(std::weak_ptr<BaseNode> vNodeGraph, BaseNodeStateStruct *vCanvasState, int vLevel);

public:
	LibraryCategory();
	~LibraryCategory();
	void Clear();
	LibraryCategory* AddCategory(const std::string& vCategoryName);
	void AddShader(const std::string& vShaderName, const std::string& vShaderPath);
	void AddCustom(const std::string& vCategoryPath, const std::string& vNodeLabel, const std::string& vNodeType);
	LibraryEntry ShowContent(std::weak_ptr<BaseNode> vNodeGraph, BaseNodeStateStruct *vCanvasState, int vLevel);
};

class UserNodeLibrary
{
private:
	LibraryCategory m_RootLibraryCategory;

public:
	std::weak_ptr<BaseNode> ShowNewNodeMenu(
		std::weak_ptr<BaseNode> vNodeGraph, BaseNodeStateStruct *vCanvasState);
	void AnalyseRootDirectory();

private:
	std::weak_ptr<BaseNode> CreateNode(std::weak_ptr<BaseNode> vNodeGraph, const LibraryEntry& vLibraryEntry);
	void AnalyseRootDirectoryRecurs(const char *name, int indent, LibraryCategory *vLibraryCategory);
	template<class T>
	std::weak_ptr<BaseNode> CreateCustomNode(std::weak_ptr<BaseNode> vNodeGraph)
	{
		std::weak_ptr<BaseNode> res;

		auto node = T::Create();
		if (node)
		{
			if (!vNodeGraph.expired())
			{
				auto graphPtr = vNodeGraph.lock();
				if (graphPtr)
				{
					graphPtr->AddChildNode(node);
					res = node;
				}
			}
		}

		return res;
	}

public: // singleton
	static UserNodeLibrary *Instance()
	{
		static UserNodeLibrary _instance;
		return &_instance;
	}

protected:
	UserNodeLibrary(); // Prevent construction
	UserNodeLibrary(const UserNodeLibrary&) {}; // Prevent construction by copying
	UserNodeLibrary& operator =(const UserNodeLibrary&) { return *this; }; // Prevent assignment
	~UserNodeLibrary() = default; // Prevent unwanted destruction
};