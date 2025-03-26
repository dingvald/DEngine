#include "pch.h"
#include "HUD.h"
#include "Spatial/Conversions.h"
#include "Spatial/WorldGrid.h"
#include "Utility/EntityHelpers.h"
#include "Utility/GetTextCenter.h"
#include "Utility/SmoothTransition.h"
#include "Systems/Helpers/ItemDatabase.h"
#include "Systems/Helpers/FindItemOwner.h"
#include "Systems/Helpers/GetCurrentCamera.h"
#include "Systems/Helpers/ToHotbarIndex.h"
#include <Systems/Helpers/GetPlayerHandle.h>

#include "Components/DescriptionComponent.h"
#include "Components/PositionComponent.h"
#include "Components/PlayerInputComponent.h"
#include "Components/RenderComponent.h"
#include "Components/HealthComponent.h"
#include "Components/LevelingComponent.h"
#include "Components/StaminaComponent.h"
#include "Components/HotbarComponent.h"
#include <Components/Actions/HotbarAction.h>

#include <Components/Components.h>

#include <Defines/CommonGuiColors.h>

#include <Systems/Core/MouseVisualizationSystem.h>
#include <Events/HUDHotbarPressedEvent.h>
#include <Events/ChangeHUDEnabledEvent.h>

#include "Ability/AbilityRegistry.h"

#include "Utility/TextureAtlas.h"
#include <Utility/TGUIHelpers.h>

static const char* HotbarWidgetId = "hotbar";

static const char* HealthBarFillWidgetId = "health bar fill";
static const char* HealthBarContainerWidgetId = "health bar container";

static const char* StaminaBarFillWidgetId = "stamina bar fill";
static const char* StaminaBarContainerWidgetId = "stamina bar container";

namespace
{
	void setOverlayColor(tgui::Group::Ptr group, sf::Color color)
	{
		auto overlay = group->get<tgui::Panel>("overlay");
		overlay->getRenderer()->setBackgroundColor(color);
	}
}

void drft::system::HUD::setVisible(entt::registry& registry, bool shouldBeVisible)
{
	registry.ctx().get<entt::dispatcher>().trigger(events::ChangeHUDEnabledEvent{ .shouldShow = shouldBeVisible });
}

void drft::system::HUD::setEnabled(entt::registry& registry, bool shouldBeEnabled)
{
	registry.ctx().get<entt::dispatcher>().trigger(events::ChangeHUDEnabledEvent{ .shouldEnable = shouldBeEnabled });
}

void drft::system::HUD::init()
{
	auto& gui = _registry.ctx().get<tgui::Gui>();
	_gui = tgui::Group::create();
	gui.add(_gui);
	_gui->setVisible(true);
	_gui->onMouseEnter([this]() { MouseVisualizationSystem::changeMouseVisibility(_registry, { false, false }); });
	_gui->onMouseLeave([this]() { MouseVisualizationSystem::changeMouseVisibility(_registry, { true, true }); });

	createLevelInfo();
	createHealthBar();
	createStaminaBar();
	createInHandsDisplay();
	createItemsOnGroundDisplay();
	createHotbar();

	_registry.on_construct<component::action::TakeDamage>().connect<&HUD::onTakeDamage>(this);
	_registry.on_construct<component::action::ConsumeStamina>().connect<&HUD::onConsumeStamina>(this);
	_registry.on_construct<HotbarAction>().connect<&HUD::onHotbarPressed>(this);

	_dispatcher.sink<events::ChangeHUDEnabledEvent>().connect<&HUD::onChangeHUDEnabledEvent>(this);
}

void drft::system::HUD::update()
{
	auto player = getPlayerConstHandle(_registry);

	// Player relevant displays
	updateLevelInfo(player);
	updateHealthBar(player);
	updateStaminaBar(player);
	updateInHandsDisplay(player);
	updateItemsOnGround(player);
	updateHotbar(player);

	updateFlashEffects();
}

void drft::system::HUD::shutdown()
{
	auto& gui = _registry.ctx().get<tgui::Gui>();
	gui.remove(_gui);
}

void drft::system::HUD::createLevelInfo()
{

}

void drft::system::HUD::createHealthBar()
{
	auto label = tgui::Label::create();
	label->setPosition(16, 24);
	label->setText("HP");
	label->setTextSize(20);
	label->setOrigin(0.f, 0.5f);
	label->setWidth(32);
	label->setVerticalAlignment(tgui::VerticalAlignment::Center);
	label->getRenderer()->setBackgroundColor(guiColor::BlackAgate);
	_gui->add(label);

	auto container = tgui::Panel::create();
	container->setSize({ 1, 14 });
	container->setPosition(tgui::bindRight(label), tgui::bindPosY(label));
	container->setOrigin(0.f, 0.5f);
	container->getRenderer()->setBackgroundColor(guiColor::BlackAgate);
	_gui->add(container, HealthBarContainerWidgetId);

	auto fill = tgui::Panel::create();
	fill->setPosition(tgui::bindPosition(container) + tgui::Layout2d{1, 0});
	fill->setOrigin(0.f, 0.5f);
	fill->setSize({tgui::bindWidth(container) - 2, tgui::bindHeight(container) - 2});
	fill->getRenderer()->setBackgroundColor(guiColor::MGSHudBlue);
	_gui->add(fill, HealthBarFillWidgetId);
}

void drft::system::HUD::createStaminaBar()
{
	auto label = tgui::Label::create();
	label->setPosition(16, 48);
	label->setText("ST");
	label->setTextSize(20);
	label->setOrigin(0.f, 0.5f);
	label->setWidth(32);
	label->setVerticalAlignment(tgui::VerticalAlignment::Center);
	label->getRenderer()->setBackgroundColor(guiColor::BlackAgate);
	_gui->add(label);

	auto container = tgui::Panel::create();
	container->setSize({ 1, 14 });
	container->setPosition(tgui::bindRight(label), tgui::bindPosY(label));
	container->setOrigin(0.f, 0.5f);
	container->getRenderer()->setBackgroundColor(guiColor::BlackAgate);
	_gui->add(container, StaminaBarContainerWidgetId);

	auto fill = tgui::Panel::create();
	fill->setPosition(tgui::bindPosition(container) + tgui::Layout2d{ 1, 0 });
	fill->setOrigin(0.f, 0.5f);
	fill->setSize({ tgui::bindWidth(container) - 2, tgui::bindHeight(container) - 2 });
	fill->getRenderer()->setBackgroundColor(guiColor::StaminaGreen);
	_gui->add(fill, StaminaBarFillWidgetId);
}

void drft::system::HUD::createInHandsDisplay()
{

}

void drft::system::HUD::createItemsOnGroundDisplay()
{

}

void drft::system::HUD::createHotbar()
{
	// Template icon setup
	_templateHotbarIcon = tgui::Group::create();
	_templateHotbarIcon->setSize(64, 64);

	auto button = tgui::Button::create();
	button->setSize(tgui::bindSize(_templateHotbarIcon));
	button->getRenderer()->setBorderColor(guiColor::TranslucentAsh);
	button->getRenderer()->setBorders({ 2, 2 });
	button->getRenderer()->setBorderColorHover(tgui::Color{ 255, 255, 255, 100 });
	button->getRenderer()->setTextColorFocused(button->getRenderer()->getTextColor());

	_templateHotbarIcon->add(button, "button");

	auto overlay = tgui::Panel::create();
	overlay->setSize(tgui::bindSize(button));
	overlay->getRenderer()->setBackgroundColor(tgui::Color::Transparent);
	overlay->setIgnoreMouseEvents(true);

	_templateHotbarIcon->add(overlay, "overlay");
	///

	auto hotbar = tgui::GrowHorizontalLayout::create();
	hotbar->setHeight(64);
	hotbar->setOrigin(0.5f, 0.5f);
	hotbar->setPosition("50%", "95%");
	hotbar->getRenderer()->setSpaceBetweenWidgets(12);

	auto hotbarBackground = tgui::Panel::create();
	hotbarBackground->setOrigin(0.5f, 0.5f);
	hotbarBackground->setSize(tgui::bindSize(hotbar));
	hotbarBackground->setPosition(tgui::bindPosition(hotbar));
	hotbarBackground->getRenderer()->setBackgroundColor(guiColor::BlackAgate);

	_gui->add(hotbarBackground);
	_gui->add(hotbar, HotbarWidgetId);

	for (size_t i = 0; i < HOTBAR_SIZE; i++)
	{
		auto newGroup = tgui::Group::copy(_templateHotbarIcon);
		hotbar->add(newGroup, std::format("index_{}", i));

		auto newButton = newGroup->get<tgui::Button>("button");
		newButton->setText(tgui::String::fromNumber(toHotbarIndex(i)));
		newButton->setTextSize(12);
		newButton->setTextPosition("10%, 10%", { 0.5f, 0.5f });
		newButton->onPress([this, i]() { _dispatcher.trigger(events::HUDHotbarPressedEvent{ i }); });

		auto overlay = newGroup->get<tgui::Panel>("overlay");
		overlay->setIgnoreMouseEvents(true);
	}
}

void drft::system::HUD::updateLevelInfo(entt::const_handle player)
{
	if (auto level = player.try_get<LevelingComponent>())
	{

	}
}

void drft::system::HUD::updateHealthBar(entt::const_handle player)
{
	if (auto health = player.try_get<HealthComponent>())
	{
		auto container = _gui->get<tgui::Panel>(HealthBarContainerWidgetId);
		container->setWidth(24 * health->max);

		const float ratio = health->current / health->max;

		auto fill = _gui->get<tgui::Panel>(HealthBarFillWidgetId);
		fill->setSize({ (tgui::bindWidth(container) - 2) * ratio, tgui::bindHeight(container) - 2 });
	}
	else
	{

	}
}

void drft::system::HUD::updateStaminaBar(entt::const_handle player)
{
	if (auto stamina = player.try_get<StaminaComponent>())
	{
		auto container = _gui->get<tgui::Panel>(StaminaBarContainerWidgetId);
		container->setWidth(24 * stamina->max);

		const float ratio = stamina->current / stamina->max;

		auto fill = _gui->get<tgui::Panel>(StaminaBarFillWidgetId);
		fill->setSize({ (tgui::bindWidth(container) - 2) * ratio, tgui::bindHeight(container) - 2 });
	}
	else
	{

	}
}

void drft::system::HUD::updateItemsOnGround(entt::const_handle player)
{
	using namespace entt::literals;
	const auto& grid = _registry.ctx().get<spatial::WorldGrid&>();

	if (auto pos = player.try_get<PositionComponent>())
	{
		auto entities = grid.entitiesAt(pos->tile,
			[this](entt::entity entity) -> bool
			{
				if (_registry.all_of<DescriptionComponent>(entity)
				&& !_registry.any_of<PlayerInputComponent>(entity))
				{
					return true;
				}
				return false;
			});
		int count = 0;
		for (auto entity : entities)
		{

		}
	}
}

void drft::system::HUD::updateInHandsDisplay(entt::const_handle player)
{

}

void drft::system::HUD::updateFlashEffects()
{
	auto it = _flashEffects.begin();
	while (it != _flashEffects.end())
	{
		--(it->ttl);
		if (it->fades)
		{
			sf::Color color = it->shape->getRenderer()->getBackgroundColor();
			color.a = math::remap(0, 120, 0, 255, it->ttl);
			it->shape->getRenderer()->setBackgroundColor(color);
		}
		if (it->ttl <= 0)
		{
			_gui->remove(it->shape);
			it = _flashEffects.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void drft::system::HUD::updateHotbar(entt::const_handle player)
{
	if (auto hotbarComponent = player.try_get<HotbarComponent>())
	{
		TextureAtlas& textures = _registry.ctx().get<TextureAtlas>();

		auto hotbar = _gui->get<tgui::GrowHorizontalLayout>(HotbarWidgetId);
		for (size_t i = 0; i < HOTBAR_SIZE; i++)
		{
			const std::string groupName = std::format("index_{}", i);
			auto group = hotbar->get<tgui::Group>(groupName);

			auto abilityType = hotbarComponent->abilities.at(i);
			const IAbility& ability = AbilityRegistry::get(abilityType);

			// Set Icon
			auto icon = ability.getIconData();
			auto uv = textures.getUV(icon.textureId, icon.uvSize, icon.uv);	
			auto texture = GuiHelpers::createTGUITextureFromUV(groupName, uv);

			sf::Color iconColor = ability.isValid(player) ? icon.color : sf::Color{ 150, 150, 150, 100 };
			texture.setColor(iconColor);

			auto button = group->get<tgui::Button>("button");
			button->getRenderer()->setTexture(texture);

			sf::Color overlayColor = ability.isToggledOn(player) ? sf::Color{ 255, 255, 0, 100 } : sf::Color::Transparent;
			setOverlayColor(group, overlayColor);
		}
	}
}

void drft::system::HUD::queueFlashEffect(sf::Vector2f position, sf::Vector2f size, int ttl, bool fades /*=false*/)
{
	auto shape = tgui::Panel::create();
	shape->setPosition(position.x, position.y);
	shape->setSize(size.x, size.y);
	shape->getRenderer()->setBackgroundColor(tgui::Color::White);
	shape->setIgnoreMouseEvents(true);

	_flashEffects.emplace_back(shape, fades, ttl);
	_gui->add(shape);
}

void drft::system::HUD::onHotbarPressed(entt::registry& registry, entt::entity entity)
{
	auto& hotbarAction = registry.get<HotbarAction>(entity);

	auto hotbar = _gui->get<tgui::GrowHorizontalLayout>(HotbarWidgetId);
	auto group = hotbar->get<tgui::Group>(std::format("index_{}", hotbarAction.index));

	sf::Vector2f position = group->getAbsolutePosition();
	sf::Vector2f size = group->getSize();

	queueFlashEffect(position, size, 45, true);
}

void drft::system::HUD::onTakeDamage(entt::registry& registry, entt::entity entity)
{
	if (!registry.all_of<PlayerInputComponent>(entity)) return;
	
}

void drft::system::HUD::onConsumeStamina(entt::registry& registry, entt::entity entity)
{
	if (!registry.all_of<PlayerInputComponent>(entity)) return;

}

void drft::system::HUD::onChangeHUDEnabledEvent(const events::ChangeHUDEnabledEvent& ev)
{
	_gui->setEnabled(ev.shouldEnable.value_or(_gui->isEnabled()));
	_gui->setVisible(ev.shouldShow.value_or(_gui->isVisible()));
}
