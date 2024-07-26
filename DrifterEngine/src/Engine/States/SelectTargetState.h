#pragma once
#include "Engine/States/State.h"
#include "GUI/GUIElement.h"

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
		bool update(const float dt) override;
		void render(sf::RenderTarget& target) override;

		void onPush() override;
		void onPop() override;

	private:
		bool isInRange() const;
		void moveCursor(sf::Vector2i direction);
		void renderTargetRadius(sf::RenderTarget& target);
		void renderTargetAoE(sf::RenderTarget& target);

	private:
		component::action::SelectTarget* _targetSelect = nullptr;
		sf::Vector2i _cursorPosition = { 0,0 };
		sf::Vector2i _cursorDelta = { 0,0 };
		sf::Vector2i _startPosition = { 0,0 };
		gui::Label _displayText;
		entt::entity _cursor = entt::null;
		std::vector<entt::entity> _radiusEffects;
		std::vector<entt::entity> _aoeEffects;
	};
}

