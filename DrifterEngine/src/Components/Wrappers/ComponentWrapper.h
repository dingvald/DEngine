#pragma once
#include <EnTT/entt.hpp>

template<typename ComponentType>
class ComponentWrapper
{
public:
	ComponentWrapper() = default;
	ComponentWrapper(entt::handle entity);

	bool isValid() const;

protected:
	const ComponentType* tryGetUnderlyingConst() const;
	ComponentType* tryGetUnderlying();
	void modify(std::function<void(ComponentType& comp)> func);
	/// <summary>
	/// Attempts to grab the registry
	/// - Successful calls to `isValid` guarentees the registry exists
	/// </summary>
	/// <returns></returns>
	const entt::registry* tryGetRegistryConst() const;
	entt::registry* tryGetRegistry();

	entt::const_handle getConstHandle() const;
	entt::handle getHandle();

private:
	entt::handle _handle;
};

template<typename ComponentType>
inline ComponentWrapper<ComponentType>::ComponentWrapper(entt::handle entity)
	: _handle(entity)
{}

template<typename ComponentType>
inline bool ComponentWrapper<ComponentType>::isValid() const
{
	return _handle && _handle.all_of<ComponentType>();
}


template<typename ComponentType>
inline const ComponentType* ComponentWrapper<ComponentType>::tryGetUnderlyingConst() const
{
	if (!isValid()) return nullptr;
	return _handle.try_get<ComponentType>();
}

template<typename ComponentType>
inline ComponentType* ComponentWrapper<ComponentType>::tryGetUnderlying()
{
	if (!isValid()) return nullptr;
	return _handle.try_get<ComponentType>();
}

template<typename ComponentType>
inline void ComponentWrapper<ComponentType>::modify(std::function<void(ComponentType& comp)> func)
{
	if (!isValid()) return;

	_handle.patch<ComponentType>(func);
}

template<typename ComponentType>
inline const entt::registry* ComponentWrapper<ComponentType>::tryGetRegistryConst() const
{
	if (!isValid()) return nullptr;
	return _handle.registry();
}

template<typename ComponentType>
inline entt::registry* ComponentWrapper<ComponentType>::tryGetRegistry()
{
	if (!isValid()) return nullptr;
	return _handle.registry();
}

template<typename ComponentType>
inline entt::const_handle ComponentWrapper<ComponentType>::getConstHandle() const
{
	return _handle;
}

template<typename ComponentType>
inline entt::handle ComponentWrapper<ComponentType>::getHandle()
{
	return _handle;
}
