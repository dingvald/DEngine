#pragma once
#include "Structures/StructureBaseShape.h"
#include "Utility/Math.h"

namespace drft
{
	class RectangleShape : public StructureBaseShape
	{
	public:
		virtual void createFromJSON(const rapidjson::Value& json) override;

	protected:
		virtual void doGenerate(StructureShapeInstance& shape) override;

	private:
		math::Range<int> _width;
		math::Range<int> _height;
		std::unordered_set<entt::id_type> _outlineTags;
		std::unordered_set<entt::id_type> _fillTags;
	};
}


