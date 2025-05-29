#pragma once

namespace drft
{
	class Easing
	{
	public:
		static float linear(float f);
		static float easeOutElastic(float f);
		static float easeOutBack(float f);
		static float easeInOutCirc(float f);
		static float easeInBack(float f);
		static float easeInBackSlow(float f);
	};
}