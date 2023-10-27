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
	this->_equipped = other._equipped;
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
		this->_equipped = other._equipped;
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

bool BodyPart::equip(unsigned long itemID, EquipmentLayer layer)
{
	if (this->type != PartType::Hand && layer == EquipmentLayer::Held)
	{
		return false;
	}
	if (_equipped.contains(static_cast<int>(layer))) return false;
	_equipped[static_cast<int>(layer)] = itemID;
	return true;
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

void BodyPart::unequip(unsigned long itemID)
{
	int layerToErase = -1;
	for (auto& [layer, item] : _equipped)
	{
		if (itemID == item)
		{
			layerToErase = layer;
		}
	}
	if (layerToErase >= 0)
	{
		_equipped.erase(static_cast<unsigned int>(layerToErase));
	}
}

bool BodyPart::hasItemEquipped(unsigned int itemID)
{
	for (auto [layer, item] : _equipped)
	{
		if (item == itemID) return true;
	}
	return false;
}

std::optional<unsigned long> BodyPart::getEquipped(EquipmentLayer layer) const
{
	if (this->type != PartType::Hand && layer == EquipmentLayer::Held)
	{
		return std::nullopt;
	}
	if (_equipped.contains(static_cast<int>(layer)))
	{
		return _equipped.at(static_cast<int>(layer));
	}
	return std::nullopt;
}

std::vector<unsigned long> BodyPart::getEquipped() const
{
	std::vector<unsigned long> result;
	for (auto [layer, itemID] : _equipped)
	{
		result.push_back(itemID);
	}
	return result;
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
	std::unordered_set<unsigned long> alreadyIncluded;
	for (auto part : this->flatten())
	{
		auto equipped = part->getEquipped();
		for (auto item : equipped)
		{
			if (alreadyIncluded.contains(item)) continue;
			result.push_back({ part->name, item });
			alreadyIncluded.insert(item);
		}
	}
	return result;
}

std::vector<PartTree::PartItemPair> PartTree::getAllHeldEquipped()
{
	std::vector<PartItemPair> result;
	std::unordered_set<unsigned long> alreadyIncluded;
	for (auto part : this->search(PartType::Hand))
	{
		auto optionalItem = part->getEquipped(EquipmentLayer::Held);
		if (optionalItem.has_value())
		{
			if (alreadyIncluded.contains(optionalItem.value())) continue;
			result.push_back({ part->name, optionalItem.value() });
			alreadyIncluded.insert(optionalItem.value());
		}
	}
	return result;
}

std::vector<PartTree::PartItemPair> PartTree::getAllWornEquipped()
{
	std::vector<PartItemPair> result;
	std::unordered_set<unsigned long> alreadyIncluded;
	for (auto part : this->flatten())
	{
		for (int i = static_cast<int>(EquipmentLayer::Held) + 1; i <= static_cast<int>(EquipmentLayer::OverAll); ++i)
		{
			auto optionalItem = part->getEquipped(static_cast<EquipmentLayer>(i));
			if (optionalItem.has_value())
			{
				if (alreadyIncluded.contains(optionalItem.value())) continue;
				result.push_back({ part->name, optionalItem.value() });
				alreadyIncluded.insert(optionalItem.value());
			}
		}
	}
	return result;
}

CompatibleParts PartTree::getCompatiblePartsForItem(const std::vector<std::string>& slots, const std::vector<std::string>& covers, EquipmentLayer layer)
{
	CompatibleParts result;
	std::vector<BodyPart*> potentialParts;
	for (auto& slot : slots)
	{
		potentialParts = this->search(string2PartType.at(slot));
		for (auto it = potentialParts.begin(); it != potentialParts.end();)
		{
			if ((*it)->getEquipped(layer).has_value())
			{
				result.conflicts.emplace((*it)->name, (*it)->getEquipped(layer).value());
				it = potentialParts.erase(it);
			}
			else
			{
				++it;
			}
		}
	}

	// check covers
	for (auto& partName : covers)
	{
		// check if a specific part
		auto specificPart = this->search(partName);
		if (specificPart)
		{
			auto itemEquipped = specificPart->getEquipped(layer);
			if (itemEquipped.has_value())
			{
				result.conflicts.emplace(partName, itemEquipped.value());
				result.canEquip = false;
			}
		}
		else
		{
			// check if a generic typed part
			if (!string2PartType.contains(partName))
			{
				// Cannot equip item on body, so early out
				result.canEquip = false;
				return result;
			}
			auto typedParts = this->search(string2PartType.at(partName));
			for (auto it = potentialParts.begin(); it != potentialParts.end();)
			{
				bool isConnected = false;
				for (auto typedPart : typedParts)
				{
					auto optionalItem = typedPart->getEquipped(layer);
					if ((*it)->isConnectedTo(typedPart))
					{
						if (optionalItem.has_value())
						{
							result.conflicts.emplace(typedPart->name, optionalItem.value());
						}
						else
						{
							isConnected = true;
						}
					}
				}
				if (!isConnected)
				{
					it = potentialParts.erase(it);
				}
				else
				{
					++it;
				}
			}
		}
	}
	if (result.canEquip && !potentialParts.empty())
	{
		for (auto& part : potentialParts)
		{
			result.freeSlots.insert(part->name);
		}
	}
	else
	{
		result.canEquip = false;
	}

	return result;
}

void PartTree::unequipItemFromBody(unsigned long itemID)
{
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
