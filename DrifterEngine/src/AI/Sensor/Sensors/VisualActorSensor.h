#pragma once
#include "AI/Sensor/ISensor.h"

class VisualActorSensor : public ISensor
{
public:
	void sense(entt::handle agent) const override;
	bool canUseSensor(entt::const_handle agent) override;
};


