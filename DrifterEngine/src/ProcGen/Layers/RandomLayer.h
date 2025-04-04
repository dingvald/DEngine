#pragma once
#include <ProcGen/LayeredProcGen/LayeredProcGen.h>
#include <JSON/ICreateFromJson.h>

namespace drft
{
	class RandomLayer : public OnDemandLayer
	{
	public:
		void createFromJson(const rapidjson::Value& json) override;
		double getValueAt(sf::Vector3i tilePosition) override;
	};
}
