#pragma once

namespace drft
{
	class StructureShapeInstance;
	class StructureBaseShape
	{
	public:
		virtual void createFromJSON(const rapidjson::Value& json) = 0;
		std::unique_ptr<StructureShapeInstance> generate();
	protected:
		virtual void doGenerate(StructureShapeInstance& shape) = 0;
	};
}

