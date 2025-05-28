#pragma once
#include <States/State.h>
#include <Components/Wrappers/SkillsWrapper.h>
#include <SFML/Graphics/Sprite.hpp>
#include <optional>
#include <GUI/AbilityTooltip.h>

namespace drft
{
	class IAbility;

	class SkillsScreenState : public State
	{
	public:
		SkillsScreenState(StateStack& stack, StateContext& context);

		bool handleEvent(const sf::Event& ev) override;
		bool update() override;
		void guiRender(sf::RenderTarget& target) override;

	private:
		void determineSessionEntity();
		void setupPanelTemplate(tgui::Panel::Ptr templatePanel);
		void refreshSkillsList(tgui::PanelListBox::Ptr skillsList);
		
		void setupTemplateAbilityIcon(tgui::Panel::Ptr temp);
		void refreshAbilities(tgui::HorizontalWrap::Ptr abilities, entt::id_type skillId);

		void onMousePress();
		void onEnterAbilityContainingWidget(const IAbility& ability);
		void onExitAbilityContainingWidget();

		// Dragging ability icon support
		struct DraggingAbility {
			entt::id_type abilityId;
			std::unique_ptr<sf::Sprite> icon;
			sf::RectangleShape background;
			bool isClickHandled = false;

			DraggingAbility(entt::id_type id, const sf::Sprite& sprite, const sf::Color& color);
			void render(sf::RenderTarget& target) const;
			void setPosition(sf::Vector2i pos);
		};

		std::optional<DraggingAbility> createDraggedAbility(entt::id_type abilityId) const;

	private:
		SkillsWrapper _skills;
		entt::handle _sessionEntity;

		tgui::Panel::Ptr _templateAbilityIcon;
		std::optional<DraggingAbility> _draggingAbility;
		std::optional<AbilityTooltip> _abilityTooltip;
	};
}