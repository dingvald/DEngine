#include "pch.h"
#include "BodyPart.h"
#include <Utility/StringManipulation.h>
#include <Utility/StandardLogger.h>

const std::unordered_map<std::string, BodyPart::Slot::Type> String2SlotType =
{
	{"torso", BodyPart::Slot::Type::Torso},
	{"head", BodyPart::Slot::Type::Head},
	{"upper_limb", BodyPart::Slot::Type::UpperLimb},
	{"hand", BodyPart::Slot::Type::Hand},
	{"held", BodyPart::Slot::Type::Held},
	{"lower_limb", BodyPart::Slot::Type::LowerLimb},
	{"feet", BodyPart::Slot::Type::Feet},
};

BodyPart::BodyPart(const std::string& partName)
	: _name(partName)
{}

void BodyPart::createFromJson(const rapidjson::Value& json)
{
	if (json.HasMember("slots"))
	{
		for (auto&& val : json["slots"].GetArray())
		{
			auto slotObj = val.GetObject();
			std::string name = slotObj["name"].GetString();

			Slot newSlot;
			std::string typeStr = slotObj["type"].GetString();
			newSlot.type = String2SlotType.at(typeStr);
			
			_slots.emplace(name, std::move(newSlot));
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

const std::unordered_map<std::string, BodyPart::Slot>& BodyPart::getSlots() const
{
	return _slots;
}

BodyPart::Slot* BodyPart::getSlot(const std::string& slotName)
{
	if (!_slots.contains(slotName)) return nullptr;

	return &_slots.at(slotName);
}

BodyPart::Slot* BodyPart::getSlotType(Slot::Type type)
{
	for (auto&& [name, slot] : _slots)
	{
		if (slot.type != type) continue;
		return &slot;
	}
	return nullptr;
}

std::optional<BodyPart::Slot::Type> BodyPart::stringToSlotType(const std::string& slot)
{
	if (!String2SlotType.contains(slot)) return std::nullopt;
	return String2SlotType.at(slot);
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

const BodyPart::Slot* BodyPartTree::getSlot(const std::string& slot) const
{
	return searchForSlot(_root, slot);
}

BodyPart::Slot* BodyPartTree::getSlot(const std::string& slot)
{
	return searchForSlot(_root, slot);
}

std::vector<BodyPart*> BodyPartTree::getAllPartsWithSlotType(BodyPart::Slot::Type type)
{
	return getAllPartsWithSlotType(_root, type);
}

std::vector<const BodyPart*> BodyPartTree::getAllPartsWithSlotType(BodyPart::Slot::Type type) const
{
	return getAllPartsWithSlotType(_root, type);
}

BodyPart* BodyPartTree::getPartWithSlotType(BodyPart::Slot::Type type, const std::string& uniqueSlotTypeModifier)
{
	auto parts = getAllPartsWithSlotType(type);
	for (auto&& part : parts)
	{
		if (!part) continue;

		for (auto&& [name, slot] : part->getSlots())
		{
			if (drft::util::stringContains(name, uniqueSlotTypeModifier))
			{
				return part;
			}
		}
	}
	return nullptr;
}

unsigned long BodyPartTree::swapItem(unsigned long item, const std::string& slotName)
{
	if (auto slot = searchForSlot(_root, slotName))
	{
		unsigned int existingItem = slot->item;
		slot->item = item;
		return existingItem;
	}
	return 0u;
}

bool BodyPartTree::equipItem(unsigned long item, const std::string& slotName)
{
	if (auto slot = searchForSlot(_root, slotName))
	{
		slot->item = item;
		return true;
	}
	return false;
}

unsigned long BodyPartTree::unequipItem(const std::string& slotName)
{
	if (auto slot = searchForSlot(_root, slotName))
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


unsigned long BodyPartTree::getEquipped(const std::string& slotName) const
{
	if (auto slot = searchForSlot(_root, slotName))
	{
		return slot->item;
	}
	return 0u;
}

unsigned long BodyPartTree::getEquipped(BodyPart::Slot::Type type, const std::string& uniqueSlotNameToken) const
{
	auto parts = getAllPartsWithSlotType(type);
	for (auto&& part : parts)
	{
		if (!part) continue;

		for (auto&& [name, slot] : part->getSlots())
		{
			if (slot.type != type) continue;

			if (drft::util::stringContains(name, uniqueSlotNameToken))
			{
				return slot.item;
			}
		}
	}
	return 0ul; // No Item
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
	if (root._slots.contains(slotName))
	{
		result.push_back(&root);
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
	if (root._slots.contains(slotName))
	{
		result.push_back(&root);
	}
	for (auto&& part : root._children)
	{
		auto parts = getAllPartsWithSlot(part, slotName);
		result.insert(result.end(), parts.begin(), parts.end());
	}
	return result;
}

std::vector<BodyPart*> BodyPartTree::getAllPartsWithSlotType(BodyPart& root, BodyPart::Slot::Type type)
{
	std::vector<BodyPart*> result;
	for (auto&& [name, slot] : root._slots)
	{
		if (slot.type == type)
		{
			result.push_back(&root);
		}
	}
	for (auto&& part : root._children)
	{
		auto parts = getAllPartsWithSlotType(part, type);
		result.insert(result.end(), parts.begin(), parts.end());
	}
	return result;
}

std::vector<const BodyPart*> BodyPartTree::getAllPartsWithSlotType(const BodyPart& root, BodyPart::Slot::Type type)
{
	std::vector<const BodyPart*> result;
	for (auto&& [name, slot] : root._slots)
	{
		if (slot.type == type)
		{
			result.push_back(&root);
		}
	}
	for (auto&& part : root._children)
	{
		auto parts = getAllPartsWithSlotType(part, type);
		result.insert(result.end(), parts.begin(), parts.end());
	}
	return result;
}

BodyPart::Slot* BodyPartTree::searchForSlot(BodyPart& root, const std::string& slotName)
{
	if (root._slots.contains(slotName))
	{
		return &root._slots.at(slotName);
	}
	for (auto&& part : root._children)
	{
		if (auto result = searchForSlot(part, slotName))
		{
			return result;
		}
	}
	return nullptr;
}

const BodyPart::Slot* BodyPartTree::searchForSlot(const BodyPart& root, const std::string& slotName)
{
	if (root._slots.contains(slotName))
	{
		return &root._slots.at(slotName);
	}
	for (auto&& part : root._children)
	{
		if (auto result = searchForSlot(part, slotName))
		{
			return result;
		}
	}
	return nullptr;
}

std::vector<BodyPart::Slot*> BodyPartTree::searchForSlotWithItem(BodyPart& root, unsigned long item)
{
	std::vector<BodyPart::Slot*> result;
	for (auto&& [name, slot] : root._slots)
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
	for (auto&& [name, slot] : root._slots)
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
	for (auto&& [name, slot] : root._slots)
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
