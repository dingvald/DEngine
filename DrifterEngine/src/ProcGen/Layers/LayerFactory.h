#pragma once
#include <ProcGen/LayeredProcGen/LayeredProcGen.h>


class LayerFactory
{
public:
	void bind();

	template<drft::DerivedLayer T>
	void registerLayer(entt::id_type id)
	{
		static_assert(std::is_base_of<drft::IGetValueAtLayer, T>::value, "Type must be derived from ICreateFromJson");
		_factories.emplace(std::move(id), []() {return std::make_unique<T>(); });
	}

	std::unique_ptr<drft::IGetValueAtLayer> build(entt::id_type id, const rapidjson::Value& json) const;
	
private:
	using FactoryMethod = std::function<std::unique_ptr<drft::IGetValueAtLayer>()>;
	std::unordered_map<entt::id_type, FactoryMethod> _factories;
};