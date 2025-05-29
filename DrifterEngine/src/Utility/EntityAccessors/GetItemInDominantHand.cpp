#include "pch.h"
#include "GetItemInDominantHand.h"
#include <Components/BodyComponent.h>
#include <Systems/Helpers/GetDominantSide.h>
#include <Systems/Helpers/ItemDatabase.h>

entt::const_handle drft::util::getItemInDominantHand(entt::const_handle entity)
{
    if (auto body = entity.try_get<BodyComponent>())
    {
        auto item = body->parts.getEquipped(BodyPart::Slot::Type::Held, util::getDominantSide(entity));
        auto itemEntity = ItemDatabase::getEntityFromItemID(item);
        return entt::const_handle{ *entity.registry(), itemEntity };
    }
    return {};
}
