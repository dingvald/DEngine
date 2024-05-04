#pragma once

namespace drft
{
	class StructureInstance;
	using StructureInstancePtr = std::unique_ptr<StructureInstance>;

	class StructureBlueprint
	{
	private:
		friend class StructureFactory;
		StructureBlueprint(std::string name);
		void createFromJSON(const rapidjson::Value& json);
		StructureInstancePtr build() const;

	private:
		std::string _name;
	};
}


