#pragma once
#include <Utility/stdHashing.h>

template <typename ActionType>
class StateActionMap
{
public:
	void bindAction(const std::string& actionName, ActionType action);
	template<typename... Args>
	void callAction(const std::string& actionName, Args... args);
	bool contains(const std::string& actionName) const;

private:
	std::unordered_map<std::string, ActionType> _actionMap;
};

template<typename ActionType>
inline void StateActionMap<ActionType>::bindAction(const std::string& actionName, ActionType action)
{
	_actionMap.emplace(actionName, action);
}

template<typename ActionType>
inline bool StateActionMap<ActionType>::contains(const std::string& actionName) const
{
	return _actionMap.contains(actionName);
}

template<typename ActionType>
template<typename ...Args>
inline void StateActionMap<ActionType>::callAction(const std::string& actionName, Args ...args)
{
	if (_actionMap.contains(actionName))
	{
		_actionMap.at(actionName)(std::forward<Args>(args)...);
	}
}
