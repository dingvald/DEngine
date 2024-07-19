#pragma once

#include <ProcGen/GenerationContext.h>

namespace drft::gen
{
	class ProbabilityMultiplier
	{
	public:
		void createFromJSON(const rapidjson::Value& json);

		// Applies the multiplier
		// Returns 1.0 if the multiplier does not apply
		float apply(sf::Vector2i position, GenerationContext& context) const;

	protected:
		using HitCount = unsigned int;
		virtual HitCount _test(sf::Vector2i position, GenerationContext& context) const = 0;
		virtual void _createFromJSON(const rapidjson::Value& json) = 0;

	private:
		float _multiplier = 0.0f;
	};
}


