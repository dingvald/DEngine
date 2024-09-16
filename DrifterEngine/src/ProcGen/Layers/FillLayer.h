#pragma once
#include <ProcGen/LayeredProcGen/LayeredProcGen.h>

namespace drft
{
	class FillLayer : public OnDemandLayer
	{
	public:
		double getValueAt(sf::Vector3i tilePosition) override;
	};
}
