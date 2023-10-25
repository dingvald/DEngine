#pragma once
#include <vector>
#include <unordered_map>
#include <string>

enum class PartType
{
	Head,
	Torso,
	UpperLimb,
	Hand,
	Waist,
	LowerLimb,
	Foot,
	Any
};

enum class EquipmentLayer
{
	Held,
	Base,
	Outer,
	OverAll
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
	{"Waist", PartType::Waist},
	{"Lower Limb", PartType::LowerLimb},
	{"Hand", PartType::Hand},
	{"Foot", PartType::Foot}
};

static inline const std::unordered_map<PartType, std::string> partType2String =
{
	{PartType::Head, "Head"},
	{PartType::Torso, "Torso"},
	{PartType::UpperLimb, "Upper Limb"},
	{PartType::Waist, "Waist"},
	{PartType::LowerLimb, "Lower Limb"},
	{PartType::Hand, "Hand"},
	{PartType::Foot, "Foot"}
};

struct CompatibleParts
{
	std::unordered_set<std::string> freeSlots;
	std::unordered_map<std::string, unsigned long> conflicts;
	bool canEquip = true;
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
	bool equip(unsigned long itemID, EquipmentLayer layer);
	bool isConnectedTo(const BodyPart* part) const;
	
	bool hasItemEquipped(unsigned int itemID);
	std::optional<unsigned long> getEquipped(EquipmentLayer layer) const;
	std::vector<unsigned long> getEquipped() const;

private:
	void unequip(unsigned long itemID);
	friend class cereal::access;
	template<class Archive>
	void save(Archive& archive) const
	{
		archive(name, size, static_cast<int>(type));
		archive(_equipped);
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
		archive(_equipped);
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
	std::map<int, unsigned long> _equipped;
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

	std::vector<unsigned long> getAllEquipped();
	std::vector<unsigned long> getAllHeldEquipped();
	std::vector<unsigned long> getAllWornEquipped();
	CompatibleParts getCompatiblePartsForItem(const std::vector<std::string>& slots, const std::vector<std::string>& covers, EquipmentLayer layer);
	void unequipItemFromBody(unsigned long itemID);

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