#pragma once
#include "Utility/SpriteIndexer.h"

/*
Things I apologize for in the following code:

- All derived classes are in the same file
- Most code is located in the header
- Multiple classes that are only used for specific things
- Not the cleanest interface
- Using an "onUpdate" callback to inject runtime logic into concrete classes

Sorry.
*/

namespace drft::gui
{
	enum class ElementState
	{
		Unselectable,
		FocusedUnselectable,
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
		OnIsSelectable,
		OnRefresh
	};

	struct Style
	{
		sf::Color fillColor = { 0,0,0,0 }; // clear
		sf::Color outlineColor = { 0,0,0,0 };

		float outlineThickness = 0.f;
		sf::Vector2f innerPadding = { 0.f, 0.f }; // Space between outer edge and inner children
		sf::Vector2f childPadding = { 0.f, 0.f }; // minimum distance between children

		sf::Font* font = nullptr;
		sf::Color textColor = sf::Color::White;
		int textSize = 16;
		sf::Vector2f textScale = { 1.f, 1.f };
	};

	class Element
	{
	public:
		using ElementPtr = std::unique_ptr<Element>;
		virtual void init() {}

		bool handleEvent(const sf::Event& ev) 
		{
			if (!_isVisible) return true;

			return onHandleEvent(ev);
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
			bool canPropagate = onUpdate(dt);
			applyStyle();

			return canPropagate;
		}
		void render(sf::RenderTarget& target)
		{
			if (!_isVisible) return;
		
			onRender(target);
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
		sf::Vector2f getPosition() const
		{
			return _shape.getPosition();
		}
		Element& setLocalPosition(sf::Vector2f position)
		{
			_localPosition = position;
			return *this;
		}
		sf::Vector2f getLocalPosition() const
		{
			return _localPosition;
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
		sf::Vector2f getSize() const
		{
			return _shape.getSize();
		}
		virtual sf::FloatRect getGlobalBounds() const
		{
			return _shape.getGlobalBounds();
		}
		virtual sf::FloatRect getLocalBounds() const
		{
			return _shape.getLocalBounds();
		}
		virtual void layoutChildren() {};

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
				_text.setPosition(sf::Vector2f{ SHAPE_RIGHT, SHAPE_TOP });
				break;
			case ElementPosition::TOP_LEFT:
				_text.setPosition(sf::Vector2f{ SHAPE_LEFT, SHAPE_TOP});
				break;
			case ElementPosition::TOP_CENTER:
				_text.setPosition(sf::Vector2f{ SHAPE_CENTER_X, SHAPE_TOP });
				break;
			case ElementPosition::BOTTOM_CENTER:
				_text.setPosition(sf::Vector2f{ SHAPE_CENTER_X, SHAPE_BOTTOM });
				break;
			case ElementPosition::BOTTOM_LEFT:
				_text.setPosition(sf::Vector2f{ SHAPE_LEFT, SHAPE_BOTTOM});
				break;
			case ElementPosition::BOTTOM_RIGHT:
				_text.setPosition(sf::Vector2f{ SHAPE_RIGHT, SHAPE_BOTTOM});
				break;
			case ElementPosition::CENTER_LEFT:
				_text.setPosition(sf::Vector2f{ SHAPE_LEFT, SHAPE_CENTER_Y });
				break;
			case ElementPosition::CENTER_RIGHT:
				_text.setPosition(sf::Vector2f{ SHAPE_RIGHT, SHAPE_CENTER_Y });
				break;
			case ElementPosition::CENTER:
				_text.setPosition(sf::Vector2f{ SHAPE_CENTER_X, SHAPE_CENTER_Y });
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

		template<typename T>
		T& insert(std::string&& name, T&& child)
		{
			static_assert(std::derived_from<T, Element>);
			_children.push_back(std::make_unique<T>(std::move(child)));
			_childrenMap[name] = _children.size() - 1;

			return static_cast<T&>(*_children.back());
		}
		void remove(std::string&& name)
		{
			auto itr = _children.begin() + _childrenMap.at(name);
			int lastIndex = _children.size() - 1;
			for (auto& [key, val] : _childrenMap)
			{
				if (val == lastIndex)
				{
					val = _childrenMap.at(name);
					break;
				}
			}

			_childrenMap.erase(name);
			std::iter_swap(itr, _children.end() - 1);
			_children.pop_back();
		}
		void remove(size_t index)
		{
			auto itr = _children.begin() + index;
			int lastIndex = _children.size() - 1;
			std::string name;
			for (auto& [key, val] : _childrenMap)
			{
				if (val == index)
				{
					name = key;
					break;
				}
			}
			for (auto& [key, val] : _childrenMap)
			{
				if (val == lastIndex)
				{
					val = index;
					break;
				}
			}

			_childrenMap.erase(name);
			std::iter_swap(itr, _children.end() - 1);
			_children.pop_back();
		}
		void popBack();
		bool isEmpty() const
		{
			return _children.empty();
		}
		void clear()
		{
			_childrenMap.clear();
			_children.clear();
		}

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

		Element& operator[](size_t index)
		{
			return *_children.at(index);
		}
		Element& operator[](std::string&& name)
		{
			return *_children.at(_childrenMap.at(name));
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
			if (_state == state) return;
			_state = state;
			switch (state)
			{
			case ElementState::Idle:
				onLeave();
				break;
			case ElementState::Focused:
				onFocus();
				break;
			case ElementState::Active:
				onSelect();
				break;
			default:
				break;
			}
			applyStyle();
		}
		ElementState getState() const
		{
			return _state;
		}

	protected:
		virtual bool onHandleEvent(const sf::Event& ev) { return true; }
		virtual bool onUpdate(const float dt) = 0;
		virtual void onRender(sf::RenderTarget& target) = 0;
		virtual void onSelect()
		{
			if (_callback.contains(ElementCallbackType::OnSelect)
				&& _callback.at(ElementCallbackType::OnSelect))
			{
				_callback.at(ElementCallbackType::OnSelect)();
			}
		}
		virtual void onFocus()
		{
			if (_callback.contains(ElementCallbackType::OnFocus)
				&& _callback.at(ElementCallbackType::OnFocus))
			{
				_callback.at(ElementCallbackType::OnFocus)();
			}
		}
		virtual void onLeave()
		{
			if (_callback.contains(ElementCallbackType::OnLeave)
				&& _callback.at(ElementCallbackType::OnLeave))
			{
				_callback.at(ElementCallbackType::OnLeave)();
			}
		}

	private:
		virtual void applyStyle()
		{
			if (!_style.contains(_state)) return;

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
		sf::RectangleShape _shape;
		sf::Text _text;
		ElementState _state = ElementState::Idle;
		ElementPosition _origin = ElementPosition::CENTER;
		ElementPosition _textOrigin = ElementPosition::CENTER;
		ElementPosition _textPosition = ElementPosition::CENTER;
		std::map<ElementState, Style> _style;

		std::map<std::string, size_t> _childrenMap;
		std::vector<ElementPtr> _children;
		ElementPosition _childAlignment = ElementPosition::CENTER;
		sf::Vector2f _childOffset = { 0,0 };
		sf::Vector2f _childOrigin;
		sf::Vector2f _localPosition = { 0,0 };

	private:
		std::unordered_map<ElementCallbackType, std::function<bool()> > _callback;
		bool _isVisible = true;
		bool _isInitialized = false;
	};

	class Container : public Element
	{
	public:
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
		virtual void layoutChildren() = 0;	
	};

	// Container that allows the calling code to control which child has priority.
	class FlowControl : public Container
	{
	public:
		void transferControlTo(std::string&& childname);
		void layoutChildren() override;

	protected:
		bool onHandleEvent(const sf::Event& ev) override;
		bool onUpdate(const float dt) override;
		void onRender(sf::RenderTarget& target) override;

	private:
		Element* _inControl = nullptr;
	};

	// General-purpose container that makes no attempt to control it's children.
	// Will pass thorugh all event, update, and render calls.
	class Blob : public Container
	{
	public:
		void layoutChildren() override;

	protected:
		bool onHandleEvent(const sf::Event& ev) override;
		bool onUpdate(const float dt) override;
		void onRender(sf::RenderTarget& target) override;
	};

	// Container where elements added will have hierachical control.
	// (i.e. VISIBLE items added at the top can supersede items below)
	class Stack : public Container
	{
	public:
		void layoutChildren() override;

	protected:
		bool onHandleEvent(const sf::Event& ev) override;
		bool onUpdate(const float dt) override;
		void onRender(sf::RenderTarget& target) override;
	};

	// Container that holds only one child element.
	class SingleContainer : public Container
	{
	public:
		void layoutChildren() override;

	protected:
		bool onHandleEvent(const sf::Event& ev) override;
		bool onUpdate(const float dt) override;
		void onRender(sf::RenderTarget& target) override;
	};

	class DualContainer : public Container
	{
	public:
		void layoutChildren() override;

	protected:
		bool onHandleEvent(const sf::Event& ev) override;
		bool onUpdate(const float dt) override;
		void onRender(sf::RenderTarget& target) override;

		void onSelect() override;
		void onFocus() override;
		void onLeave() override;
	};

	class MultiContainer : public DualContainer
	{
	public:
		void layoutChildren() override;
	};

	// Container where items added will be ordered in an auto-sizing list from top -> down.
	class List : public Container
	{
	public:
		List(bool canInteract);
		void layoutChildren() override;

	protected:
		void onFocus() override;
		void onLeave() override;
		bool onHandleEvent(const sf::Event& ev) override;
		bool onUpdate(const float dt) override;
		void onRender(sf::RenderTarget& target) override;

	private:
		void autoSize();
		void setStartingCursorPosition();
		void moveCursorDown();
		void moveCursorUp();

	private:
		int _cursorPosition = -1;
		bool _canInteract = false;
	};

	class ScrollingList : public List
	{
	public:
		ScrollingList(bool canInteract);
		void layoutChildren() override;
	};

	// Container where items added will be organized into an auto-sizing grid with a width & height.
	class Grid : public Container
	{
	public:
		Grid(int columns, int rows);
		void layoutChildren() override;
		void setStartingCursorPosition();

	protected:
		void onFocus() override;
		void onLeave() override;
		bool onHandleEvent(const sf::Event& ev) override;
		bool onUpdate(const float dt) override;
		void onRender(sf::RenderTarget& target) override;

	private:
		void autoSize();
		void moveCursorDown();
		void moveCursorUp();
		void moveCursorRight();
		void moveCursorLeft();

	private:
		bool _isInitialized = false;
		int _numColumns;
		int _numRows;
		sf::Vector2i _cursorPosition = { -1, -1 };
	};

	// Element with a shape and text
	class Panel : public Element
	{
	protected:
		bool onUpdate(const float dt) override;
		void onRender(sf::RenderTarget& target) override;
	};

	// Element that fits its shape to its text
	class Label : public Element
	{
	public:
		sf::FloatRect getGlobalBounds() const override;
		sf::FloatRect getLocalBounds() const override;

	protected:
		bool onUpdate(const float dt) override;
		void onRender(sf::RenderTarget& target) override;
	};

	class PopupMessage : public Label
	{
	public:
		bool onHandleEvent(const sf::Event& ev) override;
	};

	// Element that does nothing special... yet?
	class Button : public Element
	{
	protected:
		bool onUpdate(const float dt) override;
		void onRender(sf::RenderTarget& target) override;
	};

	// Element that displays a sprite instead of a shape.
	class Icon : public Element
	{
	public:
		Icon(sf::Sprite sprite);
		void init() override;

	protected:
		bool onUpdate(const float dt) override;
		void onRender(sf::RenderTarget& target) override;

	private:
		sf::Sprite _sprite;
	};
}


