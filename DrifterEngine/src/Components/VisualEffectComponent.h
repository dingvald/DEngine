#pragma once

#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"

struct VisualEffectComponent
{
	std::optional<float> ttl = std::nullopt;
	std::function<float(float)> fadeFunc = [](float f) {return f;};
	bool requiresInFOV = true;

	float elapsed = 0;
};


