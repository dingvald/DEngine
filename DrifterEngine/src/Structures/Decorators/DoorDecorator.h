#pragma once
#include "Structures/StructureDecorator.h"
#include "Utility/Math.h"

namespace drft
{
	class DoorDecorator : public StructureDecorator
	{
	public:
		virtual void createFromJSON(const rapidjson::Value& json) override;
		virtual void apply(StructureShapeInstance& shape) const override;

	private:
		math::Range<int> _numberOfDoors = { 1,1 };
		std::vector<std::pair<std::string, int>> _entityWeights;
	};
}

