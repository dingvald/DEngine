#pragma once
#include "Utility/SpriteIndexer.h"

namespace drft::gui
{
	enum class ElementState
	{
		Unselectable,
		Idle,
		Focused,
		Active,
		Total
	};
	enum class ElementAlignment
	{
		TOP_LEFT,
		TOP_CENTER,
		TOP_RIGHT,
		BOTTOM_LEFT,
		BOTTOM_CENTER,
		BOTTOM_RIGHT,
		CENTER
	};
	enum class ElementOrigin
	{
		TOP_LEFT,
		TOP_CENTER,
		TOP_RIGHT,
		BOTTOM_LEFT,
		BOTTOM_CENTER,
		BOTTOM_RIGHT,
		CENTER_LEFT,
		CENTER_RIGHT,
		CENTER
	};
	enum class ElementTextPosition
	{
		TOP_RIGHT,
		TOP_LEFT,
		TOP_CENTER,
		BOTTOM_RIGHT,
		BOTTOM_LEFT,
		BOTTOM_CENTER,
		CENTER_RIGHT,
		CENTER_LEFT,
		CENTER
	};
	enum class ElementCallbackType
	{
		OnSelect,
		OnFocus,
		OnLeave,
		OnIsSelectable
	};

	struct Style
	{
		sf::Color fillColor = { 0,0,0,0 }; // clear
		sf::Color outlineColor = { 0,0,0,0 };

		float outlineThickness = 0.f;
		float innerPadding = 0.f;
		float childPadding = 16.f; // minimum distance between children

		sf::Font* font = nullptr;
		sf::Color textColor = {};
		int textSize = 16;
		sf::Vector2f textScale = { 1.f, 1.f };
	};


	class Element
	{
	public:
		using ElementPtr = std::unique_ptr<Element>;

		void onSelect()
		{
			setState(ElementState::Active);
			if (_callback.contains(ElementCallbackType::OnSelect) 
				&& _callback.at(ElementCallbackType::OnSelect))
			{
				_callback.at(ElementCallbackType::OnSelect)();
			}
		}
		void onFocus()
		{
			setState(ElementState::Focused);
			if (_callback.contains(ElementCallbackType::OnFocus)
				&& _callback.at(ElementCallbackType::OnFocus))
			{
				_callback.at(ElementCallbackType::OnFocus)();
			}
		}
		void onLeave()
		{
			setState(ElementState::Idle);
			if (_callback.contains(ElementCallbackType::OnLeave)
				&& _callback.at(ElementCallbackType::OnLeave))
			{
				_callback.at(ElementCallbackType::OnLeave)();
			}
		}
		bool isSelectable()
		{
			if (_callback.contains(ElementCallbackType::OnIsSelectable)
				&& _callback.at(ElementCallbackType::OnIsSelectable))
			{
				return _callback.at(ElementCallbackType::OnIsSelectable)();
			}
			return true;
		}

		Element& setPosition(sf::Vector2f position)
		{ 
			if (_parent)
			{
				position += _parent->_childOrigin;
			}

			_shape.setPosition(position); 
			setTextPosition(_textPosition);

			return *this; 
		}
		Element& setOrigin(ElementOrigin origin)
		{
			_origin = origin;
			switch (_origin)
			{
			case ElementOrigin::TOP_LEFT:
				_shape.setOrigin({ 0,0 });
				break;
			case ElementOrigin::TOP_CENTER:
				_shape.setOrigin(_shape.getSize().x / 2, 0);
				break;
			case ElementOrigin::TOP_RIGHT:
				_shape.setOrigin(_shape.getSize().x, 0);
				break;
			case ElementOrigin::BOTTOM_LEFT:
				_shape.setOrigin(0, _shape.getSize().y);
				break;
			case ElementOrigin::BOTTOM_CENTER:
				_shape.setOrigin(_shape.getSize().x / 2, _shape.getSize().y);
				break;
			case ElementOrigin::BOTTOM_RIGHT:
				_shape.setOrigin(_shape.getSize());
				break;
			case ElementOrigin::CENTER_LEFT:
				_shape.setOrigin(0, _shape.getSize().y / 2.f);
				break;
			case ElementOrigin::CENTER_RIGHT:
				_shape.setOrigin(_shape.getSize().x, _shape.getSize().y / 2.f);
				break;
			case ElementOrigin::CENTER:
				_shape.setOrigin(_shape.getSize() / 2.f);
				break;
			}
			return *this;
		}
		Element& setSize(sf::Vector2f size)
		{ 
			_shape.setSize(size); 
			setOrigin(_origin);
			return *this;
		}

		Element& setTextString(std::string&& string)
		{
			_text.setString(std::move(string));
			setTextOrigin(_textOrigin);
			return *this;
		}
		Element& setTextPosition(ElementTextPosition position)
		{ 
			const float SHAPE_TOP = round(_shape.getGlobalBounds().top);
			const float SHAPE_LEFT = round(_shape.getGlobalBounds().left);
			const float SHAPE_BOTTOM = round(SHAPE_TOP + _shape.getGlobalBounds().height);
			const float SHAPE_RIGHT = round(SHAPE_LEFT + _shape.getGlobalBounds().width);
			const float SHAPE_CENTER_X = round((SHAPE_LEFT + SHAPE_RIGHT) / 2.f);
			const float SHAPE_CENTER_Y = round((SHAPE_TOP + SHAPE_BOTTOM) / 2.f);

			switch (position)
			{
			case ElementTextPosition::TOP_RIGHT:
				_text.setPosition({ SHAPE_RIGHT, SHAPE_TOP });
				break;
			case ElementTextPosition::TOP_LEFT:
				_text.setPosition({ SHAPE_LEFT, SHAPE_TOP});
				break;
			case ElementTextPosition::TOP_CENTER:
				_text.setPosition({ SHAPE_CENTER_X, SHAPE_TOP });
				break;
			case ElementTextPosition::BOTTOM_CENTER:
				_text.setPosition({ SHAPE_CENTER_X, SHAPE_BOTTOM });
				break;
			case ElementTextPosition::BOTTOM_LEFT:
				_text.setPosition({ SHAPE_LEFT, SHAPE_BOTTOM});
				break;
			case ElementTextPosition::BOTTOM_RIGHT:
				_text.setPosition({ SHAPE_RIGHT, SHAPE_BOTTOM});
				break;
			case ElementTextPosition::CENTER_LEFT:
				_text.setPosition({ SHAPE_LEFT, SHAPE_CENTER_Y });
				break;
			case ElementTextPosition::CENTER_RIGHT:
				_text.setPosition({ SHAPE_RIGHT, SHAPE_CENTER_Y });
				break;
			case ElementTextPosition::CENTER:
				_text.setPosition({ SHAPE_CENTER_X, SHAPE_CENTER_Y });
				break;
			}

			return *this;
		}
		Element& setTextOrigin(ElementOrigin origin)
		{
			_textOrigin = origin;
			const auto textRect = _text.getLocalBounds();
			switch (_textOrigin)
			{
			case ElementOrigin::TOP_LEFT:
				_text.setOrigin(0, 0);
				break;
			case ElementOrigin::TOP_CENTER:
				_text.setOrigin(textRect.left + textRect.width / 2, 0);
				break;
			case ElementOrigin::TOP_RIGHT:
				_text.setOrigin(textRect.left + textRect.width, 0);
				break;
			case ElementOrigin::BOTTOM_LEFT:
				_text.setOrigin(0, textRect.top + textRect.height);
				break;
			case ElementOrigin::BOTTOM_CENTER:
				_text.setOrigin(textRect.left + textRect.width / 2, textRect.top + textRect.height);
				break;
			case ElementOrigin::BOTTOM_RIGHT:
				_text.setOrigin(textRect.left + textRect.width, textRect.top + textRect.height);
				break;
			case ElementOrigin::CENTER_RIGHT:
				_text.setOrigin(textRect.left + textRect.width, textRect.top + textRect.height / 2);
				break;
			case ElementOrigin::CENTER_LEFT:
				_text.setOrigin(0, textRect.top + textRect.height / 2);
				break;
			case ElementOrigin::CENTER:
				_text.setOrigin(textRect.left + textRect.width / 2, textRect.top + textRect.height / 2);
				break;
			}
			return *this;
		}

		Element& setChildrenOrigin(ElementAlignment alignment, sf::Vector2f offset = {0,0})
		{
			_childAlignment = alignment;
			_childOffset = offset;

			const float SHAPE_TOP = (_shape.getGlobalBounds().top);
			const float SHAPE_LEFT = (_shape.getGlobalBounds().left);
			const float SHAPE_BOTTOM = (SHAPE_TOP + _shape.getGlobalBounds().height);
			const float SHAPE_RIGHT = (SHAPE_LEFT + _shape.getGlobalBounds().width);
			const float SHAPE_CENTER_X = ((SHAPE_LEFT + SHAPE_RIGHT) / 2.f);
			const float SHAPE_CENTER_Y = ((SHAPE_TOP + SHAPE_BOTTOM) / 2.f);

			switch (alignment)
			{
			case ElementAlignment::TOP_RIGHT:
				_childOrigin = { SHAPE_RIGHT, SHAPE_TOP };
				break;
			case ElementAlignment::TOP_LEFT:
				_childOrigin = { SHAPE_LEFT, SHAPE_TOP };
				break;
			case ElementAlignment::TOP_CENTER:
				_childOrigin = { SHAPE_CENTER_X, SHAPE_TOP };
				break;
			case ElementAlignment::BOTTOM_CENTER:
				_childOrigin = { SHAPE_CENTER_X, SHAPE_BOTTOM };
				break;
			case ElementAlignment::BOTTOM_LEFT:
				_childOrigin = { SHAPE_LEFT, SHAPE_BOTTOM };
				break;
			case ElementAlignment::BOTTOM_RIGHT:
				_childOrigin = { SHAPE_RIGHT, SHAPE_BOTTOM };
				break;
			case ElementAlignment::CENTER:
				_childOrigin = { SHAPE_CENTER_X, SHAPE_CENTER_Y };
				break;
			}
			_childOrigin += offset;

			return *this;
		}
		Element& setStyle(ElementState state, Style&& style)
		{ 
			_style[state] = style;
			_needsStyleUpdate = true;
			if (state == _state)
			{
				applyStyle();
			}
			
			return *this;
		}

		Style& modifyStyle(ElementState state)
		{
			_needsStyleUpdate = true;
			return _style.at(state);
		}

		Element& registerCallback(ElementCallbackType type, std::function<bool()> callback)
		{
			_callback[type] = callback;
			return *this;
		}

		template<typename T>
		Element& insertChild(std::string name, T&& child)
		{
			static_assert(std::derived_from<T, Element>);
			_children.push_back(std::make_unique<T>(std::move(child)));
			_children.back()->_name = name;
			_children.back()->_parent = this;
			_children.back()->setPosition({0,0});
			_childrenMap[name] = _children.size() - 1;

			return *this;
		}
		Element& removeChild(std::string name)
		{

			_childrenMap.erase(name);
		}
		Element& operator[](std::string name)
		{
			if (!_childrenMap.contains(name))
			{
				throw std::exception("Map does not contain name");
			}
			return *_children.at(_childrenMap.at(name));
		}

		void setState(ElementState state)
		{
			_state = state;
			applyStyle();
		}
		ElementState getState() const
		{
			return _state;
		}

		virtual sf::FloatRect getGlobalBounds() const
		{
			return _shape.getGlobalBounds();
		}
		virtual sf::FloatRect getLocalBounds() const
		{
			return _shape.getLocalBounds();
		}

		virtual void init() {}
		virtual bool handleEvent(const sf::Event& ev) = 0;
		bool update(const float dt)
		{
			if (!_isInitialized)
			{
				init();
				_isInitialized = true;
			}
			if (needsStyleUpdate())
			{
				applyStyle();
			}

			onUpdate(dt);
			setChildrenOrigin(_childAlignment, _childOffset);
			layoutChildren();

			for (auto& child : _children)
			{
				child->update(dt);
			}

			return false;
		}
		void render(sf::RenderTarget& target)
		{
			onRender(target);
			for (auto& child : _children)
			{
				child->render(target);
			}
		}

	protected:
		virtual void onUpdate(const float dt) {}
		virtual void onRender(sf::RenderTarget& target) {}
		virtual void layoutChildren() 
		{
			for (auto& child : _children)
			{
				child->setPosition({ 0,0 });
			}
		}
		virtual void applyStyle()
		{
			_shape.setFillColor(_style[_state].fillColor);
			_shape.setOutlineColor(_style[_state].outlineColor);
			_shape.setOutlineThickness(_style[_state].outlineThickness);

			_text.setFont(*_style[_state].font);
			_text.setFillColor(_style[_state].textColor);
			_text.setCharacterSize(_style[_state].textSize);
			_text.setScale(_style[_state].textScale);

			setTextOrigin(_textOrigin);
			setTextPosition(_textPosition);
			

			_needsStyleUpdate = false;
		}

	protected:
		std::string _name;
		std::unordered_map<ElementCallbackType, std::function<bool()> > _callback;
		sf::RectangleShape _shape;
		sf::Text _text;
		ElementTextPosition _textPosition = ElementTextPosition::CENTER;
		Element* _parent = nullptr;
		std::unordered_map<std::string, size_t> _childrenMap;
		std::vector<ElementPtr> _children;
		ElementAlignment _childAlignment = gui::ElementAlignment::CENTER;
		sf::Vector2f _childOffset = { 0,0 };
		sf::Vector2f _childOrigin;
		ElementState _state = ElementState::Idle;
		ElementOrigin _origin = ElementOrigin::CENTER;
		ElementOrigin _textOrigin = ElementOrigin::CENTER;
		std::map<ElementState, Style> _style;

	private:
		bool needsStyleUpdate() const
		{
			return _needsStyleUpdate;
		}

	private:
		bool _isInitialized = false;
		bool _needsStyleUpdate = true;
	};

	class Window : public Element
	{
	public:
		bool handleEvent(const sf::Event& ev) override;

	protected:
		void onRender(sf::RenderTarget& target) override;
	};

	class List : public Element
	{
	public:
		void init() override;
		bool handleEvent(const sf::Event& ev) override;

	protected:
		void onUpdate(const float dt) override;
		void onRender(sf::RenderTarget& target) override;

	private:
		void setStartingCursorPosition();
		void layoutChildren() override;
		void moveCursorDown();
		void moveCursorUp();

	private:
		int _cursorPosition = 0;
	};

	// Auto-sizing - no need to set size
	class Grid : public Element
	{
	public:
		Grid(int columns, int rows);
		bool handleEvent(const sf::Event& ev) override;

	protected:
		void onUpdate(const float dt) override;
		void onRender(sf::RenderTarget& target) override;

	private:
		void setStartingCursorPosition();
		void autoSize();
		void layoutChildren() override;
		void moveCursorDown();
		void moveCursorUp();
		void moveCursorRight();
		void moveCursorLeft();

	private:
		bool _isInitialized = false;
		int _numColumns;
		int _numRows;
		sf::Vector2i _cursorPosition = { 0, 0 };
	};

	class Label : public Element
	{
	public:
		bool handleEvent(const sf::Event& ev) override;
		sf::FloatRect getGlobalBounds() const override;
		sf::FloatRect getLocalBounds() const override;

	protected:
		void onRender(sf::RenderTarget& target) override;
	};

	class Button : public Element
	{
	public:
		bool handleEvent(const sf::Event& ev) override;

	protected:
		void onRender(sf::RenderTarget& target) override;
	};

	class Icon : public Element
	{
	public:
		Icon(sf::Sprite sprite);
		void init() override;
		bool handleEvent(const sf::Event& ev) override;

	protected:
		void onUpdate(const float dt) override;
		void onRender(sf::RenderTarget& target) override;

	private:
		sf::Sprite _sprite;
	};
}


