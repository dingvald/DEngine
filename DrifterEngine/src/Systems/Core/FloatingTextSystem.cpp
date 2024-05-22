#include "pch.h"
#include "FloatingTextSystem.h"
#include "Components/Components.h"
#include "Systems/Helpers/GetCurrentCamera.h"
#include "Utility/GetTextCenter.h"
#include "Spatial/Helpers.h"
#include "Spatial/Conversions.h"
#include "Utility/Math.h"

void drft::system::FloatingTextSystem::init()
{
	auto& dispatcher = _registry->ctx().get<entt::dispatcher&>();
	dispatcher.sink<events::SendFloatingMessageEvent>().connect<&FloatingTextSystem::onSendFloatingMessageEvent>(this);
}

void drft::system::FloatingTextSystem::fixedUpdate()
{
	updateFloatingMessagesDisplay();
}

void drft::system::FloatingTextSystem::render(sf::RenderTarget& target)
{
	for (auto& message : _floatingMessages)
	{
		target.draw(message.text);
	}
}

void drft::system::FloatingTextSystem::onSendFloatingMessageEvent(events::SendFloatingMessageEvent& ev)
{
	queueFloatingMessage(ev.message, ev.color, ev.tracksEntity, ev.position, ev.velocity, ev.fades, ev.ttl, ev.isScreenSpace);
}

void drft::system::FloatingTextSystem::queueFloatingMessage(const std::string& message, sf::Color color, entt::entity trackedEntity, sf::Vector2i position, sf::Vector2f velocity, bool fades, int ttl, bool isScreenSpace)
{
	using namespace entt::literals;
	const auto& font = _registry->ctx().get<sf::Font&>("terminus"_hs);
	const auto camera = getCurrentCamera(*_registry);

	if (isScreenSpace)
	{
		position = spatial::toTileSpace(toScreenSpace(position, camera));
	}

	_floatingMessages.emplace_back(sf::Text(std::string(message), font), position, velocity, trackedEntity, fades, ttl, isScreenSpace);
	auto& newMessage = _floatingMessages.back();
	newMessage.text.setFillColor(color);
	newMessage.text.setCharacterSize(16);
	newMessage.text.setOrigin(util::getTextCenter(newMessage.text));
}

void drft::system::FloatingTextSystem::updateFloatingMessagesDisplay()
{
	const auto camera = getCurrentCamera(*_registry);
	const sf::Vector2f offset = { spatial::TILE_WIDTH / 2.f, 0.f }; // So messages originate from the center of cells

	auto it = _floatingMessages.begin();
	while (it != _floatingMessages.end())
	{
		if (it->trackedEntity != entt::null)
		{
			if (auto posComp = _registry->try_get<component::Position>(it->trackedEntity))
			{
				it->position = posComp->position;
			}
		}
		if (it->isScreenSpace)
		{
			it->text.setPosition(spatial::toFloatSpace(it->position) + it->distanceTraveled + offset);
		}
		else
		{
			it->text.setPosition(toScreenSpace(it->position, camera) + it->distanceTraveled + offset);
		}
		if (it->fades)
		{
			sf::Color color = it->text.getFillColor();
			color.a = math::remap(0, 120, 0, 255, it->ttl);
			it->text.setFillColor(color);
		}

		it->distanceTraveled += it->velocity;
		--(it->ttl);
		it = (it->ttl <= 0) ? (_floatingMessages.erase(it)) : (it + 1);
	}
}
