#pragma once
#include "AI/Sensor/ISensor.h"

class VisualActorSensor : public ISensor
{
public:
	void sense(entt::const_handle agent, Blackboard& blackboard) const override;
	bool canUseSensor(entt::const_handle agent) override;
};


