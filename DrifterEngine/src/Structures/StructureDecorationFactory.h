#pragma once

namespace drft
{
	class StructureDecoration;
	class StructureDecorationFactory
	{
	public:
		
		template<typename T>
		static void registerDecoration(std::string name)
		{
			static_assert(std::is_base_of<StructureDecoration, T>::value, "Type must be derived from StructureDecoration");
			_factoryMethods.emplace(name, []() {return std::make_unique<T>(); });
		}
		
		static std::unique_ptr<StructureDecoration> build(std::string name);

	private:
		using DecorationFactoryMethod = std::function<std::unique_ptr<StructureDecoration>()>;
		using FactoryMethodMap = std::unordered_map<std::string, DecorationFactoryMethod>;
		static FactoryMethodMap _factoryMethods;
	};
}



