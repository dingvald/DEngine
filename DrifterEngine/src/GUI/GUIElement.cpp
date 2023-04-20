#include "pch.h"
#include "GUIElement.h"

// WINDOW

bool drft::gui::Window::handleEvent(const sf::Event& ev)
{
	for (auto& child : _children)
	{
		child->handleEvent(ev);
	}

	return false;
}

bool drft::gui::Window::update(const float dt)
{
	if (needsStyleUpdate())
	{
		applyStyle();
	}
	for (auto& child : _children)
	{
		child->update(dt);
	}
	return false;
}

void drft::gui::Window::render(sf::RenderTarget& target)
{
	target.draw(_shape);
	target.draw(_text);
	for (auto& child : _children)
	{
		child->render(target);
	}
}

// LIST

bool drft::gui::List::handleEvent(const sf::Event& ev)
{
	switch (ev.type)
	{
	case sf::Event::KeyPressed:
		if (ev.key.code == sf::Keyboard::Numpad8 || ev.key.code == sf::Keyboard::Up)
		{
			moveCursorUp();
			return false;
		}
		if (ev.key.code == sf::Keyboard::Numpad2 || ev.key.code == sf::Keyboard::Down)
		{
			moveCursorDown();
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
	if (!_isInitialized)
	{
		setStartingCursorPosition();
		_isInitialized = true;
	}
	if (needsStyleUpdate())
	{
		applyStyle();
	}

	int count = 0;
	for (auto& child : _children)
	{
		const bool isSelectable = child->isSelectable();
		if (isSelectable)
		{
			child->setState(ElementState::Idle);
		}
		else
		{
			child->setState(ElementState::Unselectable);
		}
		
		if (_cursorIndex == count)
		{
			child->setState(ElementState::Focused);
		}
		child->setPosition({ 0, (count * _style[_state].childPadding) + _style[_state].innerPadding});
		++count;
	}
	return false;
}

void drft::gui::List::render(sf::RenderTarget& target)
{
	target.draw(_shape);
	target.draw(_text);
	for (auto& child : _children)
	{
		child->render(target);
	}
}

void drft::gui::List::setStartingCursorPosition()
{
	int count = 0;
	bool isSelectionFound = false;
	for (auto& child : _children)
	{
		const bool isSelectable = child->isSelectable();
		if (isSelectable)
		{
			child->setState(ElementState::Idle);
			if (!isSelectionFound)
			{
				isSelectionFound = true;
				_cursorIndex = count;
			}
		}
		else
		{
			child->setState(ElementState::Unselectable);
		}

		if (_cursorIndex == count)
		{
			child->setState(ElementState::Focused);
		}
		child->setPosition({ 0, (count * _style[_state].childPadding) + _style[_state].innerPadding });
		++count;
	}
}

void drft::gui::List::moveCursorDown()
{
	int safetyCount = 0;
	do 
	{
		++_cursorIndex;
		if (_cursorIndex >= _children.size())
		{
			_cursorIndex = 0;
		}
		++safetyCount;
	} while (!_children.at(_cursorIndex)->isSelectable() && safetyCount < _children.size());
}

void drft::gui::List::moveCursorUp()
{
	int safetyCount = 0;
	do
	{
		--_cursorIndex;
		if (_cursorIndex < 0)
		{
			_cursorIndex = _children.size() - 1;
		}
		++safetyCount;
	} while (!_children.at(_cursorIndex)->isSelectable() && safetyCount < _children.size());
}

// LABEL

bool drft::gui::Label::handleEvent(const sf::Event& ev)
{
	return false;
}

bool drft::gui::Label::update(const float dt)
{
	if (needsStyleUpdate())
	{
		applyStyle();
	}
	return false;
}

void drft::gui::Label::render(sf::RenderTarget& target)
{
	target.draw(_shape);
	target.draw(_text);
	for (auto& child : _children)
	{
		child->render(target);
	}
}

bool drft::gui::Button::handleEvent(const sf::Event& ev)
{
	return false;
}

bool drft::gui::Button::update(const float dt)
{
	if (needsStyleUpdate())
	{
		applyStyle();
	}
	return false;
}

void drft::gui::Button::render(sf::RenderTarget& target)
{
	target.draw(_shape);
	target.draw(_text);
	for (auto& child : _children)
	{
		child->render(target);
	}
}
