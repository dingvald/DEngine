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
	this->_covering = other._covering;
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
		this->_covering = other._covering;
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
	addCoveringItem(itemID, layer);
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
	removeCoveringItem(itemID);
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

void BodyPart::addCoveringItem(unsigned long itemID, EquipmentLayer layer)
{
	if (this->type != PartType::Hand && layer == EquipmentLayer::Held)
	{
		return;
	}
	_covering[static_cast<int>(layer)].push_back(itemID);
}

std::vector<unsigned long> BodyPart::getCoveringItems(EquipmentLayer layer) const
{
	if (this->type != PartType::Hand && layer == EquipmentLayer::Held)
	{
		return std::vector<unsigned long>{};
	}
	if (_covering.contains(static_cast<int>(layer)))
	{
		return _covering.at(static_cast<int>(layer));
	}
	return std::vector<unsigned long>{};
}

void BodyPart::removeCoveringItem(unsigned long itemID)
{
	for (auto& [layer, items] : _covering)
	{
		items.erase(std::remove(items.begin(), items.end(), itemID), items.end());
	}
}

std::vector<unsigned long> BodyPart::getAllCoveringItems() const
{
	std::vector<unsigned long> result;
	for (auto& [layer, items] : _covering)
	{
		for (auto itemID : items)
		{
			result.push_back(itemID);
		}
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

std::vector<PartTree::PartItemPair> PartTree::getAllEquipped()
{
	std::vector<PartItemPair> result;
	for (auto part : this->flatten())
	{
		auto equipped = part->getAllSlotted();
		for (auto item : equipped)
		{
			result.push_back({ part->name, item });
		}
	}
	return result;
}

std::vector<PartTree::PartItemPair> PartTree::getAllHeldEquipped()
{
	std::vector<PartItemPair> result;
	for (auto part : this->search(PartType::Hand))
	{
		auto item = part->getSlotItem(EquipmentLayer::Held);
		if (item.has_value())
		{
			result.push_back({ part->name, item.value() });
		}
	}
	return result;
}

std::vector<PartTree::PartItemPair> PartTree::getAllWornEquipped()
{
	std::vector<PartItemPair> result;
	for (auto part : this->flatten())
	{
		for (int i = static_cast<int>(EquipmentLayer::Held) + 1; i <= static_cast<int>(EquipmentLayer::OverAll); ++i)
		{
			auto item = part->getSlotItem(static_cast<EquipmentLayer>(i));
			if (item.has_value())
			{
				result.push_back({ part->name, item.value() });
			}
		}
	}
	return result;
}

std::vector<std::string> PartTree::getSlotPartsForItem(const std::vector<std::string>& slots)
{
	std::vector<std::string> result;
	for (auto& slot : slots)
	{
		auto parts = this->search(string2PartType.at(slot));
		for (auto part : parts)
		{
			result.push_back(part->name);
		}
	}

	return result;
}

bool PartTree::equipItem(unsigned long itemID, EquipmentLayer layer, const std::string& slot, const std::vector<std::string>& covering)
{
	auto slotPart = this->search(slot);
	if (!slotPart || !slotPart->addSlotItem(itemID, layer)) return false;

	for (auto& coveredPartName : covering)
	{
		if (auto coveredPart = this->search(coveredPartName))
		{
			coveredPart->addCoveringItem(itemID, layer);
		}
	}
	return true;
}

void PartTree::unequipItem(unsigned long itemID)
{
	for (auto part : flatten())
	{
		part->removeSlotItem(itemID);
		part->removeCoveringItem(itemID);
	}
}

std::vector<std::string> PartTree::getCoveredPartsForItem(const std::string& slot, const std::vector<std::string> covers, EquipmentLayer layer)
{
	std::vector<std::string> result;
	auto slotPart = this->search(slot);
	// check covers
	for (auto& partName : covers)
	{
		// check if a specific part
		auto specificPart = this->search(partName);
		if (specificPart)
		{
			result.push_back(specificPart->name);
		}
		else
		{
			auto typedParts = this->search(string2PartType.at(partName));
			for (auto typedPart : typedParts)
			{
				if (slotPart->isConnectedTo(typedPart))
				{
					result.push_back(typedPart->name);
				}
			}
		}
	}

	return result;
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
