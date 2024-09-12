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
#include "Components/PlayerComponent.h"
#include "Components/RenderComponent.h"
#include "Components/HealthComponent.h"
#include "Components/LevelingComponent.h"
#include "Components/BodyComponent.h"
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

void drft::system::HUD::onFixedUpdate()
{
	auto view = _registry.view<PlayerComponent>();
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
	target.draw(_lvlText);
	target.draw(_xpText);

	target.draw(_healthBarContainer);
	target.draw(_healthBar);
	target.draw(_heartIcon);

	target.draw(_staminaBarContainer);
	target.draw(_staminaBar);
	target.draw(_staminaIcon);

	_inHandsDisplay.render(target);
	_itemsOnGround.render(target);
	_hotbar.render(target);
	
	for (auto&& effect : _flashEffects)
	{
		target.draw(effect.shape);
	}
}

void drft::system::HUD::createLevelInfo()
{
	using namespace entt::literals;
	const auto& font = _registry.ctx().get<sf::Font&>("terminus"_hs);

	_lvlText.setFont(font);
	_xpText.setFont(font);

	_lvlText.setPosition(HEALTHBAR_POSITION - sf::Vector2f(0.f, 24.f));
	_lvlText.setString("Level: ");
	_lvlText.setCharacterSize(16);

	_xpText.setPosition(HEALTHBAR_POSITION - sf::Vector2f(-64.f, 24.f));
	_xpText.setString("xp: 0/100 ");
	_xpText.setCharacterSize(16);
}

void drft::system::HUD::createHealthBar()
{
	using namespace entt::literals;
	const auto& textureAtlas = _registry.ctx().get<TextureAtlas&>();

	_heartIcon = textureAtlas.getSprite("simple_tileset"_hs, { 16, 16 }, { 5, 1 });
	_heartIcon.setPosition(HEALTHBAR_POSITION - sf::Vector2f(20.f, 4.f));
	_heartIcon.setColor(sf::Color(150, 60, 60, 200));

	_healthBarContainer.setSize({ 1.f, HEALTHBAR_HEIGHT + 2.f });
	_healthBarContainer.setPosition(HEALTHBAR_POSITION);
	_healthBarContainer.setFillColor(sf::Color(180, 180, 180, 128));

	_healthBar.setSize({ 1.f, HEALTHBAR_HEIGHT });
	_healthBar.setPosition(HEALTHBAR_POSITION + sf::Vector2f{ 1.f, 1.f });
	_healthBar.setFillColor(sf::Color(150, 60, 60, 200));
}

void drft::system::HUD::createStaminaBar()
{
	using namespace entt::literals;
	const auto& textureAtlas = _registry.ctx().get<TextureAtlas&>();

	_staminaIcon = textureAtlas.getSprite("simple_tileset"_hs, { 16, 16 }, { 8, 1 });
	_staminaIcon.setPosition(STAMINABAR_POSITION - sf::Vector2f(20.f, 4.f));
	_staminaIcon.setColor(sf::Color(60, 150, 60, 200));

	_staminaBarContainer.setSize({ 1.f, STAMINABAR_HEIGHT + 2.f });
	_staminaBarContainer.setPosition(STAMINABAR_POSITION);
	_staminaBarContainer.setFillColor(sf::Color(180, 180, 180, 128));

	_staminaBar.setSize({ 1.f, STAMINABAR_HEIGHT });
	_staminaBar.setPosition(STAMINABAR_POSITION + sf::Vector2f{ 1.f, 1.f });
	_staminaBar.setFillColor(sf::Color(60, 150, 60, 200));
}

void drft::system::HUD::createInHandsDisplay()
{
	_inHandsDisplay.setPosition(HEALTHBAR_POSITION + sf::Vector2f(16.f, 64.f))
		.setChildrenOrigin(gui::ElementPosition::CENTER_LEFT)
		.setStyle(gui::ElementState::Idle, {
			.childPadding = {48.f, 0.f}
			});
}

void drft::system::HUD::createItemsOnGroundDisplay()
{
	using namespace entt::literals;
	const auto& view = _registry.ctx().get<sf::RenderWindow&>().getView();
	_itemsOnGround.setPosition({ view.getCenter().x + (view.getSize().x / 2) - 80, view.getCenter().y + (view.getSize().y / 2) - 64 })
		.setStyle(gui::ElementState::Idle, {
			.fillColor = sf::Color(0,0,0,100),
			.outlineThickness = 1.f,
			.innerPadding = {0.f, 0.f},
			.childPadding = {0.f, 4.f},
			.font = &_registry.ctx().get<sf::Font&>("terminus"_hs),
			.textColor = sf::Color(200,200,200,200),
			.textSize = 16
			})
		.setChildrenOrigin(gui::ElementPosition::TOP_LEFT);
}

void drft::system::HUD::createHotbar()
{
	const auto& view = _registry.ctx().get<sf::RenderWindow&>().getView();
	const sf::Vector2f position = { view.getCenter().x - 208, view.getCenter().y + (view.getSize().y / 2) - 64 };

	_hotbar.setPosition(position)
		.setStyle(gui::ElementState::Idle, {
					.childPadding = {HOTBAR_ICON_X_OFFSET, 0.f}
			});
}

void drft::system::HUD::updateLevelInfo(entt::const_handle player)
{
	if (auto level = player.try_get<LevelingComponent>())
	{
		_lvlText.setString("Lvl " + std::to_string(level->currentLevel));
		_xpText.setString("XP " + std::to_string(level->currentXP) + "/" + std::to_string(level->neededXP));
	}
}

void drft::system::HUD::updateHealthBar(entt::const_handle player)
{
	if (auto health = player.try_get<HealthComponent>())
	{
		_healthBarContainer.setSize({health->max * HEALTHBAR_WIDTH_MULTIPLIER, HEALTHBAR_HEIGHT + 2.f });
		_healthBar.setSize({ (health->current / health->max)
			* (health->max * HEALTHBAR_WIDTH_MULTIPLIER) - 2.0f, HEALTHBAR_HEIGHT });
	}
	else
	{
		_healthBar.setSize({ 0.f, HEALTHBAR_HEIGHT });
	}
}

void drft::system::HUD::updateStaminaBar(entt::const_handle player)
{
	if (auto stamina = player.try_get<StaminaComponent>())
	{
		_staminaBarContainer.setSize({ stamina->max * STAMINABAR_WIDTH_MULTIPLIER, STAMINABAR_HEIGHT + 2.f });
		_staminaBar.setSize({ (stamina->current / stamina->max) * (stamina->max * STAMINABAR_WIDTH_MULTIPLIER) - 2.f, STAMINABAR_HEIGHT });
	}
	else
	{
		_staminaBar.setSize({ 0.f, HEALTHBAR_HEIGHT });
	}
}

void drft::system::HUD::updateItemsOnGround(entt::const_handle player)
{
	using namespace entt::literals;
	const auto& grid = _registry.ctx().get<spatial::WorldGrid&>();

	_itemsOnGround.clear();

	if (auto pos = player.try_get<PositionComponent>())
	{
		auto entities = grid.entitiesAt(pos->tile,
			[this](entt::entity entity) -> bool
			{
				if (_registry.all_of<DescriptionComponent>(entity)
				&& !_registry.any_of<PlayerComponent>(entity))
				{
					return true;
				}
				return false;
			});
		int count = 0;
		for (auto entity : entities)
		{
			_itemsOnGround.insert(std::to_string(count), gui::Label())
				.setStyle(gui::ElementState::Idle, {
					.font = &_registry.ctx().get<sf::Font&>("terminus"_hs),
					.textColor = sf::Color::White,
					.textSize = 16
					})
				.setOrigin(gui::ElementPosition::CENTER_LEFT)
				.setTextString(util::getEntityName({ _registry, entity }));
		}
	}
	
	_itemsOnGround.update(0.f);
}

void drft::system::HUD::updateInHandsDisplay(entt::const_handle player)
{
	_inHandsDisplay.clear();
	if (auto body = player.try_get<BodyComponent>())
	{
		auto handParts = body->parts.search(PartType::Hand);
		for (auto hand : handParts)
		{
			auto& handContainer = _inHandsDisplay.insert(std::string(hand->name), gui::DualContainer());
			handContainer.setSize({ 32, 32 })
				.setStyle(gui::ElementState::Idle, {
					.fillColor = {0,0,0,150},
					.outlineColor = {150,150,150,100},
					.outlineThickness = 1.f
					})
				.insert("Item", gui::DualContainer());

			auto heldItems = hand->getSlotItem(EquipmentLayer::Held);
			if (heldItems.has_value())
			{
				auto itemEntity = ItemDatabase::getEntityFromItemID(heldItems.value());
				addItemIcon(handContainer["Item"], itemEntity);
			}
		}
	}
	_inHandsDisplay.update(0.f);
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
	using namespace entt::literals;
	_hotbar.clear();
	if (auto hotbar = player.try_get<HotbarComponent>())
	{
		const int hotbarSize = hotbar->abilities.size();
		const auto& textureAtlas = _registry.ctx().get<TextureAtlas&>();
		for (int i = 0; i < hotbarSize; ++i) 
		{
			const auto& ability = AbilityRegistry::get(static_cast<AbilityType>(hotbar->abilities[i]));
			const bool isValidAbility = ability.isValid(player);
			const bool isToggledOn = ability.isToggledOn(player);
			auto& hotbarContainer = _hotbar.insert(std::to_string(i), gui::DualContainer());
			hotbarContainer.setSize({ 32,32 });
			hotbarContainer.setStyle(gui::ElementState::Idle, {
					.fillColor = isValidAbility ? sf::Color(0,0,0,60) : sf::Color(100,100,100,60),
					.outlineColor = isValidAbility ? (isToggledOn ? sf::Color::Yellow : sf::Color(255,255,255,50)) : sf::Color(100,100,100,100),
					.outlineThickness = 1.f,
					.innerPadding = {0.f, 0.f}
				});
			hotbarContainer.setOrigin(gui::ElementPosition::CENTER);

			const auto textureUV = ability.getTextureUV();
			sf::Sprite abilitySprite = textureAtlas.getSprite("icons"_hs, { textureUV.width, textureUV.height }, { textureUV.left, textureUV.top });
			auto& abilityIconGUI = hotbarContainer.insert("icon", gui::Icon{ abilitySprite });
			abilityIconGUI.setSize({ 32,32 });
			abilityIconGUI.setStyle(gui::ElementState::Idle, {
					.fillColor = isValidAbility ? ability.getIconColor() : sf::Color(100,100,100,100)
				});

			hotbarContainer.insert("Slot Abbrev", gui::Label())
				.setLocalPosition({ -8, -8 })
				.setStyle(gui::ElementState::Idle, {
					.font = &_registry.ctx().get<sf::Font&>("terminus"_hs),
					.textColor = sf::Color::White
					})
				.setStyle(gui::ElementState::Focused, {
					.font = &_registry.ctx().get<sf::Font&>("terminus"_hs),
					.textColor = sf::Color::White
					})
				.setTextString(std::to_string((i+1)%10));
		}
	}
	_hotbar.update(0.f);
}

void drft::system::HUD::addItemIcon(gui::Element& container, entt::entity item)
{
	using namespace entt::literals;
	const auto& itemRender = _registry.get<RenderComponent>(item);
	const auto& textureAtlas = _registry.ctx().get<TextureAtlas&>();

	sf::Sprite sprite = textureAtlas.getSprite(itemRender.texture, itemRender.uvSize, itemRender.uvCoords);
	container.insert("Icon", gui::Icon(sprite))
		.setSize({ 32,32 })
		.setOrigin(gui::ElementPosition::BOTTOM_RIGHT)
		.setStyle(gui::ElementState::Idle, {
				.fillColor = itemRender.color
			})
		.setStyle(gui::ElementState::Focused, {
					.fillColor = itemRender.color
			});

	if (auto health = _registry.try_get<HealthComponent>(item))
	{
		float scalingFactor = health->current / health->max;
		container.insert("Health", gui::Panel())
			.setSize({ 32, (32 - 32 * scalingFactor) })
			.setLocalPosition({ 0, 16 })
			.setOrigin(gui::ElementPosition::BOTTOM_CENTER)
			.setStyle(gui::ElementState::Idle, {
				.fillColor = sf::Color(255,0,0,60)
				})
			.setStyle(gui::ElementState::Focused, {
				.fillColor = sf::Color(255,0,0,60)
				});
	}
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
	auto& hotbarPressed = registry.get<component::action::HotbarPressed>(entity);
	sf::Vector2f hotbarPosition = _hotbar.getPosition();
	hotbarPosition.x += hotbarPressed.slot * HOTBAR_ICON_X_OFFSET;
	hotbarPosition += sf::Vector2f{ -16, -16 }; // to offset from center
	queueFlashEffect(hotbarPosition, { 32,32 }, 60, true);
}

void drft::system::HUD::onTakeDamage(entt::registry& registry, entt::entity entity)
{
	if (!registry.all_of<PlayerComponent>(entity)) return;

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
	if (!registry.all_of<PlayerComponent>(entity)) return;

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
