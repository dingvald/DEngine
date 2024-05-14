#pragma once
#include "Structures/StructureDecorator.h"

namespace drft
{
	class WallDecorator : public StructureDecorator
	{
	public:
		virtual void createFromJSON(const rapidjson::Value& json) override;
		virtual void apply(StructureShapeInstance& shape) const override;

	private:
		float _integrity = 1.0f;
		std::vector<std::pair<std::string, int>> _entityWeights;
	};
}


