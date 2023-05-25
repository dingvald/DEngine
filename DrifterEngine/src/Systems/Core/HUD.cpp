#include "pch.h"
#include "HUD.h"
#include "Spatial/Conversions.h"
#include "Utility/SpriteIndexer.h"
#include "Utility/ItemIDToEntityID.h"
#include "Components/Components.h"

static const sf::Vector2f HEALTHBAR_POSITION = { 32.f, 16.f };
static constexpr float HEALTHBAR_HEIGHT = 7;
static constexpr int HEALTHBAR_WIDTH_MULTIPLIER = 7;

static const sf::Vector2f STAMINABAR_POSITION = HEALTHBAR_POSITION + sf::Vector2f(0.f, 16.f);
static constexpr float STAMINABAR_HEIGHT = HEALTHBAR_HEIGHT;
static constexpr int STAMINABAR_WIDTH_MULTIPLIER = HEALTHBAR_WIDTH_MULTIPLIER;

void drft::system::HUD::init()
{
	createHealthBar();
	createStaminaBar();
	createInHandsDisplay();
}

void drft::system::HUD::fixedUpdate()
{
	auto view = registry->view<component::Player>();
	auto player = entt::handle(*registry, view.front());
	if (auto health = player.try_get<component::Health>())
	{
		_healthBarContainer.setSize({ static_cast<float>(health->max * HEALTHBAR_WIDTH_MULTIPLIER), HEALTHBAR_HEIGHT + 2.f });
		_healthBar.setSize({ (static_cast<float>(health->current) / static_cast<float>(health->max)) 
			* static_cast<float>(health->max * HEALTHBAR_WIDTH_MULTIPLIER)-2.0f, HEALTHBAR_HEIGHT });
	}
	else
	{
		_healthBar.setSize({ 0.f, HEALTHBAR_HEIGHT });
	}

	if (auto stamina = player.try_get<component::Stamina>())
	{
		_staminaBarContainer.setSize({ static_cast<float>(stamina->max * HEALTHBAR_WIDTH_MULTIPLIER), HEALTHBAR_HEIGHT + 2.f });
		_staminaBar.setSize({ (static_cast<float>(stamina->current) / static_cast<float>(stamina->max))
			* static_cast<float>(stamina->max * HEALTHBAR_WIDTH_MULTIPLIER) - 2.0f, HEALTHBAR_HEIGHT });
	}
	else
	{
		_staminaBar.setSize({ 0.f, HEALTHBAR_HEIGHT });
	}

	_inHandsDisplay["RightHandContainer"]["Item"].clear();
	_inHandsDisplay["LeftHandContainer"]["Item"].clear();

	if (auto body = player.try_get<component::Body>())
	{
		if (body->parts.contains("HeldR") && body->parts.at("HeldR") != component::Item::NONE)
		{
			entt::entity rightHandItem = util::ItemIDToEntityID(body->parts.at("HeldR"), *registry);
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
			entt::entity leftHandItem = util::ItemIDToEntityID(body->parts.at("HeldL"), *registry);
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

void drft::system::HUD::render(sf::RenderTarget& target)
{
	target.draw(_healthBarContainer);
	target.draw(_healthBar);
	target.draw(_heartIcon);

	target.draw(_staminaBarContainer);
	target.draw(_staminaBar);
	target.draw(_staminaIcon);

	_inHandsDisplay.render(target);
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
	_inHandsDisplay.setPosition(HEALTHBAR_POSITION - sf::Vector2f(7.f, -18.f))
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
		float scalingFactor = (static_cast<float>(health->current) / static_cast<float>(health->max));
		container.insert("Health", gui::Panel())
			.setSize({ 32, (32 - 32 * scalingFactor) })
			.setLocalPosition({ 0, 16 })
			.setOrigin(gui::ElementPosition::BOTTOM_CENTER)
			.setStyle(gui::ElementState::Idle, {
				.fillColor = sf::Color(255,0,0,30)
				})
			.setStyle(gui::ElementState::Focused, {
				.fillColor = sf::Color(255,0,0,30)
				});
	}
}
