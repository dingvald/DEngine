#include "pch.h"
#include "FactionSystem.h"

using namespace drft::system;

constexpr int FRIEND_CLOSENESS = 100;
constexpr int ENEMY_CLOSENESS = -100;
constexpr int MAX_CLOSENESS = INT_MAX;

FactionSystem::FactionTable FactionSystem::_factionTable;

void drft::system::FactionSystem::init()
{
	_factionTable["good"]["bad"] = -1000;
	_factionTable["good"]["neutral"] = 0;
	_factionTable["bad"]["neutral"] = 0;
}

Closeness drft::system::FactionSystem::getCloseness(const std::string& faction1, const std::string& faction2)
{
	if (faction1 == faction2) return MAX_CLOSENESS;
	if (_factionTable.contains(faction1) && _factionTable.at(faction1).contains(faction2))
	{
		return _factionTable[faction1][faction2];
	}
	else if (_factionTable.contains(faction2) && _factionTable.at(faction2).contains(faction1))
	{
		return  _factionTable[faction2][faction1];
	}
	return 0;
}

void drft::system::FactionSystem::modifyCloseness(const std::string& faction1, const std::string& faction2, Closeness deltaValue)
{
	if (_factionTable.contains(faction1) && _factionTable.at(faction1).contains(faction2))
	{
		_factionTable[faction1][faction2] += deltaValue;
	}
	else if (_factionTable.contains(faction2) && _factionTable.at(faction2).contains(faction1))
	{
		_factionTable[faction2][faction1] += deltaValue;
	}
}

drft::system::Relationship drft::system::FactionSystem::resolveRelationship(const std::string& faction1, const std::string& faction2)
{
	Closeness closeness = getCloseness(faction1, faction2);
	
	if (faction1 == faction2)
	{
		return Relationship::Friendly;
	}
	if (closeness < ENEMY_CLOSENESS)
	{
		return Relationship::Hostile;
	}
	else if (closeness > FRIEND_CLOSENESS)
	{
		return Relationship::Friendly;
	}
	else
	{
		return Relationship::Neutral;
	}
}
