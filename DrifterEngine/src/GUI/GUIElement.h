#pragma once

namespace drft::gui
{
	enum class ElementType
	{
		Window,
		Label,
		List,
		Button,
		Scrollbar,
		Textfield,
		Total
	};
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
		sf::Color outlineColor = {};
		float outlineThickness = 0.f;
		float innerPadding = 0.f;
		float childPadding = 16.f;

		sf::Font* font = nullptr;
		sf::Color textColor = {};
		int textSize = 16;
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
				position += _parent->_childAlignment;
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
			case ElementOrigin::CENTER:
				_shape.setOrigin(_shape.getSize() / 2.f);
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
			case ElementTextPosition::CENTER:
				_text.setPosition({ SHAPE_CENTER_X, SHAPE_CENTER_Y });
				break;
			}

			return *this;
		}
		Element& setTextOrigin(ElementOrigin origin)
		{
			_textOrigin = origin;
			const auto localWidth = _text.getLocalBounds().width;
			const auto localHeight = _text.getLocalBounds().height;
			switch (_textOrigin)
			{
			case ElementOrigin::TOP_LEFT:
				_text.setOrigin(0, 0);
				break;
			case ElementOrigin::TOP_CENTER:
				_text.setOrigin(localWidth / 2, 0);
				break;
			case ElementOrigin::TOP_RIGHT:
				_text.setOrigin(localWidth, 0);
				break;
			case ElementOrigin::BOTTOM_LEFT:
				_text.setOrigin(0, localHeight);
				break;
			case ElementOrigin::BOTTOM_CENTER:
				_text.setOrigin(localWidth / 2, localHeight);
				break;
			case ElementOrigin::BOTTOM_RIGHT:
				_text.setOrigin(localWidth, localHeight);
				break;
			case ElementOrigin::CENTER:
				_text.setOrigin(localWidth/2, localHeight/2);
			}
			return *this;
		}
		Element& setChildrenAlignment(ElementAlignment alignment)
		{
			const float SHAPE_TOP = (_shape.getGlobalBounds().top);
			const float SHAPE_LEFT = (_shape.getGlobalBounds().left);
			const float SHAPE_BOTTOM = (SHAPE_TOP + _shape.getGlobalBounds().height);
			const float SHAPE_RIGHT = (SHAPE_LEFT + _shape.getGlobalBounds().width);
			const float SHAPE_CENTER_X = ((SHAPE_LEFT + SHAPE_RIGHT) / 2.f);
			const float SHAPE_CENTER_Y = ((SHAPE_TOP + SHAPE_BOTTOM) / 2.f);

			switch (alignment)
			{
			case ElementAlignment::TOP_RIGHT:
				_childAlignment = { SHAPE_RIGHT, SHAPE_TOP };
				break;
			case ElementAlignment::TOP_LEFT:
				_childAlignment = { SHAPE_LEFT, SHAPE_TOP };
				break;
			case ElementAlignment::TOP_CENTER:
				_childAlignment = { SHAPE_CENTER_X, SHAPE_TOP };
				break;
			case ElementAlignment::BOTTOM_CENTER:
				_childAlignment = { SHAPE_CENTER_X, SHAPE_BOTTOM };
				break;
			case ElementAlignment::BOTTOM_LEFT:
				_childAlignment = { SHAPE_LEFT, SHAPE_BOTTOM };
				break;
			case ElementAlignment::BOTTOM_RIGHT:
				_childAlignment = { SHAPE_RIGHT, SHAPE_BOTTOM };
				break;
			case ElementAlignment::CENTER:
				_childAlignment = { SHAPE_CENTER_X, SHAPE_CENTER_Y };
				break;
			}

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

		Element& registerCallback(ElementCallbackType type, std::function<bool()> callback)
		{
			_callback[type] = callback;
			return *this;
		}
		template<typename T>
		Element& insertChild(std::string name, T&& child)
		{
			static_assert(std::derived_from<T, Element>);
			_children.push_back(std::make_unique<T>());
			_children.back()->_name = name;
			_children.back()->_parent = this;
			_children.back()->setPosition({0,0});
			_childrenMap[name] = _children.size() - 1;

			return *this;
		}
		Element& removeChild(std::string name)
		{

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

		virtual bool handleEvent(const sf::Event& ev) = 0;
		virtual bool update(const float dt) = 0;
		virtual void render(sf::RenderTarget& target) = 0;

	protected:
		bool needsStyleUpdate() const
		{
			return _needsStyleUpdate;
		}
		void applyStyle()
		{
			_shape.setFillColor(_style[_state].fillColor);
			_shape.setOutlineColor(_style[_state].outlineColor);
			_shape.setOutlineThickness(_style[_state].outlineThickness);

			_text.setFont(*_style[_state].font);
			_text.setFillColor(_style[_state].textColor);
			_text.setCharacterSize(_style[_state].textSize);

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
		sf::Vector2f _childAlignment;
		ElementState _state = ElementState::Idle;
		ElementOrigin _origin = ElementOrigin::CENTER;
		ElementOrigin _textOrigin = ElementOrigin::CENTER;
		std::map<ElementState, Style> _style;
	private:
		bool _needsStyleUpdate = true;
	};

	class Window : public Element
	{
	public:
		bool handleEvent(const sf::Event& ev) override;
		bool update(const float dt) override;
		void render(sf::RenderTarget& target) override;
	};

	class List : public Element
	{
	public:
		bool handleEvent(const sf::Event& ev) override;
		bool update(const float dt) override;
		void render(sf::RenderTarget& target) override;

	private:
		void setStartingCursorPosition();
		void moveCursorDown();
		void moveCursorUp();

	private:
		bool _isInitialized = false;
		int _cursorIndex = 0;
	};

	class Label : public Element
	{
	public:
		bool handleEvent(const sf::Event& ev) override;
		bool update(const float dt) override;
		void render(sf::RenderTarget& target) override;
	};

	class Button : public Element
	{
	public:
		bool handleEvent(const sf::Event& ev) override;
		bool update(const float dt) override;
		void render(sf::RenderTarget& target) override;
	};
}


