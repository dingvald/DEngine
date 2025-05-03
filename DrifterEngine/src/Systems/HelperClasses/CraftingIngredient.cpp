#include "pch.h"
#include "CraftingIngredient.h"

void CraftingIngredient::createFromJson(const rapidjson::Value& json)
{
	_entityId = entt::hashed_string{ json.GetArray()[0].GetString() };
	_amount = json.GetArray()[1].GetInt();
}

entt::id_type CraftingIngredient::getEntityId() const
{
	return _entityId;
}

int CraftingIngredient::getAmount() const
{
	return _amount;
}
