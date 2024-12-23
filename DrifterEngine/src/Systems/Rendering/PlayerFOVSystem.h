#pragma once
#include "Systems/System.h"
#include "Utility/Visibility.h"
#include "Utility/stdHashing.h"

namespace drft::system
{
	class PlayerFOVSystem : public System
	{
	public:
		using System::System;

		void init() override;
		void update() override;

	private:
		std::unique_ptr<Visibility> _fov;
		std::unordered_set<sf::Vector3i> _lightBlockingPositions;
		std::unordered_set<sf::Vector3i> _toLight;

	};
}


