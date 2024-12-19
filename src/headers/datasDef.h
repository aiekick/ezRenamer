#pragma once

#include <set>
#include <map>
#include <array>
#include <vector>
#include <string>
#include <memory>
#include <cstdint>

#include <apis/ezRenamerPluginApi.h>

typedef std::string PluginName;
typedef std::string PluginPath;
typedef std::map<PluginName, rnm::RenamerModulePtr> RenamerContainer;

class BaseNode;
typedef std::weak_ptr<BaseNode> BaseNodeWeak;
typedef std::shared_ptr<BaseNode> BaseNodePtr;

class NodeSlot;
typedef std::weak_ptr<NodeSlot> NodeSlotWeak;
typedef std::shared_ptr<NodeSlot> NodeSlotPtr;

class NodeLink;
typedef std::weak_ptr<NodeLink> NodeLinkWeak;
typedef std::shared_ptr<NodeLink> NodeLinkPtr;

class NodeSlotInput;
typedef std::weak_ptr<NodeSlotInput> NodeSlotInputWeak;
typedef std::shared_ptr<NodeSlotInput> NodeSlotInputPtr;

class NodeSlotOutput;
typedef std::weak_ptr<NodeSlotOutput> NodeSlotOutputWeak;
typedef std::shared_ptr<NodeSlotOutput> NodeSlotOutputPtr;

typedef std::set<std::string> FilesSet;
typedef void* SlotUserDatas;
