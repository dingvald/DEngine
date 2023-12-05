#pragma once
#include "Systems/System.h"
#include "Events/SendFloatingMessageEvent.h"

namespace drft::system
{
	class FloatingTextSystem : public System
	{
	public:
		void init() override;
		void fixedUpdate() override;
		void render(sf::RenderTarget& target) override;

	private:
		void onSendFloatingMessageEvent(events::SendFloatingMessageEvent& ev);
		void queueFloatingMessage(const std::string& message, sf::Color color, entt::entity trackedEntity, sf::Vector2i position, sf::Vector2f velocity, bool fades, int ttl, bool isScreenSpace);
		void updateFloatingMessagesDisplay();

	private:
		struct FloatingMessage
		{
			sf::Text text;
			sf::Vector2i position;
			sf::Vector2f velocity;
			entt::entity trackedEntity;
			bool fades = false;
			int ttl = 0;
			bool isScreenSpace = false;
			sf::Vector2f distanceTraveled;
		};

		std::vector<FloatingMessage> _floatingMessages;
	};
}


