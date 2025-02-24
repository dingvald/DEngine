#include "pch.h"
#include "StateKeybindings.h"
#include <Utility/StandardLogger.h>
#include <Utility/StringManipulation.h>


static inline const std::unordered_map<sf::Keyboard::Key, std::string> SfmlKeyToString =
{
    {sf::Keyboard::Unknown,          "Unknown"},

    // Alphabet keys
    {sf::Keyboard::A,               "A"},
    {sf::Keyboard::B,               "B"},
    {sf::Keyboard::C,               "C"},
    {sf::Keyboard::D,               "D"},
    {sf::Keyboard::E,               "E"},
    {sf::Keyboard::F,               "F"},
    {sf::Keyboard::G,               "G"},
    {sf::Keyboard::H,               "H"},
    {sf::Keyboard::I,               "I"},
    {sf::Keyboard::J,               "J"},
    {sf::Keyboard::K,               "K"},
    {sf::Keyboard::L,               "L"},
    {sf::Keyboard::M,               "M"},
    {sf::Keyboard::N,               "N"},
    {sf::Keyboard::O,               "O"},
    {sf::Keyboard::P,               "P"},
    {sf::Keyboard::Q,               "Q"},
    {sf::Keyboard::R,               "R"},
    {sf::Keyboard::S,               "S"},
    {sf::Keyboard::T,               "T"},
    {sf::Keyboard::U,               "U"},
    {sf::Keyboard::V,               "V"},
    {sf::Keyboard::W,               "W"},
    {sf::Keyboard::X,               "X"},
    {sf::Keyboard::Y,               "Y"},
    {sf::Keyboard::Z,               "Z"},

    // Number row
    {sf::Keyboard::Num0,            "Num0"},
    {sf::Keyboard::Num1,            "Num1"},
    {sf::Keyboard::Num2,            "Num2"},
    {sf::Keyboard::Num3,            "Num3"},
    {sf::Keyboard::Num4,            "Num4"},
    {sf::Keyboard::Num5,            "Num5"},
    {sf::Keyboard::Num6,            "Num6"},
    {sf::Keyboard::Num7,            "Num7"},
    {sf::Keyboard::Num8,            "Num8"},
    {sf::Keyboard::Num9,            "Num9"},

    // Function keys
    {sf::Keyboard::F1,              "F1"},
    {sf::Keyboard::F2,              "F2"},
    {sf::Keyboard::F3,              "F3"},
    {sf::Keyboard::F4,              "F4"},
    {sf::Keyboard::F5,              "F5"},
    {sf::Keyboard::F6,              "F6"},
    {sf::Keyboard::F7,              "F7"},
    {sf::Keyboard::F8,              "F8"},
    {sf::Keyboard::F9,              "F9"},
    {sf::Keyboard::F10,             "F10"},
    {sf::Keyboard::F11,             "F11"},
    {sf::Keyboard::F12,             "F12"},
    {sf::Keyboard::F13,             "F13"},
    {sf::Keyboard::F14,             "F14"},
    {sf::Keyboard::F15,             "F15"},

    // Numpad
    {sf::Keyboard::Numpad0,         "Numpad0"},
    {sf::Keyboard::Numpad1,         "Numpad1"},
    {sf::Keyboard::Numpad2,         "Numpad2"},
    {sf::Keyboard::Numpad3,         "Numpad3"},
    {sf::Keyboard::Numpad4,         "Numpad4"},
    {sf::Keyboard::Numpad5,         "Numpad5"},
    {sf::Keyboard::Numpad6,         "Numpad6"},
    {sf::Keyboard::Numpad7,         "Numpad7"},
    {sf::Keyboard::Numpad8,         "Numpad8"},
    {sf::Keyboard::Numpad9,         "Numpad9"},
    {sf::Keyboard::Add,             "+"},
    {sf::Keyboard::Subtract,        "-"},
    {sf::Keyboard::Multiply,        "*"},
    {sf::Keyboard::Divide,          "/"},

    // Navigation
    {sf::Keyboard::Up,              "Up"},
    {sf::Keyboard::Down,            "Down"},
    {sf::Keyboard::Left,            "Left"},
    {sf::Keyboard::Right,           "Right"},
    {sf::Keyboard::Home,            "Home"},
    {sf::Keyboard::End,             "End"},
    {sf::Keyboard::PageUp,          "PageUp"},
    {sf::Keyboard::PageDown,        "PageDown"},
    {sf::Keyboard::Insert,          "Insert"},
    {sf::Keyboard::Delete,          "Delete"},

    // Modifiers
    {sf::Keyboard::LControl,        "LControl"},
    {sf::Keyboard::RControl,        "RControl"},
    {sf::Keyboard::LShift,          "LShift"},
    {sf::Keyboard::RShift,          "RShift"},
    {sf::Keyboard::LAlt,            "LAlt"},
    {sf::Keyboard::RAlt,            "RAlt"},
    {sf::Keyboard::LSystem,         "LSystem"},
    {sf::Keyboard::RSystem,         "RSystem"},
    {sf::Keyboard::Menu,            "Menu"},

    // Special
    {sf::Keyboard::Space,           "Space"},
    {sf::Keyboard::Enter,           "Enter"},
    {sf::Keyboard::Escape,          "Escape"},
    {sf::Keyboard::Backspace,       "Backspace"},
    {sf::Keyboard::Tab,             "Tab"},
    {sf::Keyboard::Pause,           "Pause"},

    // Punctuation
    {sf::Keyboard::Grave,           "`"},
    {sf::Keyboard::Hyphen,          "-"},
    {sf::Keyboard::Equal,           "="},
    {sf::Keyboard::LBracket,        "["},
    {sf::Keyboard::RBracket,        "]"},
    {sf::Keyboard::Backslash,       "\\"},
    {sf::Keyboard::Semicolon,       ";"},
    {sf::Keyboard::Apostrophe,      "'"},
    {sf::Keyboard::Comma,           ","},
    {sf::Keyboard::Period,          "."},
    {sf::Keyboard::Slash,           "/"},
    {sf::Keyboard::Tilde,           "~"},
};

static inline const std::unordered_map<std::string, sf::Keyboard::Key> StringToSfmlKey =
{
    // Unknown
    {"Unknown", sf::Keyboard::Unknown},

    // Alphabet keys
    {"A", sf::Keyboard::A},
    {"B", sf::Keyboard::B},
    {"C", sf::Keyboard::C},
    {"D", sf::Keyboard::D},
    {"E", sf::Keyboard::E},
    {"F", sf::Keyboard::F},
    {"G", sf::Keyboard::G},
    {"H", sf::Keyboard::H},
    {"I", sf::Keyboard::I},
    {"J", sf::Keyboard::J},
    {"K", sf::Keyboard::K},
    {"L", sf::Keyboard::L},
    {"M", sf::Keyboard::M},
    {"N", sf::Keyboard::N},
    {"O", sf::Keyboard::O},
    {"P", sf::Keyboard::P},
    {"Q", sf::Keyboard::Q},
    {"R", sf::Keyboard::R},
    {"S", sf::Keyboard::S},
    {"T", sf::Keyboard::T},
    {"U", sf::Keyboard::U},
    {"V", sf::Keyboard::V},
    {"W", sf::Keyboard::W},
    {"X", sf::Keyboard::X},
    {"Y", sf::Keyboard::Y},
    {"Z", sf::Keyboard::Z},

    // Number row
    {"Num0", sf::Keyboard::Num0},
    {"Num1", sf::Keyboard::Num1},
    {"Num2", sf::Keyboard::Num2},
    {"Num3", sf::Keyboard::Num3},
    {"Num4", sf::Keyboard::Num4},
    {"Num5", sf::Keyboard::Num5},
    {"Num6", sf::Keyboard::Num6},
    {"Num7", sf::Keyboard::Num7},
    {"Num8", sf::Keyboard::Num8},
    {"Num9", sf::Keyboard::Num9},

    // Function keys
    {"F1", sf::Keyboard::F1},
    {"F2", sf::Keyboard::F2},
    {"F3", sf::Keyboard::F3},
    {"F4", sf::Keyboard::F4},
    {"F5", sf::Keyboard::F5},
    {"F6", sf::Keyboard::F6},
    {"F7", sf::Keyboard::F7},
    {"F8", sf::Keyboard::F8},
    {"F9", sf::Keyboard::F9},
    {"F10", sf::Keyboard::F10},
    {"F11", sf::Keyboard::F11},
    {"F12", sf::Keyboard::F12},
    {"F13", sf::Keyboard::F13},
    {"F14", sf::Keyboard::F14},
    {"F15", sf::Keyboard::F15},

    // Numpad
    {"Numpad0", sf::Keyboard::Numpad0},
    {"Numpad1", sf::Keyboard::Numpad1},
    {"Numpad2", sf::Keyboard::Numpad2},
    {"Numpad3", sf::Keyboard::Numpad3},
    {"Numpad4", sf::Keyboard::Numpad4},
    {"Numpad5", sf::Keyboard::Numpad5},
    {"Numpad6", sf::Keyboard::Numpad6},
    {"Numpad7", sf::Keyboard::Numpad7},
    {"Numpad8", sf::Keyboard::Numpad8},
    {"Numpad9", sf::Keyboard::Numpad9},
    {"+", sf::Keyboard::Add},
    {"-", sf::Keyboard::Subtract},
    {"*", sf::Keyboard::Multiply},
    {"/", sf::Keyboard::Divide},

    // Navigation
    {"Up", sf::Keyboard::Up},
    {"Down", sf::Keyboard::Down},
    {"Left", sf::Keyboard::Left},
    {"Right", sf::Keyboard::Right},
    {"Home", sf::Keyboard::Home},
    {"End", sf::Keyboard::End},
    {"PageUp", sf::Keyboard::PageUp},
    {"PageDown", sf::Keyboard::PageDown},
    {"Insert", sf::Keyboard::Insert},
    {"Delete", sf::Keyboard::Delete},

    // Modifiers
    {"LControl", sf::Keyboard::LControl},
    {"RControl", sf::Keyboard::RControl},
    {"LShift", sf::Keyboard::LShift},
    {"RShift", sf::Keyboard::RShift},
    {"LAlt", sf::Keyboard::LAlt},
    {"RAlt", sf::Keyboard::RAlt},
    {"LSystem", sf::Keyboard::LSystem},
    {"RSystem", sf::Keyboard::RSystem},
    {"Menu", sf::Keyboard::Menu},

    // Special
    {"Space", sf::Keyboard::Space},
    {"Enter", sf::Keyboard::Enter},
    {"Escape", sf::Keyboard::Escape},
    {"Backspace", sf::Keyboard::Backspace},
    {"Tab", sf::Keyboard::Tab},
    {"Pause", sf::Keyboard::Pause},

    // Punctuation
    {"`", sf::Keyboard::Grave},
    {"-", sf::Keyboard::Hyphen},
    {"=", sf::Keyboard::Equal},
    {"[", sf::Keyboard::LBracket},
    {"]", sf::Keyboard::RBracket},
    {"\\", sf::Keyboard::Backslash},
    {";", sf::Keyboard::Semicolon},
    {"'", sf::Keyboard::Apostrophe},
    {",", sf::Keyboard::Comma},
    {".", sf::Keyboard::Period},
    {"/", sf::Keyboard::Slash},
    {"~", sf::Keyboard::Tilde}
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

        auto key1 = parseStringToKey(keyNames[0].GetString());
        boundKeys.primary = key1;
        _keyToAction.emplace(key1, actionName);
        
        if (keyNames.Size() > 1)
        {
            auto key2 = parseStringToKey(keyNames[1].GetString());
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

void StateKeybindings::bindKeyToAction(sf::Keyboard::Key key, KeyModifier modifier, std::string_view actionName, BindingPosition position)
{
    StateKeybindings::Key newKey = { modifier, key };

    if (_keyToAction.contains(newKey))
    {
        _keyToAction[{modifier, key}] = actionName;
    }

    if (_actionToKeys.contains(actionName))
    {
        auto& boundKeys = _actionToKeys.at(actionName);
        if (position == BindingPosition::Primary)
        {
            if (boundKeys.secondary == newKey)
            {
                boundKeys.secondary = InvalidKeyBind;
            }
            boundKeys.primary = newKey;
        }
        else
        {
            if (boundKeys.primary == newKey)
            {
                boundKeys.primary = InvalidKeyBind;
            }
            boundKeys.secondary = newKey;
        }
    }  
}

bool StateKeybindings::isKeyBound(sf::Keyboard::Key key, KeyModifier modifier) const
{
    return _keyToAction.contains(StateKeybindings::Key{ modifier, key });
}

std::optional<std::string_view> StateKeybindings::getActionForKey(sf::Keyboard::Key key, KeyModifier modifier)
{
    if (!_keyToAction.contains({ modifier, key }))
    {
        return std::nullopt;
    }
    return _keyToAction.at({ modifier, key });
}

StateKeybindings::Key StateKeybindings::parseStringToKey(const std::string& input)
{
	auto inputNoWhitespace = drft::util::removeWhitespace(input);
	auto splitStrings = drft::util::split(inputNoWhitespace, "+");

    StateKeybindings::Key result = {};

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
        if (StringToSfmlKey.contains(splitStrings[0]))
        {
            auto mod = StringToSfmlKey.at(splitStrings[0]);
            if (mod == sf::Keyboard::Key::LShift || mod == sf::Keyboard::Key::RShift)
            {
                result.modifier = KeyModifier::Shift;
            }
            else if (mod == sf::Keyboard::Key::LControl || mod == sf::Keyboard::Key::RControl)
            {
                result.modifier = KeyModifier::Ctrl;
            }
            else
            {
                logUnknownModifierValueError(splitStrings[0]);
                return InvalidKeyBind;
            }
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
