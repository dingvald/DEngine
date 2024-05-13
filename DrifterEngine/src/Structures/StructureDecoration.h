#pragma once

namespace drft
{
	class StructureBaseShape;
	class StructureDecoration
	{
	public:
		virtual ~StructureDecoration() = default;
		virtual void createFromJSON(const rapidjson::Value& json) = 0;
		virtual void apply(StructureBaseShape& shape) const = 0;
	};
}


