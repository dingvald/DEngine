#include "pch.h"
#include "EasingFunctions.h"

// Easings taken from easing.net

float drft::Easing::linear(float f)
{
	return f;
}

float drft::Easing::easeOutElastic(float f)
{
	const float c4 = (2 * 3.1415f) / 3;
	return f == 0 ? 0 : (f == 1.f ? 1.f : std::powf(2, -10 * f) * std::sinf((f * 10 - 0.75f) * c4) + 1);
}

float drft::Easing::easeOutBack(float f)
{
	const float c1 = 1.70158f;
	const float c3 = c1 + 1.f;

	return 1 + c3 * std::powf(f - 1, 3) + c1 * std::powf(f - 1, 2);
}

float drft::Easing::easeInOutCirc(float f)
{
	return f < 0.5f
		? (1 - std::sqrtf(1.f - std::powf(2.f * f, 2.f))) / 2.f
		: (std::sqrtf(1.f - std::powf(-2.f * f + 2.f, 2.f)) + 1.f) / 2.f;
}
