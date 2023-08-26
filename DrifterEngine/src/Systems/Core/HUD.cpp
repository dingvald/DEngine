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
#include "Components/Components.h"

static const sf::Vector2f HEALTHBAR_POSITION = { 32.f, 32.f };
static constexpr float HEALTHBAR_HEIGHT = 7;
static constexpr int HEALTHBAR_WIDTH_MULTIPLIER = 7;

static const sf::Vector2f STAMINABAR_POSITION = HEALTHBAR_POSITION + sf::Vector2f(0.f, 16.f);
static constexpr float STAMINABAR_HEIGHT = HEALTHBAR_HEIGHT;
static constexpr int STAMINABAR_WIDTH_MULTIPLIER = HEALTHBAR_WIDTH_MULTIPLIER;

static constexpr int MESSAGE_LIFETIME = 80; // frames

void drft::system::HUD::init()
{
	createLevelInfo();
	createHealthBar();
	createStaminaBar();
	createInHandsDisplay();
	createItemsOnGroundDisplay();

	auto& dispatcher = registry->ctx().get<entt::dispatcher&>();
	dispatcher.sink<events::ItemBreakEvent>().connect<&HUD::onItemBreakEvent>(this);
	dispatcher.sink<events::SendFloatingMessageEvent>().connect<&HUD::onSendFloatingMessageEvent>(this);
	registry->on_construct<component::action::TakeDamage>().connect<&HUD::onTakeDamage>(this);
	registry->on_construct<component::action::ConsumeStamina>().connect<&HUD::onConsumeStamina>(this);
	registry->on_construct<component::action::LevelUp>().connect<&HUD::onLevelUp>(this);
}

void drft::system::HUD::fixedUpdate()
{
	auto view = registry->view<component::Player>();
	auto player = entt::handle(*registry, view.front());

	// Player relevant displays
	updateLevelInfo(player);
	updateHealthBar(player);
	updateStaminaBar(player);
	updateInHandsDisplay(player);
	updateItemsOnGround(player);
	updateFloatingMessagesDisplay(player);

	// Effects
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
	
	for (auto& message : _floatingMessages)
	{
		target.draw(message.text);
	}

	for (auto effect : _flashEffects)
	{
		target.draw(effect.shape);
	}
}

void drft::system::HUD::createLevelInfo()
{
	using namespace entt::literals;
	const auto& font = registry->ctx().get<sf::Font&>("terminus"_hs);

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
	const auto& texture = registry->ctx().get<sf::Texture&>("sprites"_hs);
	_heartIcon.setTexture(texture);
	_heartIcon.setTextureRect(util::SpriteIndexer::get(util::Sprite::Heart, texture));
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
	const auto& texture = registry->ctx().get<sf::Texture&>("sprites"_hs);
	_staminaIcon.setTexture(texture);
	_staminaIcon.setTextureRect(util::SpriteIndexer::get(util::Sprite::Diamond, texture));
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
	_inHandsDisplay.setPosition(HEALTHBAR_POSITION + sf::Vector2f(-7.f, 12.f))
		.setChildrenOrigin(gui::ElementPosition::CENTER_LEFT)
		.setStyle(gui::ElementState::Idle, {
			.childPadding = {48.f, 0.f}
			});

	_inHandsDisplay.insert("RightHandContainer", gui::SingleContainer())
		.setSize({ 32, 32 })
		.setStyle(gui::ElementState::Idle, {
			.fillColor = {0,0,0,150},
			.outlineColor = {150,150,150,100},
			.outlineThickness = 1.f
			})
		.insert("Item", gui::DualContainer());
	_inHandsDisplay.insert("LeftHandContainer", gui::SingleContainer())
		.setSize({ 32, 32 })
		.setStyle(gui::ElementState::Idle, {
			.fillColor = {0,0,0,150},
			.outlineColor = {150,150,150,100},
			.outlineThickness = 1.f
			})
		.insert("Item", gui::DualContainer());
}

void drft::system::HUD::createItemsOnGroundDisplay()
{
	using namespace entt::literals;
	const auto view = registry->ctx().get<sf::RenderWindow&>().getView();
	_itemsOnGround.setPosition({ view.getCenter().x + (view.getSize().x / 2) - 80, view.getCenter().y + (view.getSize().y / 2) - 64 })
		.setStyle(gui::ElementState::Idle, {
			.fillColor = sf::Color(0,0,0,100),
			.outlineThickness = 1.f,
			.innerPadding = {0.f, 0.f},
			.childPadding = {0.f, 4.f},
			.font = &registry->ctx().get<sf::Font&>("terminus"_hs),
			.textColor = sf::Color(200,200,200,200),
			.textSize = 16
			})
		.setChildrenOrigin(gui::ElementPosition::TOP_LEFT);
}

void drft::system::HUD::updateLevelInfo(entt::const_handle player)
{
	if (auto level = player.try_get<component::Leveling>())
	{
		_lvlText.setString("Lvl " + std::to_string(level->currentLevel));
		_xpText.setString("XP " + std::to_string(level->currentXP) + "/" + std::to_string(level->neededXP));
	}
}

void drft::system::HUD::updateHealthBar(entt::const_handle player)
{
	if (auto health = player.try_get<component::Health>())
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
	if (auto stamina = player.try_get<component::Stamina>())
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
	const auto& grid = registry->ctx().get<spatial::WorldGrid&>();

	_itemsOnGround.clear();

	if (auto pos = player.try_get<component::Position>())
	{
		auto entities = grid.entitiesAt(pos->position,
			[this](entt::entity entity) -> bool
			{
				if (registry->all_of<component::Info>(entity)
				&& !registry->any_of<component::Player>(entity))
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
					.font = &registry->ctx().get<sf::Font&>("terminus"_hs),
					.textColor = sf::Color::White,
					.textSize = 16
					})
				.setOrigin(gui::ElementPosition::CENTER_LEFT)
				.setTextString(util::getEntityName({ *registry, entity }));
		}
	}
	
	_itemsOnGround.update(0.f);
}

void drft::system::HUD::updateInHandsDisplay(entt::const_handle player)
{
	_inHandsDisplay["RightHandContainer"]["Item"].clear();
	_inHandsDisplay["LeftHandContainer"]["Item"].clear();

	if (auto body = player.try_get<component::Body>())
	{
		if (body->parts.contains("HeldR") && body->parts.at("HeldR") != component::Item::NONE)
		{
			entt::entity rightHandItem = ItemDatabase::getEntityFromItemID(body->parts.at("HeldR"));
			addItemIcon(_inHandsDisplay["RightHandContainer"]["Item"], rightHandItem);
		}
		else
		{
			using namespace entt::literals;
			const auto& sprites = registry->ctx().get<sf::Texture&>("sprites"_hs);
			sf::Sprite sprite = { sprites, util::SpriteIndexer::get(util::Sprite::PaperDollHandR, sprites) };
			_inHandsDisplay["RightHandContainer"]["Item"].insert("Icon", gui::Icon(sprite))
				.setSize({ 32,32 })
				.setOrigin(gui::ElementPosition::BOTTOM_RIGHT)
				.setStyle(gui::ElementState::Idle, {
						.fillColor = sf::Color(80,80,80,150)
					});
		}
		if (body->parts.contains("HeldL") && body->parts.at("HeldL") != component::Item::NONE)
		{
			entt::entity leftHandItem = ItemDatabase::getEntityFromItemID(body->parts.at("HeldL"));
			addItemIcon(_inHandsDisplay["LeftHandContainer"]["Item"], leftHandItem);
		}
		else
		{
			using namespace entt::literals;
			const auto& sprites = registry->ctx().get<sf::Texture&>("sprites"_hs);
			sf::Sprite sprite = { sprites, util::SpriteIndexer::get(util::Sprite::PaperDollHandL, sprites) };
			_inHandsDisplay["LeftHandContainer"]["Item"].insert("Icon", gui::Icon(sprite))
				.setSize({ 32,32 })
				.setOrigin(gui::ElementPosition::BOTTOM_RIGHT)
				.setStyle(gui::ElementState::Idle, {
						.fillColor = sf::Color(80,80,80,150)
					});
		}
	}

	_inHandsDisplay.update(0.f);
}

void drft::system::HUD::updateFloatingMessagesDisplay(entt::const_handle)
{
	const auto camera = getCurrentCamera(*registry);
	const sf::Vector2f offset = { spatial::TILE_WIDTH / 2.f, 0.f }; // So messages originate from the center of cells
	auto it = _floatingMessages.begin();
	while (it != _floatingMessages.end())
	{
		if (it->isScreenSpace)
		{
			it->text.setPosition(spatial::toFloatSpace(it->position) + it->distanceTraveled + offset);
		}
		else
		{
			it->text.setPosition(toScreenSpace(it->position, camera) + it->distanceTraveled + offset);
		}
		
		util::SmoothTransition transition(0, 255, 0, MESSAGE_LIFETIME);
		sf::Color color = it->text.getFillColor();
		color.a = static_cast<sf::Uint8>(transition.compute(static_cast<float>(it->ttl)));
		it->distanceTraveled += it->velocity;
		--(it->ttl);
		it->text.setFillColor(color);
		it = (it->ttl <= 0) ? (_floatingMessages.erase(it)) : (it + 1);
	}
}

void drft::system::HUD::updateFlashEffects()
{
	auto it = _flashEffects.begin();
	while (it != _flashEffects.end())
	{
		--(it->ttl);
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

void drft::system::HUD::addItemIcon(gui::Element& container, entt::entity item)
{
	using namespace entt::literals;
	const auto& itemRender = registry->get<component::Render>(item);
	const auto& sprites = registry->ctx().get<sf::Texture&>("sprites"_hs);

	sf::Sprite sprite = { sprites, util::SpriteIndexer::get(static_cast<util::Sprite>(itemRender.sprite), sprites) };
	container.insert("Icon", gui::Icon(sprite))
		.setSize({ 32,32 })
		.setOrigin(gui::ElementPosition::BOTTOM_RIGHT)
		.setStyle(gui::ElementState::Idle, {
				.fillColor = itemRender.color
			})
		.setStyle(gui::ElementState::Focused, {
					.fillColor = itemRender.color
			});

	if (auto health = registry->try_get<component::Health>(item))
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

void drft::system::HUD::queueFloatingMessage(const std::string& message, sf::Color color, sf::Vector2i position, sf::Vector2f velocity, int ttl, bool isScreenSpace)
{
	using namespace entt::literals;
	const auto& font = registry->ctx().get<sf::Font&>("terminus"_hs);
	const auto camera = getCurrentCamera(*registry);

	if (isScreenSpace)
	{
		position = spatial::toTileSpace(toScreenSpace(position, camera));
	}

	_floatingMessages.emplace_back(sf::Text(std::string(message), font), position, velocity, ttl, isScreenSpace);
	auto& newMessage = _floatingMessages.back();
	newMessage.text.setFillColor(color);
	newMessage.text.setCharacterSize(16);
	newMessage.text.setOrigin(util::getTextCenter(newMessage.text));
}

void drft::system::HUD::queueFlashEffect(sf::Vector2f position, sf::Vector2f size, int ttl)
{
	sf::RectangleShape shape;
	shape.setPosition(position);
	shape.setSize(size);
	shape.setFillColor(sf::Color::White);

	_flashEffects.emplace_back(shape, ttl);
}

void drft::system::HUD::onSendFloatingMessageEvent(events::SendFloatingMessageEvent& ev)
{
	queueFloatingMessage(ev.message, ev.color, ev.position, ev.velocity, ev.ttl, ev.isScreenSpace);
}

void drft::system::HUD::onItemBreakEvent(events::ItemBreakEvent& ev)
{
	auto camera = getCurrentCamera(*registry);
	if (auto pos = registry->try_get<component::Position>(ev.owner))
	{
		auto itemName = util::getEntityName({ *registry, ItemDatabase::getEntityFromItemID(ev.itemID) });
		queueFloatingMessage(itemName + " broke!", sf::Color::Yellow, pos->position, { 0,-1 }, MESSAGE_LIFETIME * 2, false);
	}
}

void drft::system::HUD::onTakeDamage(entt::registry& registry, entt::entity entity)
{
	auto& damage = registry.get<component::action::TakeDamage>(entity);
	// queue damage numbers
	if (auto pos = registry.try_get<component::Position>(entity))
	{
		auto camera = getCurrentCamera(registry);
		if (damage.amount > 0)
		{
			queueFloatingMessage(std::to_string(damage.amount), sf::Color::White, pos->position, { 0, -0.75 }, MESSAGE_LIFETIME, false);
		}
		else if (damage.amount < 0)
		{
			queueFloatingMessage("+" + std::to_string(std::abs(damage.amount)), sf::Color::Green, pos->position, { 0, -0.75 }, MESSAGE_LIFETIME, false);
		}
		else
		{
			queueFloatingMessage(std::to_string(damage.amount), sf::Color::Blue, pos->position, { 0, -0.75 }, MESSAGE_LIFETIME, false);
		}
	}
	// Flash health bar
	if (auto health = registry.try_get<component::Health>(entity);
		registry.all_of<component::Player>(entity))
	{
		
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
	if (!registry.all_of<component::Player>(entity)) return;
	if (auto stamina = registry.try_get<component::Stamina>(entity))
	{
		auto consume = registry.get<component::action::ConsumeStamina>(entity);
		if (stamina->baseConsumption + consume.amount > 0.f)
		{
			sf::Vector2f size = { (static_cast<float>(stamina->current) / static_cast<float>(stamina->max))
			* static_cast<float>(stamina->max * STAMINABAR_WIDTH_MULTIPLIER) - 2.0f, STAMINABAR_HEIGHT + 2.f };
			queueFlashEffect(STAMINABAR_POSITION, size, 10);
		}
	}
}

void drft::system::HUD::onLevelUp(entt::registry& registry, entt::entity entity)
{
	if (auto pos = registry.try_get<component::Position>(entity))
	{
		auto camera = getCurrentCamera(registry);
		queueFloatingMessage("LEVEL UP", sf::Color::Magenta, pos->position, { 0, -0.5 }, MESSAGE_LIFETIME, false);
	}
}
