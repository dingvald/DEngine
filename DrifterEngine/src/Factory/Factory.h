#pragma once

template<typename T, typename U>
concept DerivedType = std::derived_from<T, U>;

template<typename T>
class Factory
{
public:
	template<DerivedType<T> U>
	void registerType(std::string name)
	{
		_factoryMethods.emplace(std::move(name), []() {return std::make_unique<U>(); });
	}

	template<typename... Args>
	std::unique_ptr<T> build(std::string name, Args... args)
	{
		if (!_factoryMethods.contains(name))
		{
			return nullptr;
		}
		return std::move(_factoryMethods.at(name)(args...));
	}

private:
	using FactoryMethod = std::function<std::unique_ptr<T>()>;
	using FactoryMethodMap = std::unordered_map<std::string, FactoryMethod>;
	FactoryMethodMap _factoryMethods;
};
