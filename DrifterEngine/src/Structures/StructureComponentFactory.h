#pragma once

namespace drft
{
	class StructureComponent;
	class StructureComponentFactory
	{
	public:
		
		template<typename T>
		static void registerComponent(std::string name)
		{
			static_assert(std::is_base_of<StructureComponent, T>::value, "Type must be derived from StructureComponent");
			_factoryMethods.emplace(name, []() {return std::make_unique<T>(); });
		}
		
		static std::unique_ptr<StructureComponent> build(std::string name);


	private:
		using ComponentFactoryMethod = std::function<std::unique_ptr<StructureComponent>()>;
		using FactoryMethodMap = std::unordered_map<std::string, ComponentFactoryMethod>;
		static FactoryMethodMap _factoryMethods;
	};
}



