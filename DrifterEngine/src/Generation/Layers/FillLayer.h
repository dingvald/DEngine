#pragma once
#include <Generation/LayeredProcGen/LayeredProcGen.h>

namespace drft
{
	class FillLayer : public OnDemandLayer
	{
	public:
		using OnDemandLayer::OnDemandLayer;

		void createFromJson(const rapidjson::Value& json) override;
		double getValueAt(sf::Vector3i tilePosition) override;
	};
}
