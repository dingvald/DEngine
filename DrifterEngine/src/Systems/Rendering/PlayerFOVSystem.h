#pragma once
#include "Systems/System.h"
#include "Utility/Visibility.h"
#include "Utility/stdHashing.h"

namespace drft::system
{
	class PlayerFOVSystem : public System
	{
	private:
		void init() override;
		void fixedUpdate() override;
		void onFixedUpdateEnd() override;

	private:
		std::unique_ptr<Visibility> _fov;
		std::unordered_set<sf::Vector2i> _lightBlockingPositions;
		std::vector<entt::entity> _toLight;
	};
}


