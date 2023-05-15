#pragma once
#include "Systems/System.h"
#include "Utility/stdHashing.h"
#include "Utility/Visibility.h"

namespace drft::system
{
	class LightingSystem : public System
	{
	public:
		void init() override;
		void fixedUpdate() override;

	private:
		std::unique_ptr<Visibility> _fov;
		std::unordered_set<sf::Vector2i> _lightBlockingPositions;
		std::vector<entt::entity> _toLight;
	};
}


