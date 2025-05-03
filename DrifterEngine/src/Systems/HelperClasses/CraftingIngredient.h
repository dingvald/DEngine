#pragma once
#include <JSON/ICreateFromJson.h>

class CraftingIngredient : public ICreateFromJson
{
public:
	void createFromJson(const rapidjson::Value& json) override;

	entt::id_type getEntityId() const;
	int getAmount() const;

private:
	friend class cereal::access;
	template<typename Archive>
	void serialize(Archive& ar)
	{
		ar(_entityId, _amount);
	}

private:
	entt::id_type _entityId;
	int _amount;
};

