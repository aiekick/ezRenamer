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

class BaseSlot;
typedef std::weak_ptr<BaseSlot> BaseSlotWeak;
typedef std::shared_ptr<BaseSlot> BaseSlotPtr;

class NodeLink;
typedef std::weak_ptr<NodeLink> NodeLinkWeak;
typedef std::shared_ptr<NodeLink> NodeLinkPtr;

class BaseSlotInput;
typedef std::weak_ptr<BaseSlotInput> BaseSlotInputWeak;
typedef std::shared_ptr<BaseSlotInput> BaseSlotInputPtr;

class BaseSlotOutput;
typedef std::weak_ptr<BaseSlotOutput> BaseSlotOutputWeak;
typedef std::shared_ptr<BaseSlotOutput> BaseSlotOutputPtr;

typedef std::set<std::string> FilesSet;
typedef void* SlotUserDatas;
