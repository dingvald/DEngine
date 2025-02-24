#pragma once

template <std::invocable ActionType>
class StateActionMap
{
public:
	void bindAction(entt::hashed_string actionName, ActionType action);
	template<typename... Args>
	void callAction(entt::hashed_string actionName, Args... args);

private:
	std::unordered_map<entt::hashed_string, ActionType> _actionMap;
};

template<std::invocable ActionType>
inline void StateActionMap<ActionType>::bindAction(entt::hashed_string actionName, ActionType action)
{
	_actionMap.emplace(actionName, action);
}

template<std::invocable ActionType>
template<typename ...Args>
inline void StateActionMap<ActionType>::callAction(entt::hashed_string actionName, Args ...args)
{
	if (_actionMap.contains(actionName))
	{
		_actionMap.at(actionName)(std::forward<Args>(args)...);
	}
}
