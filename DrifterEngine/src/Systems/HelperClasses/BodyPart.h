#pragma once
#include <JSON/ICreateFromJson.h>

class BodyPart : ICreateFromJson
{
public:
	struct Slot
	{
		Slot() = default;
		Slot(const char* str);

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(id, uid, item);
		}
		std::string id = {};
		int uid = 0;
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
	const std::vector<Slot>& getSlots() const;
	Slot* getSlot(const std::string& slot);

private:
	friend class BodyPartTree;

	std::string _name = {};
	std::vector<BodyPart> _children;
	std::vector<Slot> _slots;
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

	unsigned long swapItem(unsigned long item, const std::string& slot, int uid);
	bool equipItem(unsigned long item, const std::string& slot, int uid);
	unsigned long unequipItem(const std::string& slot, int uid);
	bool removeItem(unsigned long item);

	std::vector<unsigned long> getAllEquipped() const;
	std::vector<unsigned long> getEquipped(const std::string& slot) const;
	unsigned long getEquipped(const std::string& slot, int uid) const;

private:
	static BodyPart* searchForPart(BodyPart& root, const std::string& partName);
	static const BodyPart* searchForPart(const BodyPart& root, const std::string& partName);
	static std::vector<BodyPart*> getAllParts(BodyPart& root);
	static std::vector<const BodyPart*> getAllParts(const BodyPart& root);
	static std::vector<BodyPart*> getAllPartsWithSlot(BodyPart& root, const std::string& slot);
	static std::vector<const BodyPart*> getAllPartsWithSlot(const BodyPart& root, const std::string& slot);

	static BodyPart::Slot* searchForSlot(BodyPart& root, const std::string& slot, int uid);
	static const BodyPart::Slot* searchForSlot(const BodyPart& root, const std::string& slot, int uid);
	static std::vector<BodyPart::Slot*> searchForSlot(BodyPart& root, const std::string& slot);
	static std::vector<const BodyPart::Slot*> searchForSlot(const BodyPart& root, const std::string& slot);
	static std::vector<BodyPart::Slot*> searchForSlotWithItem(BodyPart& root, unsigned long item);

	static std::vector<BodyPart::Slot*> getAllSlots(BodyPart& root);
	static std::vector<const BodyPart::Slot*> getAllSlots(const BodyPart& root);

private:
	BodyPart _root;
};