#pragma once

namespace drft
{
	class StructureBaseShape;
	class StructureShapeFactory
	{
	public:

		template<typename T>
		static void registerShape(std::string name)
		{
			static_assert(std::is_base_of<StructureBaseShape, T>::value, "Type must be derived from StructureBaseShape");
			_factoryMethods.emplace(name, []() {return std::make_unique<T>(); });
		}

		static std::unique_ptr<StructureBaseShape> build(std::string name);

	private:
		using ShapeFactoryMethod = std::function<std::unique_ptr<StructureBaseShape>()>;
		using FactoryMethodMap = std::unordered_map<std::string, ShapeFactoryMethod>;
		static FactoryMethodMap _factoryMethods;
	};
}


