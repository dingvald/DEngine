#pragma once
#include <JSON/ICreateFromJson.h>

class CraftingIngredient : public ICreateFromJson
{
public:
	void createFromJson(const rapidjson::Value& json) override;

	const std::string& getEntityName() const;
	int getAmount() const;

private:
	friend class cereal::access;
	template<typename Archive>
	void serialize(Archive& ar)
	{
		ar(_name, _amount);
	}

private:
	std::string _name;
	int _amount;
};

