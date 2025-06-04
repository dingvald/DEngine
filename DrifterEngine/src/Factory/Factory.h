#pragma once

template<typename T, typename U>
concept DerivedType = std::derived_from<T, U>;

template<typename T>
class Factory
{
public:
	template<DerivedType<T> U>
	void registerType(const char* name)
	{
		if constexpr (std::is_constructible_v<U, std::string>)
		{
			_factoryMethods.emplace(std::move(name), [namecopy=name]() {return std::make_unique<U>(namecopy); });
		}
		else
		{
			_factoryMethods.emplace(std::move(name), []() {return std::make_unique<U>(); });
		}
	}

	std::unique_ptr<T> build(std::string name) const
	{
		if (!_factoryMethods.contains(name))
		{
			return nullptr;
		}
		return std::move(_factoryMethods.at(name)());
	}

private:
	using FactoryMethod = std::function<std::unique_ptr<T>()>;
	using FactoryMethodMap = std::unordered_map<std::string, FactoryMethod>;
	FactoryMethodMap _factoryMethods;
};
