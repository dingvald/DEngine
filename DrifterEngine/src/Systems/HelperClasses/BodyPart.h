#pragma once
#include <JSON/ICreateFromJson.h>


class BodyPart : ICreateFromJson
{
public:
	struct Slot
	{
		enum class Type
		{
			Torso,
			Head,
			UpperLimb,
			Hand,
			Held,
			LowerLimb,
			Feet
		};
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(type, item);
		}
		Type type = Type::Held;
		unsigned int item = 0;
	};

public:
	BodyPart() = default;
	BodyPart(const std::string& partName);

	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(_name, _children, _slots, _size);
	}
	void createFromJson(const rapidjson::Value& json) override;

	const std::string& getName() const;
	const std::unordered_map<std::string, Slot>& getSlots() const;
	Slot* getSlot(const std::string& slot);
	Slot* getSlotType(Slot::Type type);

private:
	friend class BodyPartTree;

	std::string _name = {};
	std::vector<BodyPart> _children;
	std::unordered_map<std::string, Slot> _slots;
	int _size = 0;
};

class BodyPartTree : ICreateFromJson
{
public:
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(_root);
	}
	void createFromJson(const rapidjson::Value& json) override;

	std::vector<BodyPart*> getAllParts();
	std::vector<const BodyPart*> getAllParts() const;
	std::vector<BodyPart*> getAllPartsWithSlot(const std::string& slot);
	std::vector<const BodyPart*> getAllPartsWithSlot(const std::string& slot) const;
	const BodyPart::Slot* getSlot(const std::string& slot) const;
	BodyPart::Slot* getSlot(const std::string& slot);

	std::vector<BodyPart*> getAllPartsWithSlotType(BodyPart::Slot::Type type);
	std::vector<const BodyPart*> getAllPartsWithSlotType(BodyPart::Slot::Type type) const;

	BodyPart* getPartWithSlotType(BodyPart::Slot::Type type, const std::string& uniqueSlotTypeModifier);

	unsigned long swapItem(unsigned long item, const std::string& slot);
	bool equipItem(unsigned long item, const std::string& slot);
	unsigned long unequipItem(const std::string& slot);
	bool removeItem(unsigned long item);

	std::vector<unsigned long> getAllEquipped() const;
	unsigned long getEquipped(const std::string& slot) const;
	unsigned long getEquipped(BodyPart::Slot::Type type, const std::string& uniqueSlotTypeModifier) const;

private:
	static BodyPart* searchForPart(BodyPart& root, const std::string& partName);
	static const BodyPart* searchForPart(const BodyPart& root, const std::string& partName);
	static std::vector<BodyPart*> getAllParts(BodyPart& root);
	static std::vector<const BodyPart*> getAllParts(const BodyPart& root);
	static std::vector<BodyPart*> getAllPartsWithSlot(BodyPart& root, const std::string& slot);
	static std::vector<const BodyPart*> getAllPartsWithSlot(const BodyPart& root, const std::string& slot);
	static std::vector<BodyPart*> getAllPartsWithSlotType(BodyPart& root, BodyPart::Slot::Type type);
	static std::vector<const BodyPart*> getAllPartsWithSlotType(const BodyPart& root, BodyPart::Slot::Type type);

	static BodyPart::Slot* searchForSlot(BodyPart& root, const std::string& slot);
	static const BodyPart::Slot* searchForSlot(const BodyPart& root, const std::string& slot);
	static std::vector<BodyPart::Slot*> searchForSlotWithItem(BodyPart& root, unsigned long item);

	static std::vector<BodyPart::Slot*> getAllSlots(BodyPart& root);
	static std::vector<const BodyPart::Slot*> getAllSlots(const BodyPart& root);

private:
	BodyPart _root;
};