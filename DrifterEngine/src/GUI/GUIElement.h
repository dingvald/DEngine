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
		Idle,
		Focused,
		Active,
		Total
	};

	struct Style
	{
		sf::Color outlineColor;
		sf::Color fillColor;
	};

	class Element
	{
	public:
		using ElementPtr = std::unique_ptr<Element>;

		void onSelect()
		{
			_state = ElementState::Active;
			_callback();
		}
		void onFocus()
		{
			_state = ElementState::Focused;
		}
		void onLeave()
		{
			_state = ElementState::Idle;
		}

		void setPosition(sf::Vector2f position) { _shape.setPosition(position); }
		void setSize(sf::Vector2f size) { _shape.setSize(size); }
		void setStyle(ElementState state, Style&& style) { _style[state] = style; }
		
		void registerCallback(std::function<void()> callback)
		{
			_callback = callback;
		}
		
		void insert(ElementPtr childElement)
		{
			_children.emplace_back(std::move(childElement));
		}

		virtual bool handleEvent(const sf::Event& ev) = 0;
		virtual bool update(const float dt) = 0;
		virtual void render(sf::RenderTarget& target) = 0;

	protected:
		std::function<void()> _callback = {};
		sf::RectangleShape _shape;
		std::vector<ElementPtr> _children;
		ElementState _state;
		std::map<ElementState, Style> _style;
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
		int _cursorIndex = 0;
	};


	class Button : public Element
	{
	public:


	};


}


