#pragma once
#include <Generation/LayeredProcGen/LayeredProcGen.h>

namespace drft
{
	class PositionArrayLayer : public OnDemandLayer
	{
	public:
		using OnDemandLayer::OnDemandLayer;

		void createFromJson(const rapidjson::Value& json) override;
		double getValueAt(sf::Vector3i tilePosition) override;

	private:
		std::unordered_set<sf::Vector3i> _positions3d;
		std::unordered_set<sf::Vector2i> _positions2d;
	};
}