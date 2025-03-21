#pragma once
#include <EnTT/entt.hpp>

template<typename ComponentType>
class ComponentWrapper
{
public:
	void set(entt::handle entity);
	bool isValid() const;

protected:
	ComponentType* _component;
	entt::registry* _registry;
};

template<typename ComponentType>
inline void ComponentWrapper<ComponentType>::set(entt::handle entity)
{
	if (auto comp = entity.try_get<ComponentType>())
	{
		_component = comp;
		_registry = entity.registry();
	}
}

template<typename ComponentType>
inline bool ComponentWrapper<ComponentType>::isValid() const
{
	return _component != nullptr && _registry != nullptr;
}
