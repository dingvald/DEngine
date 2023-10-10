#pragma once
#include <vector>
#include <unordered_map>
#include <string>

enum class PartType
{
	Head,
	Torso,
	UpperLimb,
	LowerLimb,
	Hand,
	Foot,
	Any
};

enum class PartAlignment
{
	Center,
	Left,
	Right
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
	BodyPart(std::string name, PartType type, unsigned int size);
	BodyPart(const BodyPart& other);
	BodyPart& operator=(BodyPart& other);
	BodyPart(BodyPart&& other) = default;
	BodyPart& operator=(BodyPart&& other) = default;


	std::string name;
	PartType type = PartType::Head;
	unsigned int size = 0;

	void attach(std::unique_ptr<BodyPart> newPart);
	bool equip(unsigned long itemID, unsigned long layer);
	void unequip(unsigned long itemID);
	bool hasItemEquipped(unsigned int itemID);
	std::optional<unsigned long> getEquipped(unsigned int layer);
	std::vector<unsigned long> getEquipped();

private:
	friend class PartTree;
	std::map<unsigned int, unsigned long> _equipped;
	std::vector<std::unique_ptr<BodyPart>> _children;
};


class PartTree
{
public:
	PartTree(std::unique_ptr<BodyPart> root);
	PartTree(const PartTree& other);
	PartTree& operator=(PartTree other);
	PartTree(PartTree&& other) = default;
	PartTree& operator=(PartTree&& other) = default;

	std::unique_ptr<BodyPart> remove(const std::string& partName);
	bool contains(const std::string& partName) const;
	BodyPart* search(const std::string& partName);
	const BodyPart* search(const std::string& partName) const;
	std::vector<BodyPart*> search(PartType type);
	const std::vector<const BodyPart*> search(PartType type) const;
	std::vector<BodyPart*> flatten();

private:
	BodyPart* recursiveSearch(BodyPart* root, const std::string& partName);
	void recursiveSearch(BodyPart* root, PartType type, std::vector<const BodyPart*>& result) const;
	void recursiveSearch(BodyPart* root, PartType type, std::vector<BodyPart*>& result);
	const BodyPart* recursiveSearch(BodyPart* root, const std::string& partName) const;

private:
	std::unique_ptr<BodyPart> _root = nullptr;
};



