#include "pch.h"
#include "HUD.h"
#include "Spatial/Conversions.h"
#include "Utility/SpriteIndexer.h"
#include "Components/Components.h"

static const sf::Vector2f HEALTHBAR_POSITION = { 32.f, 16.f };
static const int HEALTHBAR_WIDTH_MULTIPLIER = 5;

void drft::system::HUD::init()
{
	using namespace entt::literals;
	const auto& texture = registry->ctx().get<sf::Texture&>("sprites"_hs);
	_heartIcon.setTexture(texture);
	_heartIcon.setTextureRect(util::SpriteIndexer::get(static_cast<unsigned int>(util::Sprite::Heart), texture));
	_heartIcon.setPosition(HEALTHBAR_POSITION - sf::Vector2f(20.f, 5.f));
	_heartIcon.setColor(sf::Color(150, 60, 60, 200));

	_healthBarContainer.setSize({ 1.f, 8.f });
	_healthBarContainer.setPosition(HEALTHBAR_POSITION);
	_healthBarContainer.setFillColor(sf::Color(180, 180, 180, 128));

	_healthBar.setSize({ 1.f, 6.f });
	_healthBar.setPosition(HEALTHBAR_POSITION + sf::Vector2f{1.f, 1.f});
	_healthBar.setFillColor(sf::Color(150,60,60,200));
}

void drft::system::HUD::update(const float dt)
{
	auto view = registry->view<component::Player>(entt::exclude<component::Prototype>);
	auto player = entt::handle(*registry, view.front());

	if (auto health = player.try_get<component::Health>())
	{
		_healthBarContainer.setSize({ static_cast<float>(health->max * HEALTHBAR_WIDTH_MULTIPLIER), 8.f });
		_healthBar.setSize({ (static_cast<float>(health->current) / static_cast<float>(health->max)) 
			* static_cast<float>(health->max * HEALTHBAR_WIDTH_MULTIPLIER)-2.0f, 6.f });
	}
}

void drft::system::HUD::render(sf::RenderTarget& target)
{
	target.draw(_healthBarContainer);
	target.draw(_healthBar);
	target.draw(_heartIcon);
}
