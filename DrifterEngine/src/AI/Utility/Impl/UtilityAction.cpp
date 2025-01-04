#include "pch.h"
#include "UtilityAction.h"

UtilityTargetCategoryID UtilityAction::getTargetCategory() const
{
    return _targetCategory;
}

UtilityActionID UtilityAction::getActionID() const
{
    return _id;
}

void UtilityAction::createFromJson(const rapidjson::Value& json)
{
    if (json.HasMember("name"))
    {
        _id = entt::hashed_string{ json["name"].GetString() };
    }
    if (json.HasMember("target_type"))
    {
        _targetCategory = entt::hashed_string{ json["target_type"].GetString() };
    }
    if (json.HasMember("considerations"))
    {
        for (auto&& val : json["considerations"].GetArray())
        {
            Consideration newConsideration;
            auto obj = val.GetObject();
            newConsideration.inputID = entt::hashed_string{ obj["input"].GetString() };
            newConsideration.responseCurve.createFromJson(obj["curve"]);
            _considerations.push_back(std::move(newConsideration));
        }
    }
}
