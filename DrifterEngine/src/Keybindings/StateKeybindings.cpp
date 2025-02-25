#include "pch.h"
#include "StateKeybindings.h"
#include <Utility/StandardLogger.h>
#include <Utility/StringManipulation.h>


static inline const std::unordered_map<sf::Keyboard::Scancode, std::string> SfmlKeyToString =
{
    {sf::Keyboard::Scancode::Unknown,          "Unknown"},

    // Alphabet keys
    {sf::Keyboard::Scancode::A,               "A"},
    {sf::Keyboard::Scancode::B,               "B"},
    {sf::Keyboard::Scancode::C,               "C"},
    {sf::Keyboard::Scancode::D,               "D"},
    {sf::Keyboard::Scancode::E,               "E"},
    {sf::Keyboard::Scancode::F,               "F"},
    {sf::Keyboard::Scancode::G,               "G"},
    {sf::Keyboard::Scancode::H,               "H"},
    {sf::Keyboard::Scancode::I,               "I"},
    {sf::Keyboard::Scancode::J,               "J"},
    {sf::Keyboard::Scancode::K,               "K"},
    {sf::Keyboard::Scancode::L,               "L"},
    {sf::Keyboard::Scancode::M,               "M"},
    {sf::Keyboard::Scancode::N,               "N"},
    {sf::Keyboard::Scancode::O,               "O"},
    {sf::Keyboard::Scancode::P,               "P"},
    {sf::Keyboard::Scancode::Q,               "Q"},
    {sf::Keyboard::Scancode::R,               "R"},
    {sf::Keyboard::Scancode::S,               "S"},
    {sf::Keyboard::Scancode::T,               "T"},
    {sf::Keyboard::Scancode::U,               "U"},
    {sf::Keyboard::Scancode::V,               "V"},
    {sf::Keyboard::Scancode::W,               "W"},
    {sf::Keyboard::Scancode::X,               "X"},
    {sf::Keyboard::Scancode::Y,               "Y"},
    {sf::Keyboard::Scancode::Z,               "Z"},

    // Number row
    {sf::Keyboard::Scancode::Num0,            "Num0"},
    {sf::Keyboard::Scancode::Num1,            "Num1"},
    {sf::Keyboard::Scancode::Num2,            "Num2"},
    {sf::Keyboard::Scancode::Num3,            "Num3"},
    {sf::Keyboard::Scancode::Num4,            "Num4"},
    {sf::Keyboard::Scancode::Num5,            "Num5"},
    {sf::Keyboard::Scancode::Num6,            "Num6"},
    {sf::Keyboard::Scancode::Num7,            "Num7"},
    {sf::Keyboard::Scancode::Num8,            "Num8"},
    {sf::Keyboard::Scancode::Num9,            "Num9"},

    // Function keys
    {sf::Keyboard::Scancode::F1,              "F1"},
    {sf::Keyboard::Scancode::F2,              "F2"},
    {sf::Keyboard::Scancode::F3,              "F3"},
    {sf::Keyboard::Scancode::F4,              "F4"},
    {sf::Keyboard::Scancode::F5,              "F5"},
    {sf::Keyboard::Scancode::F6,              "F6"},
    {sf::Keyboard::Scancode::F7,              "F7"},
    {sf::Keyboard::Scancode::F8,              "F8"},
    {sf::Keyboard::Scancode::F9,              "F9"},
    {sf::Keyboard::Scancode::F10,             "F10"},
    {sf::Keyboard::Scancode::F11,             "F11"},
    {sf::Keyboard::Scancode::F12,             "F12"},
    {sf::Keyboard::Scancode::F13,             "F13"},
    {sf::Keyboard::Scancode::F14,             "F14"},
    {sf::Keyboard::Scancode::F15,             "F15"},

    // Numpad
    {sf::Keyboard::Scancode::Numpad0,         "Numpad0"},
    {sf::Keyboard::Scancode::Numpad1,         "Numpad1"},
    {sf::Keyboard::Scancode::Numpad2,         "Numpad2"},
    {sf::Keyboard::Scancode::Numpad3,         "Numpad3"},
    {sf::Keyboard::Scancode::Numpad4,         "Numpad4"},
    {sf::Keyboard::Scancode::Numpad5,         "Numpad5"},
    {sf::Keyboard::Scancode::Numpad6,         "Numpad6"},
    {sf::Keyboard::Scancode::Numpad7,         "Numpad7"},
    {sf::Keyboard::Scancode::Numpad8,         "Numpad8"},
    {sf::Keyboard::Scancode::Numpad9,         "Numpad9"},
    {sf::Keyboard::Scancode::NumpadPlus,      "+"},
    {sf::Keyboard::Scancode::NumpadMinus,     "-"},
    {sf::Keyboard::Scancode::NumpadMultiply,  "*"},
    {sf::Keyboard::Scancode::NumpadDivide,    "/"},

    // Navigation
    {sf::Keyboard::Scancode::Up,              "Up"},
    {sf::Keyboard::Scancode::Down,            "Down"},
    {sf::Keyboard::Scancode::Left,            "Left"},
    {sf::Keyboard::Scancode::Right,           "Right"},
    {sf::Keyboard::Scancode::Home,            "Home"},
    {sf::Keyboard::Scancode::End,             "End"},
    {sf::Keyboard::Scancode::PageUp,          "PageUp"},
    {sf::Keyboard::Scancode::PageDown,        "PageDown"},
    {sf::Keyboard::Scancode::Insert,          "Insert"},
    {sf::Keyboard::Scancode::Delete,          "Delete"},

    // Modifiers handled through KeybindingUtils

    // Special
    {sf::Keyboard::Scancode::Space,           "Space"},
    {sf::Keyboard::Scancode::Enter,           "Enter"},
    {sf::Keyboard::Scancode::Escape,          "Escape"},
    {sf::Keyboard::Scancode::Backspace,       "Backspace"},
    {sf::Keyboard::Scancode::Tab,             "Tab"},
    {sf::Keyboard::Scancode::Pause,           "Pause"},

    // Punctuation
    {sf::Keyboard::Scancode::Grave,           "`"},
    {sf::Keyboard::Scancode::Hyphen,          "-"},
    {sf::Keyboard::Scancode::Equal,           "="},
    {sf::Keyboard::Scancode::LBracket,        "["},
    {sf::Keyboard::Scancode::RBracket,        "]"},
    {sf::Keyboard::Scancode::Backslash,       "\\"},
    {sf::Keyboard::Scancode::Semicolon,       ";"},
    {sf::Keyboard::Scancode::Apostrophe,      "'"},
    {sf::Keyboard::Scancode::Comma,           ","},
    {sf::Keyboard::Scancode::Period,          "."},
    {sf::Keyboard::Scancode::Slash,           "/"}
};

static inline const std::unordered_map<std::string, sf::Keyboard::Scancode> StringToSfmlKey =
{
    // Unknown
    {"Unknown", sf::Keyboard::Scancode::Unknown},

    // Alphabet keys
    {"A", sf::Keyboard::Scancode::A},
    {"B", sf::Keyboard::Scancode::B},
    {"C", sf::Keyboard::Scancode::C},
    {"D", sf::Keyboard::Scancode::D},
    {"E", sf::Keyboard::Scancode::E},
    {"F", sf::Keyboard::Scancode::F},
    {"G", sf::Keyboard::Scancode::G},
    {"H", sf::Keyboard::Scancode::H},
    {"I", sf::Keyboard::Scancode::I},
    {"J", sf::Keyboard::Scancode::J},
    {"K", sf::Keyboard::Scancode::K},
    {"L", sf::Keyboard::Scancode::L},
    {"M", sf::Keyboard::Scancode::M},
    {"N", sf::Keyboard::Scancode::N},
    {"O", sf::Keyboard::Scancode::O},
    {"P", sf::Keyboard::Scancode::P},
    {"Q", sf::Keyboard::Scancode::Q},
    {"R", sf::Keyboard::Scancode::R},
    {"S", sf::Keyboard::Scancode::S},
    {"T", sf::Keyboard::Scancode::T},
    {"U", sf::Keyboard::Scancode::U},
    {"V", sf::Keyboard::Scancode::V},
    {"W", sf::Keyboard::Scancode::W},
    {"X", sf::Keyboard::Scancode::X},
    {"Y", sf::Keyboard::Scancode::Y},
    {"Z", sf::Keyboard::Scancode::Z},

    // Number row
    {"Num0", sf::Keyboard::Scancode::Num0},
    {"Num1", sf::Keyboard::Scancode::Num1},
    {"Num2", sf::Keyboard::Scancode::Num2},
    {"Num3", sf::Keyboard::Scancode::Num3},
    {"Num4", sf::Keyboard::Scancode::Num4},
    {"Num5", sf::Keyboard::Scancode::Num5},
    {"Num6", sf::Keyboard::Scancode::Num6},
    {"Num7", sf::Keyboard::Scancode::Num7},
    {"Num8", sf::Keyboard::Scancode::Num8},
    {"Num9", sf::Keyboard::Scancode::Num9},

    // Function keys
    {"F1", sf::Keyboard::Scancode::F1},
    {"F2", sf::Keyboard::Scancode::F2},
    {"F3", sf::Keyboard::Scancode::F3},
    {"F4", sf::Keyboard::Scancode::F4},
    {"F5", sf::Keyboard::Scancode::F5},
    {"F6", sf::Keyboard::Scancode::F6},
    {"F7", sf::Keyboard::Scancode::F7},
    {"F8", sf::Keyboard::Scancode::F8},
    {"F9", sf::Keyboard::Scancode::F9},
    {"F10", sf::Keyboard::Scancode::F10},
    {"F11", sf::Keyboard::Scancode::F11},
    {"F12", sf::Keyboard::Scancode::F12},
    {"F13", sf::Keyboard::Scancode::F13},
    {"F14", sf::Keyboard::Scancode::F14},
    {"F15", sf::Keyboard::Scancode::F15},

    // Numpad
    {"Numpad0", sf::Keyboard::Scancode::Numpad0},
    {"Numpad1", sf::Keyboard::Scancode::Numpad1},
    {"Numpad2", sf::Keyboard::Scancode::Numpad2},
    {"Numpad3", sf::Keyboard::Scancode::Numpad3},
    {"Numpad4", sf::Keyboard::Scancode::Numpad4},
    {"Numpad5", sf::Keyboard::Scancode::Numpad5},
    {"Numpad6", sf::Keyboard::Scancode::Numpad6},
    {"Numpad7", sf::Keyboard::Scancode::Numpad7},
    {"Numpad8", sf::Keyboard::Scancode::Numpad8},
    {"Numpad9", sf::Keyboard::Scancode::Numpad9},
    {"+", sf::Keyboard::Scancode::NumpadPlus},
    {"-", sf::Keyboard::Scancode::NumpadMinus},
    {"*", sf::Keyboard::Scancode::NumpadMultiply},
    {"/", sf::Keyboard::Scancode::NumpadDivide},

    // Navigation
    {"Up", sf::Keyboard::Scancode::Up},
    {"Down", sf::Keyboard::Scancode::Down},
    {"Left", sf::Keyboard::Scancode::Left},
    {"Right", sf::Keyboard::Scancode::Right},
    {"Home", sf::Keyboard::Scancode::Home},
    {"End", sf::Keyboard::Scancode::End},
    {"PageUp", sf::Keyboard::Scancode::PageUp},
    {"PageDown", sf::Keyboard::Scancode::PageDown},
    {"Insert", sf::Keyboard::Scancode::Insert},
    {"Delete", sf::Keyboard::Scancode::Delete},

    // Modifiers handled through KeybindingUtils

    // Special
    {"Space", sf::Keyboard::Scancode::Space},
    {"Enter", sf::Keyboard::Scancode::Enter},
    {"Escape", sf::Keyboard::Scancode::Escape},
    {"Backspace", sf::Keyboard::Scancode::Backspace},
    {"Tab", sf::Keyboard::Scancode::Tab},
    {"Pause", sf::Keyboard::Scancode::Pause},

    // Punctuation
    {"`", sf::Keyboard::Scancode::Grave},
    {"-", sf::Keyboard::Scancode::Hyphen},
    {"=", sf::Keyboard::Scancode::Equal},
    {"[", sf::Keyboard::Scancode::LBracket},
    {"]", sf::Keyboard::Scancode::RBracket},
    {"\\", sf::Keyboard::Scancode::Backslash},
    {";", sf::Keyboard::Scancode::Semicolon},
    {"'", sf::Keyboard::Scancode::Apostrophe},
    {",", sf::Keyboard::Scancode::Comma},
    {".", sf::Keyboard::Scancode::Period},
    {"/", sf::Keyboard::Scancode::Slash}
};

namespace
{
    void logUnknownInputValueError(const std::string& input)
    {
        error_logger << "Error: unknown input value " << input << std::endl;
        error_logger << "--- Please check key_bindings.json" << std::endl;
    }

    void logUnknownModifierValueError(const std::string& input)
    {
        error_logger << "Error: unknown input modifier value " << input << std::endl;
        error_logger << "--- Please check key_bindings.json" << std::endl;
    }

    void logTooManyTokensError(const std::string& input)
    {
        error_logger << "Error: too many tokens in input with value " << input << std::endl;
        error_logger << "--- Please check key_bindings.json" << std::endl;
    }
}

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

        auto key1 = convertStringToKey(keyNames[0].GetString());
        boundKeys.primary = key1;
        _keyToAction.emplace(key1, actionName);
        
        if (keyNames.Size() > 1)
        {
            auto key2 = convertStringToKey(keyNames[1].GetString());
            boundKeys.secondary = key2;
            _keyToAction.emplace(key2, actionName);
        }
        
        if (keyNames.Size() > 2)
        {
            warning_logger << "Warning: Too many keys bound to action " << actionName << std::endl;
            warning_logger << "--- only the first two will be used" << std::endl;
        }
		
		_actionToKeys.emplace(actionName, boundKeys);
        
	}
}

void StateKeybindings::saveToJson(rapidjson::Value& json) const
{
}

void StateKeybindings::bindKeyToAction(ModifiedKey key, entt::hashed_string actionName, BindingPosition position)
{
    if (_keyToAction.contains(key))
    {
        _keyToAction[key] = actionName;
    }

    if (_actionToKeys.contains(actionName))
    {
        auto& boundKeys = _actionToKeys.at(actionName);
        if (position == BindingPosition::Primary)
        {
            if (boundKeys.secondary == key)
            {
                boundKeys.secondary = InvalidKeyBind;
            }
            boundKeys.primary = key;
        }
        else
        {
            if (boundKeys.primary == key)
            {
                boundKeys.primary = InvalidKeyBind;
            }
            boundKeys.secondary = key;
        }
    }  
}

bool StateKeybindings::isKeyBound(ModifiedKey key) const
{
    return _keyToAction.contains(key);
}

std::optional<entt::hashed_string> StateKeybindings::getActionForKey(ModifiedKey key) const
{
    if (!_keyToAction.contains(key))
    {
        return std::nullopt;
    }
    return _keyToAction.at(key);
}

ModifiedKey StateKeybindings::convertStringToKey(const std::string& input) const
{
	auto inputNoWhitespace = drft::util::removeWhitespace(input);
	auto splitStrings = drft::util::split(inputNoWhitespace, "+");

    ModifiedKey result = {};

	if (splitStrings.size() == 1)
	{
        if (StringToSfmlKey.contains(splitStrings[0]))
        {
            result.key = StringToSfmlKey.at(splitStrings[0]);
        }
        else
        {
            logUnknownInputValueError(splitStrings[0]);
            return InvalidKeyBind;
        }
	}
	else if (splitStrings.size() == 2)
	{
        result.modifier = KeybindingUtils::convertStringToKeyModifier(splitStrings[0]);
        if (result.modifier == KeyModifier::None)
        {
            logUnknownModifierValueError(splitStrings[0]);
            return InvalidKeyBind;
        }

        if (StringToSfmlKey.contains(splitStrings[1]))
        {
            result.key = StringToSfmlKey.at(splitStrings[1]);
        }
        else
        {
            logUnknownInputValueError(splitStrings[1]);
            return InvalidKeyBind;
        }
	}
    else
    {
        logTooManyTokensError(input);
        return InvalidKeyBind;
    }

    return result;
}

std::string StateKeybindings::convertKeyToString(ModifiedKey key) const
{
    std::string result;
    switch (key.modifier)
    {
    case KeyModifier::Shift:
        result = "Shift + ";
        break;
    case KeyModifier::Ctrl:
        result = "Ctrl + ";
        break;
    default:
        break;
    }

    result.append(SfmlKeyToString.at(key.key));

    return result;
}
