#include "pch.h"
#include "AbilityScreenState.h"
#include <Components/Actions/OpenAbilitiesScreenAction.h>
#include <Components/AbilitiesComponent.h>
#include <Ability/AbilityRegistry.h>

#include <Defines/CommonGuiColors.h>
#include <Utility/TGUIHelpers.h>

static const char* w_EntryIcon = "icon";

namespace drft
{
	AbilityScreenState::AbilityScreenState(StateStack& stack, StateContext& context)
		: State(stack, context)
	{
		determineSessionEntity();

		auto background = tgui::Panel::create();
		background->setOrigin(0.5f, 0.5f);
		background->setPosition("50%, 50%");
		background->setSize(tgui::bindSize(_guiGroup) * 0.5f);
		background->getRenderer()->setBackgroundColor(guiColor::BlackAgate);
		_guiGroup->add(background);

		auto wrap = tgui::HorizontalWrap::create();
		wrap->setOrigin(0.5f, 0.5f);
		wrap->setPosition(tgui::bindPosition(background));
		wrap->setSize(tgui::bindSize(background));
		wrap->getRenderer()->setPadding({ 8 });
		wrap->getRenderer()->setSpaceBetweenWidgets(16);
		_guiGroup->add(wrap);

		auto abilitiesTitle = tgui::Label::create();
		abilitiesTitle->setSize({ tgui::bindWidth(wrap), 32 });
		abilitiesTitle->setOrigin(0.5f, 1.f);
		abilitiesTitle->setPosition(tgui::bindPosX(wrap), tgui::bindTop(wrap) - 8);
		abilitiesTitle->setText("Abilities");
		abilitiesTitle->setTextSize(20);
		abilitiesTitle->setVerticalAlignment(tgui::VerticalAlignment::Bottom);
		abilitiesTitle->setHorizontalAlignment(tgui::HorizontalAlignment::Center);
		abilitiesTitle->getRenderer()->setBackgroundColor(guiColor::BlackAgate);
		_guiGroup->add(abilitiesTitle);

		_templateAbilityIcon = tgui::Panel::create();
		setupTemplateAbilityIcon(_templateAbilityIcon);

		refreshAbilitiesIcons(wrap);
	}

	void AbilityScreenState::determineSessionEntity()
	{
		auto& registry = getContext().registry;
		auto view = registry.view<OpenAbilitiesScreenAction>();
		if (!view.empty())
		{
			_sessionEntity = { registry, view.front() };
			_sessionEntity.remove<OpenAbilitiesScreenAction>();
		}
	}

	void AbilityScreenState::setupTemplateAbilityIcon(tgui::Panel::Ptr temp)
	{
		temp->setSize({ 64, 64 });
		temp->getRenderer()->setBorderColor(guiColor::TranslucentAsh);
		temp->getRenderer()->setBorders(1);

		auto icon = tgui::Picture::create();
		icon->setSize(tgui::bindSize(temp));
		temp->add(icon, w_EntryIcon);
	}

	void AbilityScreenState::refreshAbilitiesIcons(tgui::HorizontalWrap::Ptr wrap)
	{
		if (auto abilitiesComponent = _sessionEntity.try_get<AbilitiesComponent>())
		{
			TextureAtlas& textures = getContext().registry.ctx().get<TextureAtlas>();
			for (auto&& abilityId : abilitiesComponent->abilities)
			{
				const auto& ability = AbilityRegistry::get(abilityId);
				auto templateCopy = tgui::Panel::copy(_templateAbilityIcon);
				std::string entryName = std::to_string(abilityId);

				// Set Icon
				auto iconData = ability.getIconData();
				auto uv = textures.getUV(iconData.textureId, iconData.uvSize, iconData.uv);
				auto texture = GuiHelpers::createTGUITextureFromUV(entryName, uv);
				texture.setColor(iconData.color);

				auto icon = templateCopy->get<tgui::Picture>(w_EntryIcon);	
				icon->getRenderer()->setTexture(texture);

				wrap->add(templateCopy, entryName);
			}
		}
	}

	bool AbilityScreenState::handleEvent(const sf::Event& ev)
	{
		if (const auto keypressed = ev.getIf<sf::Event::KeyPressed>())
		{
			if (keypressed->code == sf::Keyboard::Key::Escape)
			{
				requestStackPop();
				return true;
			}
		}
		return false;
	}
}