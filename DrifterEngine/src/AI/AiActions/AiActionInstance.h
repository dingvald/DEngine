#pragma once

#include <Spatial/TilePosition.h>
#include <EnTT/entt.h>

class IAiAction;

class AiActionInstance
{
public:
	void reset(const IAiAction* action = nullptr);

	bool isInRange(entt::const_handle actor) const;
	bool isValid(entt::const_handle actor) const;
	void perform(entt::handle actor) const;

	const IAiAction* action = nullptr;
	std::optional<drft::TilePosition> target = std::nullopt;
	float score = 0.f;
};