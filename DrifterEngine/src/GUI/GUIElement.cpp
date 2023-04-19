#include "pch.h"
#include "GUIElement.h"

bool drft::gui::Window::handleEvent(const sf::Event& ev)
{
	for (auto& child : _children)
	{
		child->handleEvent(ev);
	}
}

bool drft::gui::Window::update(const float dt)
{
	return false;
}

void drft::gui::Window::render(sf::RenderTarget& target)
{
	target.draw(_shape);
	for (auto& child : _children)
	{
		child->render(target);
	}
}

bool drft::gui::List::handleEvent(const sf::Event& ev)
{
	switch (ev.type)
	{
	case sf::Event::KeyPressed:
		if (ev.key.code == sf::Keyboard::Numpad8 || ev.key.code == sf::Keyboard::Up)
		{
			_children[_cursorIndex]->onLeave();
			--_cursorIndex;
			if (_cursorIndex < 0)
			{
				_cursorIndex = _children.size() - 1;
			}
			_children[_cursorIndex]->onFocus();
			return false;
		}
		if (ev.key.code == sf::Keyboard::Numpad2 || ev.key.code == sf::Keyboard::Down)
		{
			_children[_cursorIndex]->onLeave();
			++_cursorIndex;
			if (_cursorIndex >= _children.size())
			{
				_cursorIndex = 0;
			}
			_children[_cursorIndex]->onFocus();
			return false;
		}
		if (ev.key.code == sf::Keyboard::Space)
		{
			_children[_cursorIndex]->onSelect();
			return false;
		}
		break;
	}

	return false;
}

bool drft::gui::List::update(const float dt)
{
	return false;
}

void drft::gui::List::render(sf::RenderTarget& target)
{
	target.draw(_shape);
	for (auto& child : _children)
	{
		child->render(target);
	}
}
