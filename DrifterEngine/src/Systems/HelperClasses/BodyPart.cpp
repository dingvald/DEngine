#include "pch.h"
#include "BodyPart.h"
#include <Utility/StringManipulation.h>
#include <Utility/StandardLogger.h>

BodyPart::Slot::Slot(const char* str)
	: item(0)
{
	auto splitStrings = drft::util::split(str, ":");
	if (splitStrings.size() == 2)
	{
		id = splitStrings[0];
		uid = std::stoi(splitStrings[1]);
	}
	else if (splitStrings.size() == 1)
	{
		id = splitStrings[0];
	}
	else
	{
		error_logger << "Error: Could not parse slot " << str << " - improper number of tokens" << std::endl;
	}
}

BodyPart::BodyPart(const std::string& partName)
	: _name(partName)
{}

void BodyPart::createFromJson(const rapidjson::Value& json)
{
	if (json.HasMember("slots"))
	{
		for (auto&& val : json["slots"].GetArray())
		{
			_slots.emplace_back(Slot{ val.GetString() });
		}
	}
	if (json.HasMember("size"))
	{
		_size = json["size"].GetInt();
	}
	if (json.HasMember("attached_parts"))
	{
		_children.reserve(json["attached_parts"].MemberCount());
		for (auto&& member : json["attached_parts"].GetObject())
		{
			BodyPart child = { member.name.GetString() };
			child.createFromJson(member.value);
			_children.emplace_back(std::move(child));
		}
	}
}

const std::string& BodyPart::getName() const
{
	return _name;
}

const std::vector<BodyPart::Slot>& BodyPart::getSlots() const
{
	return _slots;
}

BodyPart::Slot* BodyPart::getSlot(const std::string& slotName)
{
	for (auto&& slot : _slots)
	{
		if (slot.id == slotName)
		{
			return &slot;
		}
	}
	return nullptr;
}

void BodyPartTree::createFromJson(const rapidjson::Value& json)
{
	auto root = json.MemberBegin();
	_root = BodyPart{ root->name.GetString() };
	_root.createFromJson(root->value);
}

std::vector<BodyPart*> BodyPartTree::getAllParts()
{
	return getAllParts(_root);
}

std::vector<const BodyPart*> BodyPartTree::getAllParts() const
{
	return getAllParts(_root);
}

std::vector<BodyPart*> BodyPartTree::getAllPartsWithSlot(const std::string& slot)
{
	return getAllPartsWithSlot(_root, slot);
}

std::vector<const BodyPart*> BodyPartTree::getAllPartsWithSlot(const std::string& slot) const
{
	return getAllPartsWithSlot(_root, slot);
}

unsigned long BodyPartTree::swapItem(unsigned long item, const std::string& slotName, int uid)
{
	if (auto slot = searchForSlot(_root, slotName, uid))
	{
		unsigned int existingItem = slot->item;
		slot->item = item;
		return existingItem;
	}
	return 0u;
}

bool BodyPartTree::equipItem(unsigned long item, const std::string& slotName, int uid)
{
	if (auto slot = searchForSlot(_root, slotName, uid))
	{
		slot->item = item;
		return true;
	}
	return false;
}

unsigned long BodyPartTree::unequipItem(const std::string& slotName, int uid)
{
	if (auto slot = searchForSlot(_root, slotName, uid))
	{
		unsigned int existingItem = slot->item;
		slot->item = 0u;
		return existingItem;
	}
	return 0u;
}

bool BodyPartTree::removeItem(unsigned long item)
{
	auto slots = searchForSlotWithItem(_root, item);
	for (auto&& slot : slots)
	{
		slot->item = 0u;
	}
	return slots.empty() ? false : true;
}

std::vector<unsigned long> BodyPartTree::getAllEquipped() const
{
	std::vector<unsigned long> result;
	auto slots = getAllSlots(_root);
	for (auto slot : slots)
	{
		if (slot->item == 0u) continue;
		result.push_back(slot->item);
	}
	return result;
}

std::vector<unsigned long> BodyPartTree::getEquipped(const std::string& slotName) const
{
	std::vector<unsigned long> result;
	auto slots = searchForSlot(_root, slotName);
	for (auto&& slot : slots)
	{
		result.push_back(slot->item);
	}
	return result;
}

unsigned long BodyPartTree::getEquipped(const std::string& slotName, int uid) const
{
	if (auto slot = searchForSlot(_root, slotName, uid))
	{
		return slot->item;
	}
	return 0u;
}

BodyPart* BodyPartTree::searchForPart(BodyPart& root, const std::string& partName)
{
	if (root._name == partName) return &root;
	for (auto&& part : root._children)
	{
		if (auto result = searchForPart(part, partName))
		{
			return result;
		}
	}
	return nullptr;
}

const BodyPart* BodyPartTree::searchForPart(const BodyPart& root, const std::string& partName)
{
	if (root._name == partName) return &root;
	for (auto&& part : root._children)
	{
		if (auto result = searchForPart(part, partName))
		{
			return result;
		}
	}
	return nullptr;
}

std::vector<BodyPart*> BodyPartTree::getAllParts(BodyPart& root)
{
	std::vector<BodyPart*> result;
	result.push_back(&root);
	for (auto&& part : root._children)
	{
		auto parts = getAllParts(part);
		result.insert(result.end(), parts.begin(), parts.end());
	}
	return result;
}

std::vector<const BodyPart*> BodyPartTree::getAllParts(const BodyPart& root)
{
	std::vector<const BodyPart*> result;
	result.push_back(&root);
	for (auto&& part : root._children)
	{
		auto parts = getAllParts(part);
		result.insert(result.end(), parts.begin(), parts.end());
	}
	return result;
}

std::vector<BodyPart*> BodyPartTree::getAllPartsWithSlot(BodyPart& root, const std::string& slotName)
{
	std::vector<BodyPart*> result;
	for (auto&& slot : root.getSlots())
	{
		if (slot.id == slotName)
		{
			result.push_back(&root);
		}
	}
	for (auto&& part : root._children)
	{
		auto parts = getAllPartsWithSlot(part, slotName);
		result.insert(result.end(), parts.begin(), parts.end());
	}
	return result;
}

std::vector<const BodyPart*> BodyPartTree::getAllPartsWithSlot(const BodyPart& root, const std::string& slotName)
{
	std::vector<const BodyPart*> result;
	for (auto&& slot : root.getSlots())
	{
		if (slot.id == slotName)
		{
			result.push_back(&root);
		}
	}
	for (auto&& part : root._children)
	{
		auto parts = getAllPartsWithSlot(part, slotName);
		result.insert(result.end(), parts.begin(), parts.end());
	}
	return result;
}

BodyPart::Slot* BodyPartTree::searchForSlot(BodyPart& root, const std::string& slotName, int uid)
{
	for (auto&& slot : root._slots)
	{
		if (slot.id == slotName && slot.uid == uid) return &slot;
	}
	for (auto&& part : root._children)
	{
		if (auto result = searchForSlot(part, slotName, uid))
		{
			return result;
		}
	}
	return nullptr;
}

const BodyPart::Slot* BodyPartTree::searchForSlot(const BodyPart& root, const std::string& slotName, int uid)
{
	for (auto&& slot : root._slots)
	{
		if (slot.id == slotName && slot.uid == uid) return &slot;
	}
	for (auto&& part : root._children)
	{
		if (auto result = searchForSlot(part, slotName, uid))
		{
			return result;
		}
	}
	return nullptr;
}

std::vector<BodyPart::Slot*> BodyPartTree::searchForSlot(BodyPart& root, const std::string& slotName)
{
	std::vector<BodyPart::Slot*> result;
	for (auto&& slot : root._slots)
	{
		if (slot.id == slotName)
		{
			result.push_back(&slot);
			break;
		}
	}
	for (auto&& part : root._children)
	{
		auto parts = searchForSlot(part, slotName);
		result.insert(result.end(), parts.begin(), parts.end());
	}
	return result;
}

std::vector<const BodyPart::Slot*> BodyPartTree::searchForSlot(const BodyPart& root, const std::string& slotName)
{
	std::vector<const BodyPart::Slot*> result;
	for (auto&& slot : root._slots)
	{
		if (slot.id == slotName)
		{
			result.push_back(&slot);
			break;
		}
	}
	for (auto&& part : root._children)
	{
		auto parts = searchForSlot(part, slotName);
		result.insert(result.end(), parts.begin(), parts.end());
	}
	return result;
}

std::vector<BodyPart::Slot*> BodyPartTree::searchForSlotWithItem(BodyPart& root, unsigned long item)
{
	std::vector<BodyPart::Slot*> result;
	for (auto&& slot : root._slots)
	{
		if (slot.item == item)
		{
			result.push_back(&slot);
		}
	}
	for (auto&& part : root._children)
	{
		auto parts = searchForSlotWithItem(part, item);
		result.insert(result.end(), parts.begin(), parts.end());
	}
	return result;
}

std::vector<BodyPart::Slot*> BodyPartTree::getAllSlots(BodyPart& root)
{
	std::vector<BodyPart::Slot*> result;
	for (auto&& slot : root._slots)
	{
		result.push_back(&slot);
	}
	for (auto&& part : root._children)
	{
		auto parts = getAllSlots(part);
		result.insert(result.end(), parts.begin(), parts.end());
	}

	return result;
}

std::vector<const BodyPart::Slot*> BodyPartTree::getAllSlots(const BodyPart& root)
{
	std::vector<const BodyPart::Slot*> result;
	for (auto&& slot : root._slots)
	{
		result.push_back(&slot);
	}
	for (auto&& part : root._children)
	{
		auto parts = getAllSlots(part);
		result.insert(result.end(), parts.begin(), parts.end());
	}

	return result;
}
