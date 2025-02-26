#include "pch.h"
#include "HUD.h"
#include "Spatial/Conversions.h"
#include "Spatial/WorldGrid.h"
#include "Utility/SpriteIndexer.h"
#include "Utility/EntityHelpers.h"
#include "Utility/GetTextCenter.h"
#include "Utility/SmoothTransition.h"
#include "Systems/Helpers/ItemDatabase.h"
#include "Systems/Helpers/FindItemOwner.h"
#include "Systems/Helpers/GetCurrentCamera.h"
#include "Systems/Helpers/ToHotbarIndex.h"
#include "Components/Components.h"

#include "Components/DescriptionComponent.h"
#include "Components/PositionComponent.h"
#include "Components/PlayerInputComponent.h"
#include "Components/RenderComponent.h"
#include "Components/HealthComponent.h"
#include "Components/LevelingComponent.h"
#include "Components/StaminaComponent.h"
#include "Components/HotbarComponent.h"

#include "Ability/AbilityRegistry.h"

#include "Utility/TextureAtlas.h"

static const sf::Vector2f HEALTHBAR_POSITION = { 32.f, 32.f };
static constexpr float HEALTHBAR_HEIGHT = 7;
static constexpr int HEALTHBAR_WIDTH_MULTIPLIER = 7;

static const sf::Vector2f STAMINABAR_POSITION = HEALTHBAR_POSITION + sf::Vector2f(0.f, 16.f);
static constexpr float STAMINABAR_HEIGHT = HEALTHBAR_HEIGHT;
static constexpr int STAMINABAR_WIDTH_MULTIPLIER = HEALTHBAR_WIDTH_MULTIPLIER;

static constexpr float HOTBAR_ICON_X_OFFSET = 48.f;

void drft::system::HUD::init()
{
	createLevelInfo();
	createHealthBar();
	createStaminaBar();
	createInHandsDisplay();
	createItemsOnGroundDisplay();
	createHotbar();

	_registry.on_construct<component::action::TakeDamage>().connect<&HUD::onTakeDamage>(this);
	_registry.on_construct<component::action::ConsumeStamina>().connect<&HUD::onConsumeStamina>(this);
	_registry.on_construct<component::action::HotbarPressed>().connect<&HUD::onHotbarPressed>(this);
}

void drft::system::HUD::update()
{
	auto view = _registry.view<PlayerInputComponent>();
	auto player = entt::handle(_registry, view.front());

	// Player relevant displays
	updateLevelInfo(player);
	updateHealthBar(player);
	updateStaminaBar(player);
	updateInHandsDisplay(player);
	updateItemsOnGround(player);
	updateHotbar(player);

	// Effects TODO: refactor into an effect system
	updateFlashEffects();
}

void drft::system::HUD::render(sf::RenderTarget& target)
{	
	for (auto&& effect : _flashEffects)
	{
		target.draw(effect.shape);
	}
}

void drft::system::HUD::createLevelInfo()
{

}

void drft::system::HUD::createHealthBar()
{

}

void drft::system::HUD::createStaminaBar()
{

}

void drft::system::HUD::createInHandsDisplay()
{

}

void drft::system::HUD::createItemsOnGroundDisplay()
{

}

void drft::system::HUD::createHotbar()
{

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

	}
	else
	{

	}
}

void drft::system::HUD::updateStaminaBar(entt::const_handle player)
{
	if (auto stamina = player.try_get<StaminaComponent>())
	{

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
			sf::Color color = it->shape.getFillColor();
			color.a = math::remap(0, 120, 0, 255, it->ttl);
			it->shape.setFillColor(color);
		}
		if (it->ttl <= 0)
		{
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

}

void drft::system::HUD::queueFlashEffect(sf::Vector2f position, sf::Vector2f size, int ttl, bool fades /*=false*/)
{
	sf::RectangleShape shape;
	shape.setPosition(position);
	shape.setSize(size);
	shape.setFillColor(sf::Color::White);

	_flashEffects.emplace_back(shape, fades, ttl);
}

void drft::system::HUD::onHotbarPressed(entt::registry& registry, entt::entity entity)
{

}

void drft::system::HUD::onTakeDamage(entt::registry& registry, entt::entity entity)
{
	if (!registry.all_of<PlayerInputComponent>(entity)) return;

	if (auto health = registry.try_get<HealthComponent>(entity))
	{
		auto& damage = registry.get<component::action::TakeDamage>(entity);
		if (damage.amount != 0)
		{
			sf::Vector2f size = { (health->current / health->max)
			* (health->max * HEALTHBAR_WIDTH_MULTIPLIER) - 2.0f, HEALTHBAR_HEIGHT + 2.f };
			queueFlashEffect(HEALTHBAR_POSITION, size, 10);
		}
	}
}

void drft::system::HUD::onConsumeStamina(entt::registry& registry, entt::entity entity)
{
	if (!registry.all_of<PlayerInputComponent>(entity)) return;

	if (auto stamina = registry.try_get<StaminaComponent>(entity))
	{
		auto& consume = registry.get<component::action::ConsumeStamina>(entity);
		if (stamina->baseConsumption + consume.amount > 0.f)
		{
			sf::Vector2f size = { (static_cast<float>(stamina->current) / static_cast<float>(stamina->max))
			* static_cast<float>(stamina->max * STAMINABAR_WIDTH_MULTIPLIER) - 2.0f, STAMINABAR_HEIGHT + 2.f };
			queueFlashEffect(STAMINABAR_POSITION, size, 10);
		}
	}
}
