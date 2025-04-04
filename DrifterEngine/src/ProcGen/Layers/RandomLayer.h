#pragma once
#include <ProcGen/LayeredProcGen/LayeredProcGen.h>
#include <JSON/ICreateFromJson.h>

namespace drft
{
	class RandomLayer : public OnDemandLayer
	{
	public:
		double getValueAt(sf::Vector3i tilePosition) override;
	};
}
