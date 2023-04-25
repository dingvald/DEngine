#pragma once
#include "Utility/SpriteIndexer.h"

namespace drft::gui
{
	enum class ElementState
	{
		Unselectable,
		Idle,
		Focused,
		Active
	};
	enum class ElementPosition
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
	enum class ElementCallbackType
	{
		OnUpdate,
		OnRender,
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

	class Container;

	class Element
	{
	public:
		using ElementPtr = std::unique_ptr<Element>;
		virtual void init() {}
		virtual bool handleEvent(const sf::Event& ev) 
		{
			return false;
		}

		bool update(const float dt)
		{
			if (!_isInitialized)
			{
				init();
				_isInitialized = true;
			}

			if (_callback.contains(ElementCallbackType::OnUpdate)
				&& _callback.at(ElementCallbackType::OnUpdate))
			{
				_callback.at(ElementCallbackType::OnUpdate)();
			}
			onUpdate(dt);
			applyStyle();

			return false;
		}
		void render(sf::RenderTarget& target)
		{
			if (_isVisible)
			{
				onRender(target);
			}
		}

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
			_shape.setPosition(position); 
			setTextPosition(_textPosition);

			return *this; 
		}
		Element& setOrigin(ElementPosition origin)
		{
			_origin = origin;
			switch (_origin)
			{
			case ElementPosition::TOP_LEFT:
				_shape.setOrigin({ 0,0 });
				break;
			case ElementPosition::TOP_CENTER:
				_shape.setOrigin(_shape.getSize().x / 2, 0);
				break;
			case ElementPosition::TOP_RIGHT:
				_shape.setOrigin(_shape.getSize().x, 0);
				break;
			case ElementPosition::BOTTOM_LEFT:
				_shape.setOrigin(0, _shape.getSize().y);
				break;
			case ElementPosition::BOTTOM_CENTER:
				_shape.setOrigin(_shape.getSize().x / 2, _shape.getSize().y);
				break;
			case ElementPosition::BOTTOM_RIGHT:
				_shape.setOrigin(_shape.getSize());
				break;
			case ElementPosition::CENTER_LEFT:
				_shape.setOrigin(0, _shape.getSize().y / 2.f);
				break;
			case ElementPosition::CENTER_RIGHT:
				_shape.setOrigin(_shape.getSize().x, _shape.getSize().y / 2.f);
				break;
			case ElementPosition::CENTER:
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

		// Sets the text displayed by the element string
		Element& setTextString(std::string&& string)
		{
			_text.setString(std::move(string));
			setTextOrigin(_textOrigin);
			return *this;
		}
		// Sets the text position relative to the element shape
		Element& setTextPosition(ElementPosition position)
		{ 
			_textPosition = position;
			const float SHAPE_TOP = round(_shape.getGlobalBounds().top);
			const float SHAPE_LEFT = round(_shape.getGlobalBounds().left);
			const float SHAPE_BOTTOM = round(SHAPE_TOP + _shape.getGlobalBounds().height);
			const float SHAPE_RIGHT = round(SHAPE_LEFT + _shape.getGlobalBounds().width);
			const float SHAPE_CENTER_X = round((SHAPE_LEFT + SHAPE_RIGHT) / 2.f);
			const float SHAPE_CENTER_Y = round((SHAPE_TOP + SHAPE_BOTTOM) / 2.f);

			switch (position)
			{
			case ElementPosition::TOP_RIGHT:
				_text.setPosition({ SHAPE_RIGHT, SHAPE_TOP });
				break;
			case ElementPosition::TOP_LEFT:
				_text.setPosition({ SHAPE_LEFT, SHAPE_TOP});
				break;
			case ElementPosition::TOP_CENTER:
				_text.setPosition({ SHAPE_CENTER_X, SHAPE_TOP });
				break;
			case ElementPosition::BOTTOM_CENTER:
				_text.setPosition({ SHAPE_CENTER_X, SHAPE_BOTTOM });
				break;
			case ElementPosition::BOTTOM_LEFT:
				_text.setPosition({ SHAPE_LEFT, SHAPE_BOTTOM});
				break;
			case ElementPosition::BOTTOM_RIGHT:
				_text.setPosition({ SHAPE_RIGHT, SHAPE_BOTTOM});
				break;
			case ElementPosition::CENTER_LEFT:
				_text.setPosition({ SHAPE_LEFT, SHAPE_CENTER_Y });
				break;
			case ElementPosition::CENTER_RIGHT:
				_text.setPosition({ SHAPE_RIGHT, SHAPE_CENTER_Y });
				break;
			case ElementPosition::CENTER:
				_text.setPosition({ SHAPE_CENTER_X, SHAPE_CENTER_Y });
				break;
			}

			return *this;
		}
		// Sets the origin of the text relative to its local coordinates
		Element& setTextOrigin(ElementPosition origin)
		{
			_textOrigin = origin;
			const auto textRect = _text.getLocalBounds();
			switch (_textOrigin)
			{
			case ElementPosition::TOP_LEFT:
				_text.setOrigin(0, 0);
				break;
			case ElementPosition::TOP_CENTER:
				_text.setOrigin(textRect.left + textRect.width / 2, 0);
				break;
			case ElementPosition::TOP_RIGHT:
				_text.setOrigin(textRect.left + textRect.width, 0);
				break;
			case ElementPosition::BOTTOM_LEFT:
				_text.setOrigin(0, textRect.top + textRect.height);
				break;
			case ElementPosition::BOTTOM_CENTER:
				_text.setOrigin(textRect.left + textRect.width / 2, textRect.top + textRect.height);
				break;
			case ElementPosition::BOTTOM_RIGHT:
				_text.setOrigin(textRect.left + textRect.width, textRect.top + textRect.height);
				break;
			case ElementPosition::CENTER_RIGHT:
				_text.setOrigin(textRect.left + textRect.width, textRect.top + textRect.height / 2);
				break;
			case ElementPosition::CENTER_LEFT:
				_text.setOrigin(0, textRect.top + textRect.height / 2);
				break;
			case ElementPosition::CENTER:
				_text.setOrigin(textRect.left + textRect.width / 2, textRect.top + textRect.height / 2);
				break;
			}
			return *this;
		}
		// Sets the visual style of the element for a given state
		Element& setStyle(ElementState state, Style&& style)
		{ 
			_style[state] = style;
			if (_state == state)
			{
				applyStyle();
			}

			return *this;
		}
		Style& modifyStyle(ElementState state)
		{
			return _style.at(state);
		}

		Element& registerCallback(ElementCallbackType type, std::function<bool()> callback)
		{
			_callback[type] = callback;
			return *this;
		}

		bool isVisible() const
		{
			return _isVisible;
		}
		void setVisibility(bool isVisible)
		{
			_isVisible = isVisible;
		}

		void setState(ElementState state)
		{
			_state = state;
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

	protected:
		virtual void onUpdate(const float dt) = 0;
		virtual void onRender(sf::RenderTarget& target) = 0;

	private:
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
		}

	protected:
		std::string _name;
		sf::RectangleShape _shape;
		sf::Text _text;
		ElementState _state = ElementState::Idle;
		ElementPosition _origin = ElementPosition::CENTER;
		ElementPosition _textOrigin = ElementPosition::CENTER;
		ElementPosition _textPosition = ElementPosition::CENTER;
		std::map<ElementState, Style> _style;
		std::unordered_map<ElementCallbackType, std::function<bool()> > _callback;

	private:
		bool _isVisible = true;
		bool _isInitialized = false;
	};

	class Container : public Element
	{
	public:
		using ElementPtr = std::unique_ptr<Element>;
		using InsertedElement = Element;

		template<typename T>
		InsertedElement& insert(T&& child)
		{
			static_assert(std::derived_from<T, Element>);
			_children.push_back(std::make_unique<T>(std::move(child)));

			return *_children.back();
		}
		void remove(size_t index = 0)
		{
			if (_children.size() <= index) return;
			auto itr = _children.begin() + index;
			_children.erase(itr);
		}

		Element& operator[](size_t index)
		{
			return *_children.at(index);
		}

		virtual void layoutChildren() = 0;
		Element& setChildrenOrigin(ElementPosition origin, sf::Vector2f offset = { 0,0 })
		{
			_childAlignment = origin;
			_childOffset = offset;

			const float SHAPE_TOP = (_shape.getGlobalBounds().top);
			const float SHAPE_LEFT = (_shape.getGlobalBounds().left);
			const float SHAPE_BOTTOM = (SHAPE_TOP + _shape.getGlobalBounds().height);
			const float SHAPE_RIGHT = (SHAPE_LEFT + _shape.getGlobalBounds().width);
			const float SHAPE_CENTER_X = ((SHAPE_LEFT + SHAPE_RIGHT) / 2.f);
			const float SHAPE_CENTER_Y = ((SHAPE_TOP + SHAPE_BOTTOM) / 2.f);

			switch (origin)
			{
			case ElementPosition::TOP_RIGHT:
				_childOrigin = { SHAPE_RIGHT, SHAPE_TOP };
				break;
			case ElementPosition::TOP_LEFT:
				_childOrigin = { SHAPE_LEFT, SHAPE_TOP };
				break;
			case ElementPosition::TOP_CENTER:
				_childOrigin = { SHAPE_CENTER_X, SHAPE_TOP };
				break;
			case ElementPosition::BOTTOM_CENTER:
				_childOrigin = { SHAPE_CENTER_X, SHAPE_BOTTOM };
				break;
			case ElementPosition::BOTTOM_LEFT:
				_childOrigin = { SHAPE_LEFT, SHAPE_BOTTOM };
				break;
			case ElementPosition::BOTTOM_RIGHT:
				_childOrigin = { SHAPE_RIGHT, SHAPE_BOTTOM };
				break;
			case ElementPosition::CENTER_RIGHT:
				_childOrigin = { SHAPE_RIGHT, SHAPE_CENTER_Y };
				break;
			case ElementPosition::CENTER_LEFT:
				_childOrigin = { SHAPE_RIGHT, SHAPE_CENTER_Y };
				break;
			case ElementPosition::CENTER:
				_childOrigin = { SHAPE_CENTER_X, SHAPE_CENTER_Y };
				break;
			}
			_childOrigin += offset;

			return *this;
		}

	protected:
		std::vector<ElementPtr> _children;
		ElementPosition _childAlignment = ElementPosition::CENTER;
		sf::Vector2f _childOffset = { 0,0 };
		sf::Vector2f _childOrigin;
	};

	class Panel : public Element
	{
	public:
		bool handleEvent(const sf::Event& ev) override;

	protected:
		void onUpdate(const float dt) override;
		void onRender(sf::RenderTarget& target) override;
	};

	class SingleContainer : public Container
	{
	public:
		void init() override;
		void layoutChildren() override;

	protected:
		void onUpdate(const float dt) override;
		void onRender(sf::RenderTarget& target) override;

	};

	class List : public Container
	{
	public:
		void init() override;
		bool handleEvent(const sf::Event& ev) override;
		void layoutChildren() override;

	protected:
		void onUpdate(const float dt) override;
		void onRender(sf::RenderTarget& target) override;

	private:
		void setStartingCursorPosition();
		void moveCursorDown();
		void moveCursorUp();

	private:
		int _cursorPosition = 0;
	};

	class Grid : public Container
	{
	public:
		Grid(int columns, int rows);
		bool handleEvent(const sf::Event& ev) override;
		void layoutChildren() override;

	protected:
		void onUpdate(const float dt) override;
		void onRender(sf::RenderTarget& target) override;

	private:
		void setStartingCursorPosition();
		void autoSize();
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
		void onUpdate(const float dt) override;
		void onRender(sf::RenderTarget& target) override;
	};

	class Button : public Element
	{
	protected:
		void onUpdate(const float dt) override;
		void onRender(sf::RenderTarget& target) override;
	};

	class Icon : public Element
	{
	public:
		Icon(sf::Sprite sprite);
		void init() override;

	protected:
		void onUpdate(const float dt) override;
		void onRender(sf::RenderTarget& target) override;

	private:
		sf::Sprite _sprite;
	};
}


