#pragma once
#include "Systems/System.h"
#include "Utility/stdHashing.h"
#include "Utility/Visibility.h"

namespace drft::system
{
	class LightingSystem : public System
	{
	public:
		using System::System;

		static sf::Color blendColor(const sf::Color& color1, const sf::Color& color2);
		static sf::Color blendLight(const sf::Color& color1, const sf::Color& color2);

	private:
		void init() override;
		void update() override;

	private:
		std::unique_ptr<Visibility> _fov;
		std::unordered_set<sf::Vector3i> _lightBlockingPositions;
		std::vector<entt::entity> _toLight;
	};
}


