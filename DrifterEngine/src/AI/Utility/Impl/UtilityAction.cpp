#include "pch.h"
#include "UtilityAction.h"
#include <Utility/StandardLogger.h>

AiTargetType UtilityAction::getTargetType() const
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
        auto targetTypeName = json["target_type"].GetString();
        if (StringToAiTargetType.contains(targetTypeName))
        {
            _targetType = StringToAiTargetType.at(targetTypeName);
        }
        else
        {
            LOG_ERROR("target_type {} does not exist.", targetTypeName);
        }
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
