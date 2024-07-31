#pragma once
#include "Utility/Math.h"

namespace drft
{
	class StructureShapeInstance;

	class StructureDecorator
	{
	public:
		virtual void createFromJSON(const rapidjson::Value& json);
		virtual void apply(StructureShapeInstance& shape) const;

	private:
		float _probability = 1.0f;
		math::Range<int> _number;
		entt::id_type _tag;
		std::string _entityName;
	};
}


