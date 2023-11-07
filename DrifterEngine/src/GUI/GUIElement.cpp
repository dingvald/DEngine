#include "pch.h"
#include "GUIElement.h"
#include "Utility/Math.h"

// FlOWCONTROL

const drft::gui::Element& drft::gui::FlowControl::getActive() const
{
	if (_children.empty()) return *this;
	return *_children.at(_inControlIndex);
}

void drft::gui::FlowControl::transferControlTo(std::string&& childname)
{
	_inControlIndex = _childrenMap.at(childname);
	for (auto& child : _children)
	{
		child->setState(gui::ElementState::Idle);
	}
	_children.at(_inControlIndex)->setState(gui::ElementState::Focused);
}

void drft::gui::FlowControl::cycleControl()
{
	int originalIndex = _inControlIndex;
	do
	{
		_inControlIndex = math::wrap(++_inControlIndex, 0, _children.size() - 1);
	} while ( (_children.at(_inControlIndex)->isContainer() && _children.at(_inControlIndex)->isEmpty()) 
		&& _inControlIndex != originalIndex);

	if (_inControlIndex == originalIndex) return;

	for (auto& child : _children)
	{
		child->setState(gui::ElementState::Idle);
	}
	_children.at(_inControlIndex)->setState(gui::ElementState::Focused);
}

void drft::gui::FlowControl::layoutChildren()
{
	// Does not handle layout
}

bool drft::gui::FlowControl::onHandleEvent(const sf::Event& ev)
{
	bool propagate = true;
	if (_inControlIndex < _children.size())
	{
		propagate = _children.at(_inControlIndex)->handleEvent(ev);
	}
	else
	{
		for (auto& child : _children)
		{
			child->handleEvent(ev);
		}
	}
	return propagate;
}

bool drft::gui::FlowControl::onUpdate(const float dt)
{
	for (auto& child : _children)
	{
		child->update(dt);
	}

	return true;
}

void drft::gui::FlowControl::onRender(sf::RenderTarget& target)
{
	for (auto& child : _children)
	{
		child->render(target);
	}
}

// BLOB

void drft::gui::Blob::layoutChildren()
{
	for (auto& child : _children)
	{
		child->setPosition(getPosition() + child->getPosition() + _childOrigin + _childOffset);
	}
}

bool drft::gui::Blob::onHandleEvent(const sf::Event& ev)
{
	for (auto& child : _children)
	{
		child->handleEvent(ev);
	}
	return true;
}

bool drft::gui::Blob::onUpdate(const float dt)
{
	layoutChildren();

	for (auto& child : _children)
	{
		child->update(dt);
	}

	return true;
}

void drft::gui::Blob::onRender(sf::RenderTarget& target)
{
	for (auto& child : _children)
	{
		child->render(target);
	}
}

// STACK

bool drft::gui::Stack::onHandleEvent(const sf::Event& ev)
{
	bool propagate = true;
	for (auto child = _children.rbegin(); child != _children.rend(); ++child)
	{
		if (!(*child)->handleEvent(ev))
		{
			propagate = false;
			break;
		}
	}
	switch (ev.type)
	{
	case sf::Event::KeyPressed:
		if (ev.key.code == sf::Keyboard::Escape)
		{
			if (_children.empty()) return true;
			_children.back()->setState(gui::ElementState::Idle);
			popBack();
			return false;
		}
		break;
	}

	return propagate;
}

void drft::gui::Stack::layoutChildren()
{
	for (auto& child : _children)
	{
		child->setPosition(child->getPosition() + _shape.getPosition() + _childOrigin);
	}
}

bool drft::gui::Stack::onUpdate(const float dt)
{
	if (!_children.empty())
	{
		_children.back()->setState(gui::ElementState::Focused);
	}
	for (auto child = _children.rbegin(); child != _children.rend(); ++child)
	{
		if (!(*child)->update(dt)) break;
	}
	return true;
}

void drft::gui::Stack::onRender(sf::RenderTarget& target)
{
	target.draw(_shape);
	target.draw(_text);
	for (auto& child : _children)
	{
		child->render(target);
	}
}

// SINGLE CONTAINER

bool drft::gui::SingleContainer::onHandleEvent(const sf::Event& ev)
{
	for (auto& child : _children)
	{
		child->handleEvent(ev);
	}

	return true;
}

void drft::gui::SingleContainer::layoutChildren()
{
	if (_children.empty()) return;
	_children.front()->setPosition(_shape.getPosition() + _childOrigin);
}

bool drft::gui::SingleContainer::onUpdate(const float dt)
{
	layoutChildren();
	for (auto& child : _children)
	{
		child->update(dt);
		break;
	}
	return true;
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

// DUAL CONTAINER

void drft::gui::DualContainer::layoutChildren()
{
	if (_children.empty()) return;
	_children.front()->setPosition(_shape.getPosition() + _children.front()->getLocalPosition() + _childOrigin);
	_children.back()->setPosition(_shape.getPosition() + _children.back()->getLocalPosition() + _childOrigin + _style[_state].childPadding);
}

bool drft::gui::DualContainer::onHandleEvent(const sf::Event& ev)
{
	for (auto& child : _children)
	{
		child->handleEvent(ev);
	}

	return true;
}

bool drft::gui::DualContainer::onUpdate(const float dt)
{
	layoutChildren();
	for (auto& child : _children)
	{
		child->setState(_state);
		child->update(dt);
	}
	return true;
}

void drft::gui::DualContainer::onRender(sf::RenderTarget& target)
{
	target.draw(_shape);
	for (auto& child : _children)
	{
		child->render(target);
	}
}

void drft::gui::DualContainer::onSelect()
{
	Element::onSelect();
	for (auto& child : _children)
	{
		child->setState(ElementState::Active);
	}
}

void drft::gui::DualContainer::onFocus()
{
	Element::onFocus();
	for (auto& child : _children)
	{
		child->setState(ElementState::Focused);
	}
}

void drft::gui::DualContainer::onLeave()
{
	Element::onLeave();

	for (auto& child : _children)
	{
		child->setState(ElementState::Idle);
	}
}

// MULTI CONTAINER

void drft::gui::MultiContainer::layoutChildren()
{
	if (_children.empty()) return;

	int count = 0;
	for (auto& child : _children)
	{
		child->setPosition(_shape.getPosition() + _childOrigin + (static_cast<float>(count) * _style[_state].childPadding));
		++count;
	}
}

// LIST

drft::gui::List::List(bool canInteract)
	: _canInteract(canInteract)
{}

void drft::gui::List::onFocus()
{
	Element::onFocus();
	setStartingCursorPosition();
}

void drft::gui::List::onLeave()
{
	_cursorPosition = -1;
}

bool drft::gui::List::onHandleEvent(const sf::Event& ev)
{
	if (!_canInteract) return false;
	if (getState() != ElementState::Focused) return false;
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
			if (_cursorPosition >= 0 && _cursorPosition < _children.size())
			{
				if (!_children.empty() && _children[_cursorPosition]->isSelectable())
				{
					_children[_cursorPosition]->setState(gui::ElementState::Active);
					return false;
				}
			}
		}
		break;
	}

	return true;
}

bool drft::gui::List::onUpdate(const float dt)
{
	layoutChildren();
	int count = 0;
	for (auto& child : _children)
	{
		if (_canInteract)
		{
			const bool isSelectable = child->isSelectable();
			if (isSelectable)
			{
				if (_cursorPosition == count)
				{
					child->setState(ElementState::Focused);
				}
				else
				{
					child->setState(ElementState::Idle);
				}
			}
			else
			{
				if (_cursorPosition == count)
				{
					child->setState(ElementState::FocusedUnselectable);
				}
				else
				{
					child->setState(ElementState::Unselectable);
				}
			}
			++count;
		}
		
		child->update(dt);
	}

	return true;
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

void drft::gui::List::autoSize()
{
	float widest_x = 0.0f;
	float tallest_y = 0.0f;

	for (const auto& child : _children)
	{
		widest_x = std::max(child->getGlobalBounds().width, widest_x);
		tallest_y = std::max(child->getGlobalBounds().height, tallest_y);
	}

	float sum_x = widest_x + 2 * _style[_state].innerPadding.x;
	float sum_y = (tallest_y + _style[_state].childPadding.y) * (_children.size()) + 2* _style[_state].innerPadding.y;

	setSize({ sum_x, sum_y });
	setTextOrigin(_textOrigin);
	setTextPosition(_textPosition);
	setChildrenOrigin(_childAlignment, _childOffset);
}

void drft::gui::List::setStartingCursorPosition()
{
	if (!_canInteract) return;
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
	autoSize();

	float x = _style.at(_state).innerPadding.x;
	float y = _style.at(_state).innerPadding.y;
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
		child->layoutChildren();
		y += rect.height + _style.at(_state).childPadding.y;
		maxWidth = std::max(maxWidth, rect.width + _style.at(_state).childPadding.y);
	}
}

void drft::gui::List::moveCursorDown()
{
	++_cursorPosition;
	if (_cursorPosition >= _children.size())
	{
		_cursorPosition = 0;
	}
}

void drft::gui::List::moveCursorUp()
{
	--_cursorPosition;
	if (_cursorPosition < 0)
	{
		_cursorPosition = _children.size() - 1;
	}
}

// GRID

drft::gui::Grid::Grid(int columns, int rows)
	: _numColumns(columns)
	, _numRows(rows)
{}

void drft::gui::Grid::onFocus()
{
	Element::onFocus();
	setStartingCursorPosition();
}

void drft::gui::Grid::onLeave()
{
	_cursorPosition = { -1,-1 };
}

bool drft::gui::Grid::onHandleEvent(const sf::Event& ev)
{
	if (getState() != ElementState::Focused) return true;
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
			_children.at(_cursorPosition.x + _numColumns*_cursorPosition.y)->setState(gui::ElementState::Active);
			return false;
		}
		break;
	}

	return true;
}

bool drft::gui::Grid::onUpdate(const float dt)
{
	layoutChildren();
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
		}

		++col;

		if (col >= _numColumns)
		{
			col = 0;
			++row;
		}
	}

	return true;
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
	_cursorPosition = { 0,0 };
	for (int row = 0; row < _numRows; ++row)
	{
		for (int col = 0; col < _numColumns; ++col)
		{
			if ((col + _numColumns * row) >= _children.size()) return;
			const auto& child = _children.at(col + _numColumns * row);
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

	int numColumnsOfChildren = std::min(static_cast<int>(_children.size()), _numColumns);
	int numRowOfChildren = _children.size() / _numColumns;

	const sf::Vector2f childPadding = _style[_state].childPadding;

	float sum_x = 0.f;
	float sum_y = 0.f;

	int count = 0;

	for (const auto& child : _children)
	{
		if (count < numColumnsOfChildren)
		{
			sum_x += widest_x;
			if (count < numColumnsOfChildren - 1)
			{
				sum_x += childPadding.x;
			}
		}

		if (count % numColumnsOfChildren == 0)
		{
			sum_y += tallest_y;
			if ((count / _numColumns) < numRowOfChildren - 1)
			{
				sum_y += childPadding.y;
			}
		}
		++count;
	}

	sum_x += 2 * _style[_state].innerPadding.x;
	sum_y += 2 * _style[_state].innerPadding.y;

	setSize({ sum_x, sum_y});
	setTextOrigin(_textOrigin);
	setTextPosition(_textPosition);
}

void drft::gui::Grid::layoutChildren()
{
	autoSize();

	int col = 0;
	int row = 0;
	float x = _style.at(_state).innerPadding.x + _shape.getGlobalBounds().left + _style.at(_state).outlineThickness;
	float y = _style.at(_state).innerPadding.y + _shape.getGlobalBounds().top + _style.at(_state).outlineThickness;

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
		x += widest_x + _style.at(_state).childPadding.x;

		if (col >= _numColumns)
		{
			col = 0;
			++row;
			x = _style.at(_state).innerPadding.x + _shape.getGlobalBounds().left + _style.at(_state).outlineThickness;
			y += tallest_y + _style.at(_state).childPadding.y;
		}
	}
}

void drft::gui::Grid::moveCursorDown()
{
	++_cursorPosition.y;
	if (_cursorPosition.y >= _numRows 
		|| (_cursorPosition.x + _numColumns * _cursorPosition.y) >= _children.size())
	{
		_cursorPosition.y = 0;
		while (_cursorPosition.x + (_cursorPosition.y * _numColumns) < 0)
		{
			++_cursorPosition.y;
		}
	}

}

void drft::gui::Grid::moveCursorUp()
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
}

void drft::gui::Grid::moveCursorRight()
{
	++_cursorPosition.x;
	if (_cursorPosition.x >= _numColumns 
		|| (_cursorPosition.x + _numColumns * _cursorPosition.y) >= _children.size())
	{
		_cursorPosition.x = 0;
	}
}

void drft::gui::Grid::moveCursorLeft()
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
}

// PANEL

bool drft::gui::Panel::onUpdate(const float dt)
{
	return true;
}

void drft::gui::Panel::onRender(sf::RenderTarget& target)
{
	target.draw(_shape);
	target.draw(_text);
}

// LABEL

sf::FloatRect drft::gui::Label::getGlobalBounds() const
{
	return _text.getGlobalBounds();
}

sf::FloatRect drft::gui::Label::getLocalBounds() const
{
	return _text.getLocalBounds();
}

bool drft::gui::Label::onUpdate(const float dt)
{
	const auto rect = getLocalBounds();
	_shape.setSize({ rect.width + _style[_state].innerPadding.x, rect.height + _style[_state].innerPadding.y });
	setOrigin(_origin);
	setTextOrigin(_textOrigin);

	return true;
}

void drft::gui::Label::onRender(sf::RenderTarget& target)
{
	target.draw(_shape);
	target.draw(_text);
}

// BUTTON

bool drft::gui::Button::onUpdate(const float dt)
{
	return true;
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

bool drft::gui::Icon::onUpdate(const float dt)
{
	_sprite.setColor(_style[_state].fillColor);
	_sprite.setPosition(_shape.getPosition());

	return true;
}

void drft::gui::Icon::onRender(sf::RenderTarget& target)
{
	//target.draw(_shape);
	target.draw(_sprite);
	target.draw(_text);
}

void drft::gui::Element::popBack()
{
	if (_children.empty()) return;
	remove(_children.size() - 1);
}

bool drft::gui::Element::isContainer() const
{
	if (dynamic_cast<const gui::Container*>(this) == nullptr)
	{
		return false;
	}
	return true;
}

// POPUP MESSAGE

bool drft::gui::PopupMessage::onHandleEvent(const sf::Event& ev)
{
	return false;
}

// SCROLLING LIST

drft::gui::ScrollingList::ScrollingList(bool canInteract)
	: List(canInteract)
{
}

void drft::gui::ScrollingList::layoutChildren()
{
	float x = _style.at(_state).innerPadding.x;
	float y = _style.at(_state).innerPadding.y;
	float maxHeight = 0.f;

	for (int i = _firstDisplayableIndex; i < _firstDisplayableIndex + _numDisplayableChildren; ++i)
	{
		if (i >= _children.size()) break;
		auto rect = _children[i]->getGlobalBounds();
		maxHeight = std::max(maxHeight, rect.height + _style.at(_state).childPadding.y);
		_children[i]->setPosition({ x + _childOrigin.x, y + _childOrigin.y });
		_children[i]->layoutChildren();
		y += rect.height + _style.at(_state).childPadding.y;
	}
	determineNumberOfDisplayableChildren(maxHeight);
}

bool drft::gui::ScrollingList::onUpdate(const float dt)
{
	List::onUpdate(dt);
	setScrollBarSizeAndPosition();

	return false;
}

void drft::gui::ScrollingList::onRender(sf::RenderTarget& target)
{
	target.draw(_shape);
	target.draw(_text);
	if (_children.size() > _numDisplayableChildren)
	{
		target.draw(_scrollBar);
	}
	for (int i = _firstDisplayableIndex; i < _firstDisplayableIndex + _numDisplayableChildren; ++i)
	{
		if (i < _children.size())
		{
			_children[i]->render(target);
		}
	}
}

void drft::gui::ScrollingList::moveCursorUp()
{
	--_cursorPosition;
	if (_cursorPosition < 0)
	{
		_cursorPosition = _children.size() - 1;
		_firstDisplayableIndex = _children.size() - _numDisplayableChildren;
	}
	if (_cursorPosition < _firstDisplayableIndex)
	{
		--_firstDisplayableIndex;
	}
}

void drft::gui::ScrollingList::moveCursorDown()
{
	++_cursorPosition;
	if (_cursorPosition >= _children.size())
	{
		_cursorPosition = 0;
		_firstDisplayableIndex = 0;
	}
	if (_cursorPosition >= _firstDisplayableIndex + _numDisplayableChildren)
	{
		++_firstDisplayableIndex;
	}
}

void drft::gui::ScrollingList::setScrollBarSizeAndPosition()
{
	if (_children.empty()) return;

	auto height = _shape.getSize().y;
	auto heightPrime = height * (static_cast<float>(_numDisplayableChildren) / _children.size());
	float right = _shape.getGlobalBounds().left + _shape.getSize().x;
	float offset = (static_cast<float>(_firstDisplayableIndex) / (_children.size() - _numDisplayableChildren)) * (height-heightPrime);

	_scrollBar.setFillColor(sf::Color(80, 80, 80, 200));
	_scrollBar.setSize({ 4,heightPrime });
	_scrollBar.setPosition({ right - _scrollBar.getSize().x, _shape.getGlobalBounds().top + offset});
}

void drft::gui::ScrollingList::determineNumberOfDisplayableChildren(float largestChildHeight)
{
	_numDisplayableChildren = static_cast<int>(_shape.getSize().y / largestChildHeight);
}


