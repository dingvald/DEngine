#pragma once
#include "ProcGen/GenerationParameters.h"

namespace drft::gen
{
	struct GenerationContext;

	class ISpawningPredicate
	{
	public:
		void setContext(GenerationContext& context);
		void setParameters(GenerationParameters& params);
		virtual bool canPlace(sf::Vector2i position) = 0;
	protected:
		GenerationContext* _context = nullptr;
		GenerationParameters* _params = nullptr;
	};
}