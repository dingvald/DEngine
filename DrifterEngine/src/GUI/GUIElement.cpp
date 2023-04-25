#include "pch.h"
#include "GUIElement.h"

// PANEL

bool drft::gui::Panel::handleEvent(const sf::Event& ev)
{
	Element::handleEvent(ev);

	return false;
}

void drft::gui::Panel::onUpdate(const float dt)
{
}

void drft::gui::Panel::onRender(sf::RenderTarget& target)
{
	target.draw(_shape);
	target.draw(_text);
}

// SINGLE CONTAINER

void drft::gui::SingleContainer::init()
{
}

void drft::gui::SingleContainer::layoutChildren()
{
	_children.front()->setPosition(_shape.getPosition() + _childOrigin);
}

void drft::gui::SingleContainer::onUpdate(const float dt)
{
	for (auto& child : _children)
	{
		child->update(dt);
		break;
	}
}

void drft::gui::SingleContainer::onRender(sf::RenderTarget& target)
{
	target.draw(_shape);
	for (auto& child : _children)
	{
		child->render(target);
		break;
	}
}

// LIST

void drft::gui::List::init()
{
	setStartingCursorPosition();
}

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
			_children[_cursorPosition]->onSelect();
			return false;
		}
		break;
	}

	return false;
}

void drft::gui::List::onUpdate(const float dt)
{
	int count = 0;
	for (auto& child : _children)
	{
		child->update(dt);
		const bool isSelectable = child->isSelectable();
		if (isSelectable)
		{
			child->setState(ElementState::Idle);
		}
		else
		{
			child->setState(ElementState::Unselectable);
		}
		
		if (_cursorPosition == count)
		{
			child->setState(ElementState::Focused);
		}
		++count;
	}
}

void drft::gui::List::onRender(sf::RenderTarget& target)
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
				_cursorPosition = count;
			}
		}
		else
		{
			child->setState(ElementState::Unselectable);
		}

		if (_cursorPosition == count)
		{
			child->setState(ElementState::Focused);
		}
		++count;
	}
}

void drft::gui::List::layoutChildren()
{
	float x = 0;
	float y = _style.at(_state).innerPadding;
	float maxWidth = 0;

	for (auto& child : _children)
	{
		auto rect = child->getGlobalBounds();

		if (y + rect.height > _shape.getSize().y)
		{
			x += maxWidth;
			y = 0;
			maxWidth = 0;
		}

		child->setPosition({ x + _childOrigin.x, y + _childOrigin.y });
		y += rect.height + _style.at(_state).childPadding;
		maxWidth = std::max(maxWidth, rect.width + _style.at(_state).childPadding);
	}
}

void drft::gui::List::moveCursorDown()
{
	int safetyCount = 0;
	do 
	{
		++_cursorPosition;
		if (_cursorPosition >= _children.size())
		{
			_cursorPosition = 0;
		}
		++safetyCount;
	} while (!_children.at(_cursorPosition)->isSelectable() && safetyCount < _children.size());
}

void drft::gui::List::moveCursorUp()
{
	int safetyCount = 0;
	do
	{
		--_cursorPosition;
		if (_cursorPosition < 0)
		{
			_cursorPosition = _children.size() - 1;
		}
		++safetyCount;
	} while (!_children.at(_cursorPosition)->isSelectable() && safetyCount < _children.size());
}

// GRID

drft::gui::Grid::Grid(int columns, int rows)
	: _numColumns(columns)
	, _numRows(rows)
{}

bool drft::gui::Grid::handleEvent(const sf::Event& ev)
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
		if (ev.key.code == sf::Keyboard::Numpad6 || ev.key.code == sf::Keyboard::Right)
		{
			moveCursorRight();
			return false;
		}
		if (ev.key.code == sf::Keyboard::Numpad4 || ev.key.code == sf::Keyboard::Left)
		{
			moveCursorLeft();
			return false;
		}
		if (ev.key.code == sf::Keyboard::Space)
		{
			_children.at(_cursorPosition.x + _numColumns*_cursorPosition.y)->onSelect();
			return false;
		}
		break;
	}

	return false;
}

void drft::gui::Grid::onUpdate(const float dt)
{
	if (!_isInitialized)
	{
		setStartingCursorPosition();
		_isInitialized = true;
	}

	int col = 0;
	int row = 0;

	for (auto& child : _children)
	{
		child->update(dt);

		const bool isSelectable = child->isSelectable();
		if (isSelectable)
		{
			child->setState(ElementState::Idle);
		}
		else
		{
			child->setState(ElementState::Unselectable);
		}

		if (_cursorPosition == sf::Vector2i(col, row))
		{
			child->setState(ElementState::Focused);
			child->onFocus();
		}

		++col;

		if (col >= _numColumns)
		{
			col = 0;
			++row;
		}
	}
}

void drft::gui::Grid::onRender(sf::RenderTarget& target)
{
	target.draw(_shape);
	target.draw(_text);
	for (auto& child : _children)
	{
		child->render(target);
	}
}

void drft::gui::Grid::setStartingCursorPosition()
{
	bool isSelectionFound = false;

	for (int row = 0; row < _numRows; ++row)
	{
		for (int col = 0; col < _numColumns; ++col)
		{
			if ( (col + _numRows * row) >= _children.size()) return;
			const auto& child = _children.at(col + _numRows * row);
			const bool isSelectable = child->isSelectable();
			if (isSelectable)
			{
				child->setState(ElementState::Idle);
				if (!isSelectionFound)
				{
					isSelectionFound = true;
					_cursorPosition = sf::Vector2i(col, row);
				}
			}
			else
			{
				child->setState(ElementState::Unselectable);
			}

			if (_cursorPosition == sf::Vector2i(col, row))
			{
				child->setState(ElementState::Focused);
			}
		}
	}
}

void drft::gui::Grid::autoSize()
{
	float widest_x = 0.0f;
	float tallest_y = 0.0f;

	for (const auto& child : _children)
	{
		widest_x = std::max(child->getGlobalBounds().width, widest_x);
		tallest_y = std::max(child->getGlobalBounds().height, tallest_y);
	}

	float sum_x = 2*_style[_state].innerPadding + ((_numColumns-1) * (_style[_state].childPadding)) + (_numColumns * widest_x);
	float sum_y = 2*_style[_state].innerPadding + ((_numRows-1) * (_style[_state].childPadding)) + (_numRows * tallest_y);

	setSize({ sum_x, sum_y });
	setTextOrigin(_textOrigin);
	setTextPosition(_textPosition);
}

void drft::gui::Grid::layoutChildren()
{
	autoSize();

	int col = 0;
	int row = 0;
	float x = _style.at(_state).innerPadding + _shape.getGlobalBounds().left;
	float y = _style.at(_state).innerPadding + _shape.getGlobalBounds().top;

	float widest_x = 0.0f;
	float tallest_y = 0.0f;

	for (const auto& child : _children)
	{
		widest_x = std::max(child->getGlobalBounds().width, widest_x);
		tallest_y = std::max(child->getGlobalBounds().height, tallest_y);
	}

	for (auto& child : _children)
	{
		child->setPosition({ x, y });

		++col;
		x += widest_x + _style.at(_state).childPadding;

		if (col >= _numColumns)
		{
			col = 0;
			++row;
			x = _style.at(_state).innerPadding + _shape.getGlobalBounds().left;
			y += tallest_y + _style.at(_state).childPadding;
		}
	}
}

void drft::gui::Grid::moveCursorDown()
{
	do
	{
		++_cursorPosition.y;
		if (_cursorPosition.y >= _numRows || (_cursorPosition.x + _numColumns * _cursorPosition.y) >= _children.size())
		{
			_cursorPosition.y = 0;
		}
	} while (!_children.at(_cursorPosition.x + _numColumns*_cursorPosition.y)->isSelectable());
}

void drft::gui::Grid::moveCursorUp()
{
	do
	{
		--_cursorPosition.y;
		if (_cursorPosition.y < 0)
		{
			_cursorPosition.y = _numRows - 1;
			while (_cursorPosition.x + (_cursorPosition.y * _numColumns) > (_children.size()-1))
			{
				--_cursorPosition.y;
			}
		}
	} while (!_children.at(_cursorPosition.x + _numColumns * _cursorPosition.y)->isSelectable());
}

void drft::gui::Grid::moveCursorRight()
{
	do
	{
		++_cursorPosition.x;
		if (_cursorPosition.x >= _numColumns || (_cursorPosition.x + _numColumns * _cursorPosition.y) >= _children.size())
		{
			_cursorPosition.x = 0;
		}
	} while (!_children.at(_cursorPosition.x + _numColumns * _cursorPosition.y)->isSelectable());
}

void drft::gui::Grid::moveCursorLeft()
{
	do
	{
		--_cursorPosition.x;
		if (_cursorPosition.x < 0)
		{
			_cursorPosition.x = _numColumns - 1;
			while (_cursorPosition.x + (_cursorPosition.y * _numColumns) > (_children.size() - 1))
			{
				--_cursorPosition.x;
			}
		}
	} while (!_children.at(_cursorPosition.x + _numColumns * _cursorPosition.y)->isSelectable());
}

// LABEL

bool drft::gui::Label::handleEvent(const sf::Event& ev)
{
	return false;
}

sf::FloatRect drft::gui::Label::getGlobalBounds() const
{
	return _text.getGlobalBounds();
}

sf::FloatRect drft::gui::Label::getLocalBounds() const
{
	return _text.getLocalBounds();
}

void drft::gui::Label::onRender(sf::RenderTarget& target)
{
	target.draw(_text);
}

// BUTTON

void drft::gui::Button::onUpdate(const float dt)
{
}

void drft::gui::Button::onRender(sf::RenderTarget& target)
{
	target.draw(_shape);
	target.draw(_text);
}

// ICON

drft::gui::Icon::Icon(sf::Sprite sprite)
	: _sprite(sprite)
{}

void drft::gui::Icon::init()
{
	const auto spriteBounds = _sprite.getLocalBounds();
	_sprite.setOrigin(spriteBounds.width / 2, spriteBounds.height / 2);
	const auto shapeBounds = _shape.getSize();
	const auto scalingFactorX = shapeBounds.x / spriteBounds.width;
	const auto scalingFactorY = shapeBounds.y / spriteBounds.height;
	_sprite.scale({ scalingFactorX, scalingFactorY });
}

void drft::gui::Icon::onUpdate(const float dt)
{
	_sprite.setColor(_style[_state].fillColor);
	_sprite.setPosition(_shape.getPosition());
	
}

void drft::gui::Icon::onRender(sf::RenderTarget& target)
{
	//target.draw(_shape);
	target.draw(_sprite);
}


