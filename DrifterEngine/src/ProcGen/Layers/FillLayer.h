#pragma once
#include <ProcGen/LayeredProcGen/LayeredProcGen.h>

class FillLayer : public OnDemandLayer
{
public:
	double getValueAt(sf::Vector2i tilePosition) override;
};