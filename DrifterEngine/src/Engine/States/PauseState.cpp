#include "pch.h"
#include "PauseState.h"

drft::PauseState::PauseState(StateStack& stack, Context context)
    :State(stack, context)
{
	addMenuOption("Continue", [this]() {requestStackPop(); });
	addMenuOption("Settings", [this]() {});
	addMenuOption("Quit", [this]() {requestStackClear(); requestStackPush(States::MainMenu); });
}

bool drft::PauseState::handleEvent(const sf::Event& ev)
{
	switch (ev.type)
	{
	case sf::Event::KeyPressed:
		if (ev.key.code == sf::Keyboard::Escape)
		{
			requestStackPop();
			return false;
		}
		if (ev.key.code == sf::Keyboard::Numpad8 || ev.key.code == sf::Keyboard::Up)
		{
			--_cursorIndex;
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

bool drft::PauseState::update(const float dt)
{
	for (auto& option : _menuOptions)
	{
		option.text.setFillColor(sf::Color::White);
	}
	_menuOptions[_cursorIndex].text.setFillColor(sf::Color::Yellow);

	return false;
}

void drft::PauseState::render(sf::RenderTarget& target)
{
	sf::RectangleShape background;
	background.setFillColor(sf::Color(0, 0, 0, 150));
	background.setSize({ static_cast<float>(target.getSize().x), static_cast<float>(target.getSize().y) });

	target.draw(background);

	for (auto& option : _menuOptions)
	{
		target.draw(option.text);
	}
}

void drft::PauseState::addMenuOption(std::string&& name, std::function<void()> callback)
{
	_menuOptions.push_back({ sf::Text{}, callback });
	auto& text = _menuOptions.back().text;
	text.setString(std::move(name));
	text.setPosition({ (static_cast<float>(getContext().window->getView().getSize().x) / 2.f) - 64.f, (_menuOptions.size() * 64.f) + 96.f });
	text.setFont(getContext().fonts->get("Terminus"));
	text.setFillColor(sf::Color::White);
}


