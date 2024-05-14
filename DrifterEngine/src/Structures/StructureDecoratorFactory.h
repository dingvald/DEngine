#pragma once

namespace drft
{
	class StructureDecorator;
	class StructureDecoratorFactory
	{
	public:
		
		template<typename T>
		static void registerDecorator(std::string name)
		{
			static_assert(std::is_base_of<StructureDecorator, T>::value, "Type must be derived from StructureDecorator");
			_factoryMethods.emplace(name, []() {return std::make_unique<T>(); });
		}
		
		static std::unique_ptr<StructureDecorator> build(std::string name);

	private:
		using DecoratorFactoryMethod = std::function<std::unique_ptr<StructureDecorator>()>;
		using FactoryMethodMap = std::unordered_map<std::string, DecoratorFactoryMethod>;
		static FactoryMethodMap _factoryMethods;
	};
}



