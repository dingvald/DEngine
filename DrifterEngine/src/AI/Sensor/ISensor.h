#pragma once

#include "SensorTypes.h"
#include <Spatial/TilePosition.h>

class Blackboard;

class ISensor
{
public:
	virtual bool canUseSensor(entt::const_handle agent) = 0;
	virtual void sense(entt::handle agent) const = 0;

protected:
	Blackboard& getBlackboard(entt::handle agent) const;
};


