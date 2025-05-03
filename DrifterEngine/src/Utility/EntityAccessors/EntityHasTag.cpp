#include "pch.h"
#include "EntityHasTag.h"

bool drft::util::entityHasTag(entt::const_handle entity, entt::id_type tag)
{
    auto storage = entity.registry()->storage(tag);
    if (!storage) return false;

    return storage->contains(entity.entity());
}
