#pragma once
#include "StructureComponent.h"

namespace drft
{
	class StructureInstance;
	class StructureComponent;
	using StructureInstancePtr = std::unique_ptr<StructureInstance>;

	class StructureBlueprint
	{
	private:
		friend class StructureFactory;
		StructureBlueprint(std::string name);
		void createFromJSON(const rapidjson::Value& json);
		StructureInstancePtr build() const;

	private:
		using ComponentPtr = std::unique_ptr<StructureComponent>;
		std::string _name;
		std::vector<ComponentPtr> _components;
	};
}


