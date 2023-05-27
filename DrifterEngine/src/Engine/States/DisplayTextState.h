#pragma once
#include "State.h"
#include "GUI/GUIElement.h"
#include "Utility/SpriteBatch.h"

namespace drft
{
	enum class DisplayType
	{
		Map,
		Text
	};

	class DisplayTextState : public State
	{
	public:
		DisplayTextState(StateStack& stack, StateContext& context);

		bool handleEvent(const sf::Event& ev) override;
		bool update(const float dt) override;
		void render(sf::RenderTarget& target) override;
		void onPush() override;
		void onPop() override;

	private:
		void getSessionParameters();
		void determineDisplayType();
		void buildDisplay();
		void buildMap();
		void buildText();

	private:
		gui::Panel _background;
		gui::SingleContainer _display;
		DisplayType _displayType = DisplayType::Text;
		std::unordered_map<std::string, std::string> _sessionParameters;
		SpriteBatch _sprites;
	};
}