#pragma once
#include <States/State.h>
#include "GUI/GUIElement.h"
#include <Spatial/TilePosition.h>

namespace component::action
{
	struct SelectTarget;
}

namespace drft
{
	class SelectTargetState : public State
	{
	public:
		SelectTargetState(StateStack& stack, StateContext& context);
		bool handleEvent(const sf::Event& ev) override;
		bool update() override;
		void render(sf::RenderTarget& target) override;

		void onPush() override;
		void onPop() override;

	private:
		bool isInRange() const;
		void moveCursor(sf::Vector2i direction);

	private:
		component::action::SelectTarget* _targetSelect = nullptr;
		TilePosition _cursorPosition = { 0,0,0 };
		TilePosition _startPosition = { 0,0,0 };
		gui::Label _displayText;
		entt::entity _cursor = entt::null;
		std::vector<entt::entity> _radiusEffects;
		std::vector<entt::entity> _aoeEffects;
	};
}

