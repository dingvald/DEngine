#pragma once

#include "SensorTypes.h"
#include <Spatial/TilePosition.h>

class Blackboard;

class ISensor
{
public:
	virtual bool canUseSensor(entt::const_handle agent) = 0;
	virtual void sense(entt::const_handle agent, Blackboard& blackboard) const = 0;
};


