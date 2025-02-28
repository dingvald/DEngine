#include "pch.h"
#include "StateKeybindings.h"
#include <Utility/StandardLogger.h>
#include <Utility/StringManipulation.h>


void StateKeybindings::createFromJson(const rapidjson::Value& json)
{
	for (auto&& binding : json.GetObject())
	{
		auto actionName = binding.name.GetString();
		auto keyNames = binding.value.GetArray();
		BoundKeys boundKeys;
		
        if (keyNames.Empty())
        {
            error_logger << "Error: No keys bound to action " << actionName << std::endl;
            error_logger << "--- Please check key_bindings.json" << std::endl;
            continue;
        }

        auto key1 = KeybindingUtils::convertStringToModifiedKey(keyNames[0].GetString());
        boundKeys.primary = key1;
        _keyToAction.emplace(key1, actionName);
        
        if (keyNames.Size() > 1)
        {
            auto key2 = KeybindingUtils::convertStringToModifiedKey(keyNames[1].GetString());
            boundKeys.secondary = key2;
            _keyToAction.emplace(key2, actionName);
        }
        
        if (keyNames.Size() > 2)
        {
            warning_logger << "Warning: Too many keys bound to action " << actionName << std::endl;
            warning_logger << "--- only the first two will be used" << std::endl;
        }
		
        std::cout << "adding action: " << actionName << std::endl;
		_actionToKeys.emplace(actionName, boundKeys);
        _actionKeyPairs.emplace_back(actionName, boundKeys);
	}
}

void StateKeybindings::saveToJson(rapidjson::Value& json) const
{
}

void StateKeybindings::bindKeyToAction(ModifiedKey key, const std::string& actionName, BindingPosition position)
{
    if (_actionToKeys.contains(actionName))
    {
        auto& boundKeys = _actionToKeys.at(actionName);
        auto isMatch = [&actionName](const ActionKeyPair& pair) -> bool {return pair.actionName == actionName;};
        auto keyPair = std::find_if(_actionKeyPairs.begin(), _actionKeyPairs.end(), isMatch);

        if (position == BindingPosition::Primary)
        {
            
            if (boundKeys.secondary == key)
            {
                boundKeys.secondary = ModifiedKey{};
                keyPair->boundKeys.secondary = ModifiedKey{};
            }
            _keyToAction.erase(boundKeys.primary);
            boundKeys.primary = key;
            keyPair->boundKeys.primary = key;
        }
        else
        {
            if (boundKeys.primary == key)
            {
                boundKeys.primary = ModifiedKey{};
                keyPair->boundKeys.primary = ModifiedKey{};
            }
            _keyToAction.erase(boundKeys.secondary);
            boundKeys.secondary = key;
            keyPair->boundKeys.secondary = key;
        }

        _keyToAction[key] = actionName;
    }
    else
    {
        error_logger << "Error: Could not bind key to action " << actionName << std::endl;
    }
}

void StateKeybindings::unbindKeyFromAction(ModifiedKey key, const std::string& actionName)
{
    if (_actionToKeys.contains(actionName))
    {
        auto isMatch = [&actionName](const ActionKeyPair& pair) -> bool {return pair.actionName == actionName;};
        auto keyPair = std::find_if(_actionKeyPairs.begin(), _actionKeyPairs.end(), isMatch);

        auto& boundKeys = _actionToKeys.at(actionName);
        if (boundKeys.primary == key)
        {
            boundKeys.primary = InvalidKey;
            keyPair->boundKeys.primary = InvalidKey;
        }
        else if (boundKeys.secondary == key)
        {
            boundKeys.secondary = InvalidKey;
            keyPair->boundKeys.secondary = InvalidKey;
        }
    }
}

bool StateKeybindings::isKeyBound(ModifiedKey key) const
{
    return _keyToAction.contains(key);
}

std::optional<std::string> StateKeybindings::getActionForKey(ModifiedKey key) const
{
    if (!_keyToAction.contains(key))
    {
        return std::nullopt;
    }
    return _keyToAction.at(key);
}

std::vector<StateKeybindings::ActionKeyPair>& StateKeybindings::getActionKeyPairs()
{
    return _actionKeyPairs;
}
