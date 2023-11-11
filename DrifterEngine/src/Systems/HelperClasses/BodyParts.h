#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include "EquipmentSlots.h"

enum class PartType
{
	Head,
	Torso,
	UpperLimb,
	Hand,
	LowerLimb,
	Foot,
	Any
};

enum class FlattenType
{
	DepthFirst,
	ByPartType
};

static inline const std::unordered_map<std::string, PartType> string2PartType =
{
	{"Head", PartType::Head},
	{"Torso", PartType::Torso},
	{"Upper Limb", PartType::UpperLimb},
	{"Lower Limb", PartType::LowerLimb},
	{"Hand", PartType::Hand},
	{"Foot", PartType::Foot}
};

static inline const std::unordered_map<PartType, std::string> partType2String =
{
	{PartType::Head, "Head"},
	{PartType::Torso, "Torso"},
	{PartType::UpperLimb, "Upper Limb"},
	{PartType::LowerLimb, "Lower Limb"},
	{PartType::Hand, "Hand"},
	{PartType::Foot, "Foot"}
};

struct BodyPart
{
	BodyPart() = default;
	BodyPart(std::string name, PartType type, unsigned int size);
	BodyPart(const BodyPart& other);
	BodyPart& operator=(BodyPart& other);
	BodyPart(BodyPart&& other) = default;
	BodyPart& operator=(BodyPart&& other) = default;

	std::string name;
	PartType type = PartType::Head;
	unsigned int size = 0;

	void attach(std::unique_ptr<BodyPart> newPart);

	bool addSlotItem(unsigned long itemID, EquipmentLayer layer);
	std::optional<unsigned long> getSlotItem(EquipmentLayer layer) const;
	void removeSlotItem(unsigned long itemID);
	std::vector<unsigned long> getAllSlotted() const;
	std::vector<unsigned long> getAllSlottedExcept(std::unordered_set<EquipmentLayer> exclude);

	bool isConnectedTo(const BodyPart* part) const;

private:
	friend class cereal::access;
	template<class Archive>
	void save(Archive& archive) const
	{
		archive(name, size, static_cast<int>(type));
		archive(_slotted);
		archive(static_cast<int>(_children.size()));
		for (auto& part : _children)
		{
			archive(*(part.get()));
		}
	}

	template<class Archive>
	void load(Archive& archive)
	{
		archive(name, size, static_cast<PartType>(type));
		archive(_slotted);
		int childrenSize = 0;
		archive(childrenSize);
		for (int i = 0; i < childrenSize; ++i)
		{
			BodyPart bodyPart;
			archive(bodyPart);
			bodyPart._parent = this;
			_children.push_back(std::make_unique<BodyPart>(bodyPart));
		}
	}

private:
	friend class PartTree;
	BodyPart* _parent = nullptr;
	std::map<int, unsigned long> _slotted;
	std::vector<std::unique_ptr<BodyPart>> _children;
};

class PartTree
{
public:
	PartTree() = default;
	PartTree(std::unique_ptr<BodyPart> root);
	PartTree(const PartTree& other);
	PartTree& operator=(PartTree& other);
	PartTree(PartTree&& other) = default;
	PartTree& operator=(PartTree&& other) = default;

	std::unique_ptr<BodyPart> remove(const std::string& partName);
	int size() const;
	bool contains(const std::string& partName) const;
	BodyPart* search(const std::string& partName);
	const BodyPart* search(const std::string& partName) const;
	std::vector<BodyPart*> search(PartType type);
	const std::vector<const BodyPart*> search(PartType type) const;
	std::vector<BodyPart*> flatten(FlattenType flattenHow = FlattenType::ByPartType);
	const std::vector<const BodyPart*> flatten(FlattenType flattenHow = FlattenType::ByPartType) const;

	std::vector<std::string> getPartsWithSlot(EquipmentSlot slot) const;
	std::vector<std::string> getPartsWithSlots(std::vector<EquipmentSlot> slots) const; 
	struct PartItemPair
	{
		std::string partName;
		unsigned long itemID;
	};

	std::optional<unsigned long> getEquippedOnPart(const std::string& partName, EquipmentLayer layer);
	std::vector<unsigned long> getAllEquippedOnPartExcept(const std::string& partName, std::unordered_set<EquipmentLayer> layers);
	std::vector<PartItemPair> getAllEquipped();
	std::vector<PartItemPair> getAllEquipped(std::unordered_set<EquipmentLayer> layers);
	std::vector<PartItemPair> getAllEquippedExcept(std::unordered_set<EquipmentLayer> layers);

	bool equipItem(unsigned long itemID, EquipmentLayer layer, const std::string& partName);
	void unequipItem(unsigned long itemID);

private:
	BodyPart* recursiveSearch(BodyPart* root, const std::string& partName);
	void recursiveSearch(BodyPart* root, PartType type, std::vector<const BodyPart*>& result) const;
	void recursiveSearch(BodyPart* root, PartType type, std::vector<BodyPart*>& result);
	const BodyPart* recursiveSearch(BodyPart* root, const std::string& partName) const;

	friend class cereal::access;
	template<class Archive>
	void save(Archive& archive) const
	{
		archive(*(_root.get()));
	}

	template<class Archive>
	void load(Archive& archive)
	{
		BodyPart root;
		archive(root);
		_root = std::make_unique<BodyPart>(root);
	}
private:
	
	std::unique_ptr<BodyPart> _root = nullptr;
};