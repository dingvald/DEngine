#include "pch.h"
#include "UtilityAction.h"

UtilityTargetCategoryID UtilityAction::getTargetType() const
{
    return _targetType;
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
        _targetType = entt::hashed_string{ json["target_type"].GetString() };
    }
    if (json.HasMember("considerations"))
    {
        for (auto&& val : json["considerations"].GetArray())
        {
            Consideration newConsideration;
            auto obj = val.GetObject();
            newConsideration.inputID = entt::hashed_string{ obj["input"].GetString() };
            newConsideration.responseCurve.createFromJson(obj["curve"]);
            _considerations.emplace_back(std::move(newConsideration));
        }
    }
}
