#include "pch.h"
#include "BodyParts.h"


// BODY PART //////////////////////////////////////////////////////////////////////////////////////////////////

BodyPart::BodyPart(std::string name, PartType type, unsigned int size)
	: name(name)
	, type(type)
	, size(size) {}

BodyPart::BodyPart(const BodyPart& other)
{
	this->name = other.name;
	this->type = other.type;
	this->size = other.size;
	this->_slotted = other._slotted;
	for (const auto& child : other._children)
	{
		this->_children.emplace_back(std::make_unique<BodyPart>(BodyPart{ *child }));
	}
}

BodyPart& BodyPart::operator=(BodyPart& other)
{
	if (this != &other)
	{
		this->name = other.name;
		this->type = other.type;
		this->size = other.size;
		this->_slotted = other._slotted;
		for (const auto& child : other._children)
		{
			this->_children.emplace_back(std::make_unique<BodyPart>(BodyPart{ *child }));
		}
	}
	
	return *this;
}

void BodyPart::attach(std::unique_ptr<BodyPart> newPart)
{
	newPart->_parent = this;
	_children.emplace_back(std::move(newPart));
}

bool BodyPart::addSlotItem(unsigned long itemID, EquipmentLayer layer)
{
	if (this->type != PartType::Hand && layer == EquipmentLayer::Held)
	{
		return false;
	}
	if (_slotted.contains(static_cast<int>(layer)))
	{
		return false;
	}
	_slotted[static_cast<int>(layer)] = itemID;
	return true;
}

std::optional<unsigned long> BodyPart::getSlotItem(EquipmentLayer layer) const
{
	if (this->type != PartType::Hand && layer == EquipmentLayer::Held)
	{
		return std::nullopt;
	}
	if (_slotted.contains(static_cast<int>(layer)))
	{
		return _slotted.at(static_cast<int>(layer));
	}
	return std::nullopt;
}

void BodyPart::removeSlotItem(unsigned long itemID)
{
	for (auto it = _slotted.begin(); it != _slotted.end();)
	{
		if (it->second == itemID)
		{
			it = _slotted.erase(it);
		}
		else
		{
			++it;
		}
	}
}

std::vector<unsigned long> BodyPart::getAllSlotted() const
{
	std::vector<unsigned long> result;
	for (auto& [layer, itemID] : _slotted)
	{
		result.push_back(itemID);
	}
	return result;
}

std::vector<unsigned long> BodyPart::getAllSlottedExcept(std::unordered_set<EquipmentLayer> exclude)
{
	std::vector<unsigned long> result;
	for (auto& [layer, itemID] : _slotted)
	{
		if (exclude.contains(static_cast<EquipmentLayer>(layer))) continue;
		result.push_back(itemID);
	}
	return result;
}


bool BodyPart::isConnectedTo(const BodyPart* part) const
{
	if (_parent == part) return true;
	for (auto& child : _children)
	{
		if (child.get() == part) return true;
	}
	return false;
}

// PART TREE //////////////////////////////////////////////////////////////////////////////////////////////////

PartTree::PartTree(std::unique_ptr<BodyPart> root)
	:_root(std::move(root))
{}

PartTree::PartTree(const PartTree& other)
{
	this->_root = std::make_unique<BodyPart>(BodyPart{ *other._root });
}

PartTree& PartTree::operator=(PartTree& other)
{
	if (this != &other)
	{
		this->_root = std::make_unique<BodyPart>(BodyPart{ *other._root });
	}
	return *this;
}

std::unique_ptr<BodyPart> PartTree::remove(const std::string& partName)
{
	for (auto&& part : flatten())
	{
		auto it = std::find_if(part->_children.begin(), part->_children.end(),
			[partName](const std::unique_ptr<BodyPart>& p)
			{
				return (p->name == partName);
			});
		if (it != part->_children.end())
		{
			part->_children.erase(std::remove(part->_children.begin(), part->_children.end(), *it));
			return std::move(*it);
		}
	}
	return nullptr;
}

int PartTree::size() const
{
	return flatten().size();
}

bool PartTree::contains(const std::string& partName) const
{
	auto found = search(partName);
	if (found) return true;
	else return false;
}

BodyPart* PartTree::search(const std::string& partName)
{
	return recursiveSearch(_root.get(), partName);
}

const BodyPart* PartTree::search(const std::string& partName) const
{
	return recursiveSearch(_root.get(), partName);
}

std::vector<BodyPart*> PartTree::search(PartType type)
{
	std::vector<BodyPart*> result;
	recursiveSearch(_root.get(), type, result);
	return result;
}

const std::vector<const BodyPart*> PartTree::search(PartType type) const
{
	std::vector<const BodyPart*> result;
	recursiveSearch(_root.get(), type, result);
	return result;
}

std::vector<BodyPart*> PartTree::flatten(FlattenType flattenHow)
{
	std::vector<BodyPart*> result;
	switch (flattenHow)
	{
		case FlattenType::DepthFirst:
		{
			recursiveSearch(_root.get(), PartType::Any, result);
		}
		break;
		case FlattenType::ByPartType:
		{
			for (int i = 0; i < static_cast<int>(PartType::Any); ++i)
			{
				auto typeVector = search(static_cast<PartType>(i));
				result.insert(result.end(), typeVector.begin(), typeVector.end());
			}
		}
		break;
	}
	
	
	return result;
}

const std::vector<const BodyPart*> PartTree::flatten(FlattenType flattenHow) const
{
	std::vector<const BodyPart*> result;
	switch (flattenHow)
	{
	case FlattenType::DepthFirst:
	{
		recursiveSearch(_root.get(), PartType::Any, result);
	}
	break;
	case FlattenType::ByPartType:
	{
		for (int i = 0; i < static_cast<int>(PartType::Any); ++i)
		{
			auto typeVector = search(static_cast<PartType>(i));
			result.insert(result.end(), typeVector.begin(), typeVector.end());
		}
	}
	break;
	}

	return result;
}

std::vector<std::string> PartTree::getPartsWithSlot(EquipmentSlot slot) const
{
	std::vector<std::string> result;
	switch (slot)
	{
	case EquipmentSlot::Head:
		for (auto part : search(PartType::Head))
		{
			result.emplace_back(part->name);
		}
		break;
	case EquipmentSlot::Torso:
		for (auto part : search(PartType::Torso))
		{
			result.emplace_back(part->name);
		}
		break;
	case EquipmentSlot::Arm:
		for (auto part : search(PartType::UpperLimb))
		{
			result.emplace_back(part->name);
		}
		break;
	case EquipmentSlot::Hand:
		for (auto part : search(PartType::Hand))
		{
			result.emplace_back(part->name);
		}
		break;
	case EquipmentSlot::Legs:
		result.emplace_back(EquipmentSlot2String.at(slot));
		break;
	case EquipmentSlot::Feet:
		result.emplace_back(EquipmentSlot2String.at(slot));
		break;
	default:
		break;
	}

	return result;
}

std::vector<std::string> PartTree::getPartsWithSlots(std::vector<EquipmentSlot> slots) const
{
	std::vector<std::string> result;
	for (auto slot : slots)
	{
		auto parts = getPartsWithSlot(slot);
		result.insert(result.end(), parts.begin(), parts.end());
	}

	return result;
}

std::optional<unsigned long> PartTree::getEquippedOnPart(const std::string& partName, EquipmentLayer layer)
{
	if (partName == "Legs")
	{
		auto slotParts = this->search(PartType::LowerLimb);
		for (auto part : slotParts)
		{
			if (part->getSlotItem(layer).has_value())
			{
				return part->getSlotItem(layer).value();
			}
		}
	}
	else if (partName == "Feet")
	{
		auto slotParts = this->search(PartType::Foot);
		for (auto part : slotParts)
		{
			if (part->getSlotItem(layer).has_value())
			{
				return part->getSlotItem(layer).value();
			}
		}
	}
	else
	{
		auto part = this->search(partName);
		if (part && part->getSlotItem(layer).has_value())
		{
			return part->getSlotItem(layer).value();
		}
	}

	return std::nullopt;
}

std::vector<unsigned long> PartTree::getAllEquippedOnPartExcept(const std::string& partName, std::unordered_set<EquipmentLayer> layers)
{
	std::vector<unsigned long> result;
	if (partName == "Legs")
	{
		std::unordered_set<unsigned long> tempResult;
		auto slotParts = this->search(PartType::LowerLimb);
		for (auto part : slotParts)
		{
			auto items = part->getAllSlottedExcept(layers);
			tempResult.insert(items.begin(), items.end());
		}
		result.insert(result.end(), tempResult.begin(), tempResult.end());
		return result;
	}
	else if (partName == "Feet")
	{
		std::unordered_set<unsigned long> tempResult;
		auto slotParts = this->search(PartType::Foot);
		for (auto part : slotParts)
		{
			auto items = part->getAllSlottedExcept(layers);
			tempResult.insert(items.begin(), items.end());
		}
		result.insert(result.end(), tempResult.begin(), tempResult.end());
		return result;
	}
	else
	{
		auto part = this->search(partName);
		return part->getAllSlottedExcept(layers);
	}
	return result;
}

std::vector<PartTree::PartItemPair> PartTree::getAllEquipped()
{
	std::vector<PartItemPair> result;
	for (auto part : this->flatten())
	{
		auto equipped = part->getAllSlotted();
		for (auto item : equipped)
		{
			result.emplace_back(part->name, item);
		}
	}
	return result;
}

std::vector<PartTree::PartItemPair> PartTree::getAllEquipped(std::unordered_set<EquipmentLayer> layers)
{
	std::vector<PartItemPair> result;
	for (auto part : this->flatten())
	{
		for (auto layer : layers)
		{
			auto equipped = part->getSlotItem(layer);
			if (equipped.has_value())
			{
				result.emplace_back(part->name, equipped.value());
			}
		}
	}
	return result;
}

std::vector<PartTree::PartItemPair> PartTree::getAllEquippedExcept(std::unordered_set<EquipmentLayer> layers)
{
	std::vector<PartItemPair> result;
	for (auto part : this->flatten())
	{
		for (auto layer : layers)
		{
			auto equipped = part->getAllSlottedExcept({ layer });
			for (auto item : equipped)
			{
				result.emplace_back(part->name, item);
			}
		}
	}
	return result;
}

bool PartTree::equipItem(unsigned long itemID, EquipmentLayer layer, const std::string& partName)
{
	if (partName == "Legs")
	{
		auto slotParts = this->search(PartType::LowerLimb);
		for (auto part : slotParts)
		{
			if (!part->addSlotItem(itemID, layer)) return false;
		}
	}
	else if (partName == "Feet")
	{
		auto slotParts = this->search(PartType::Foot);
		for (auto part : slotParts)
		{
			if (!part->addSlotItem(itemID, layer)) return false;
		}
	}
	else
	{
		auto slotPart = this->search(partName);
		if (!slotPart || !slotPart->addSlotItem(itemID, layer)) return false;
	}
	
	return true;
}

void PartTree::unequipItem(unsigned long itemID)
{
	for (auto part : flatten())
	{
		part->removeSlotItem(itemID);
	}
}

BodyPart* PartTree::recursiveSearch(BodyPart* root, const std::string& partName)
{
	if (!root) return nullptr;
	if (root->name == partName)
	{
		return root;
	}

	for (auto& child : root->_children)
	{
		if (auto result = recursiveSearch(child.get(), partName))
		{
			return result;
		}
	}

	return nullptr;
}

const BodyPart* PartTree::recursiveSearch(BodyPart* root, const std::string& partName) const
{
	if (!root) return nullptr;
	if (root->name == partName)
	{
		return root;
	}

	for (auto& child : root->_children)
	{
		if (auto result = recursiveSearch(child.get(), partName))
		{
			return result;
		}
	}

	return nullptr;
}

void PartTree::recursiveSearch(BodyPart* root, PartType type, std::vector<const BodyPart*>& result) const
{
	if (!root) return;
	if (root->type == type || type == PartType::Any)
	{
		result.push_back(root);
	}
	for (auto& child : root->_children)
	{
		recursiveSearch(child.get(), type, result);
	}
}

void PartTree::recursiveSearch(BodyPart* root, PartType type, std::vector<BodyPart*>& result)
{
	if (!root) return;
	if (root->type == type || type == PartType::Any)
	{
		result.push_back(root);
	}
	for (auto& child : root->_children)
	{
		recursiveSearch(child.get(), type, result);
	}
}

