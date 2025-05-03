#include "pch.h"
#include "CraftingIngredient.h"

void CraftingIngredient::createFromJson(const rapidjson::Value& json)
{
	_name = json.GetArray()[0].GetString();
	_amount = json.GetArray()[1].GetInt();
}

const std::string& CraftingIngredient::getEntityName() const
{
	return _name;
}

int CraftingIngredient::getAmount() const
{
	return _amount;
}
