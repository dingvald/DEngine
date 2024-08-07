#pragma once
#include <ProcGen/GenerationLayer/AbstractLayer.h>
#include <ProcGen/GenerationLayer/GenerationState.h>
#include <ProcGen/GenerationLayer/GenerationContext.h>

class GenerationLayerManager
{
public:
	template<typename T>
	GenerationState generate(sf::IntRect area);
	GenerationState generate(entt::id_type id, sf::IntRect area);

	template<typename T>
	void add();

	template<typename T>
	T& get();
	AbstractLayer& get(entt::id_type id);

private:
	using AbstractLayerPtr = std::unique_ptr<AbstractLayer>;
	entt::dense_map<entt::id_type, AbstractLayerPtr> _layers;
};

template<typename T>
inline GenerationState GenerationLayerManager::generate(sf::IntRect area)
{
	entt::id_type id = entt::type_index<T>::value();
	return _layers[id]->generate({ .area = area, .layers = *this });
}

template<typename T>
inline void GenerationLayerManager::add()
{
	entt::id_type id = entt::type_index<T>::value();
	_layers.emplace(id, std::make_unique<T>());
}

template<typename T>
inline T& GenerationLayerManager::get()
{
	entt::id_type id = entt::type_index<T>::value();
	return *static_cast<T*>(_layers[id].get());
}
