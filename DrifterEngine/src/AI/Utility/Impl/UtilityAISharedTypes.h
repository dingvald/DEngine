#pragma once
#include <cstdint>
#include <EnTT/entt.h>

using UtilityActionID = std::uint32_t;
const UtilityActionID NullUtilityAction = entt::hashed_string("NULL");

using UtilityTargetCategoryID = std::uint32_t;
const UtilityTargetCategoryID NullUtilityTargetCategory = entt::hashed_string("NULL");

using UtilityInputID = std::uint32_t;
const UtilityInputID NullUtilityInput = entt::hashed_string("NULL");

using UtilityArchetypeID = std::uint32_t;
const UtilityInputID NullUtilityArchetype = entt::hashed_string("NULL");