#pragma once
#include <ProcGen/LayeredProcGen/LayeredProcGen.h>

namespace drft
{
	class DepthLayer : public OnDemandLayer
	{
	public:
		using OnDemandLayer::OnDemandLayer;

		void createFromJson(const rapidjson::Value& json) override;
		double getValueAt(sf::Vector3i tilePosition) override;
	};
}
