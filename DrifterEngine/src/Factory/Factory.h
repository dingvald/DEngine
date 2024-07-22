#pragma once

template<typename T>
class Factory
{
public:
	template<typename U>
	static void registerType(std::string name)
	{
		static_assert(std::is_base_of<T, U>::value, "Type must be derived from type T");
		static_assert(std::is_default_constructible<U>::value, "Type must be default contructable");

		_factoryMethods.emplace(std::move(name), []() {return std::make_unique<U>(); });
	}

	static std::unique_ptr<T> build(std::string name);

private:
	using FactoryMethod = std::function<std::unique_ptr<T>()>;
	using FactoryMethodMap = std::unordered_map<std::string, FactoryMethod>;
	static FactoryMethodMap _factoryMethods;

};

template<typename T>
Factory<T>::FactoryMethodMap Factory<T>::_factoryMethods = {};

template<typename T>
inline std::unique_ptr<T> Factory<T>::build(std::string name)
{
	if (!_factoryMethods.contains(name))
	{
		std::cout << "Factory does not contain " << name << std::endl;
		return nullptr;
	}
	return std::move(_factoryMethods.at(name)());
}
