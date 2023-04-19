#include "pch.h"
#include "MainMenuState.h"

drft::MainMenuState::MainMenuState(StateStack& stack, StateContext& context)
	: State(stack, context)
{
	addMenuOption("Continue", [this]() {requestStackPop(); requestStackPush(States::Game); });
	addMenuOption("New Game", [this]()
		{
			std::filesystem::remove_all(".\\data\\savegame\\");
			requestStackPop(); 
			requestStackPush(States::Game);
		});
	addMenuOption("Settings",	[this]() {});
	addMenuOption("Quit",		[this](){requestStackClear();});

	if (hasSaveFile())
	{
		_cursorIndex = 0;
	}
	else
	{
		_cursorIndex = 1;
	}
}

bool drft::MainMenuState::handleEvent(const sf::Event& ev)
{
	switch (ev.type)
	{
	case sf::Event::KeyPressed:
		if (ev.key.code == sf::Keyboard::Escape)
		{
			requestStackClear();
			return false;
		}
		if (ev.key.code == sf::Keyboard::Numpad8 || ev.key.code == sf::Keyboard::Up)
		{
			--_cursorIndex;
			if (_cursorIndex == 0 && !hasSaveFile())
			{
				--_cursorIndex;
			}
			if (_cursorIndex < 0)
			{
				_cursorIndex = _menuOptions.size() - 1;
			}
			return false;
		}
		if (ev.key.code == sf::Keyboard::Numpad2 || ev.key.code == sf::Keyboard::Down)
		{
			++_cursorIndex;
			if (_cursorIndex >= _menuOptions.size())
			{
				_cursorIndex = 0;
				if (!hasSaveFile())
				{
					++_cursorIndex;
				}
			}
			return false;
		}
		if (ev.key.code == sf::Keyboard::Space)
		{
			_menuOptions[_cursorIndex].callback();
			return false;
		}
		break;
	}

	return false;
}

bool drft::MainMenuState::update(const float dt)
{
	for (auto& option : _menuOptions)
	{
		option.text.setFillColor(sf::Color::White);
	}
	_menuOptions[_cursorIndex].text.setFillColor(sf::Color::Yellow);
	if (!hasSaveFile())
	{
		_menuOptions[0].text.setFillColor(sf::Color(80, 80, 80));
	}

	return false;
}

void drft::MainMenuState::render(sf::RenderTarget& target)
{
	for (auto& option : _menuOptions)
	{
		target.draw(option.text);
	}
}

void drft::MainMenuState::addMenuOption(std::string&& name, std::function<void()> callback)
{
	_menuOptions.push_back({ sf::Text{}, callback });
	auto& text = _menuOptions.back().text;
	text.setString(std::move(name));
	text.setPosition({ (static_cast<float>(getContext().window.getView().getSize().x) / 2.f) - 64.f, (_menuOptions.size() * 64.f) + 64.f });
	text.setFont(getContext().fonts.get("Terminus"));
	text.setFillColor(sf::Color::White);
}

bool drft::MainMenuState::hasSaveFile() const
{
	return std::filesystem::exists(".\\data\\savegame\\");
}
