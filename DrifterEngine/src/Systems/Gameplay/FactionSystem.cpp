#include "pch.h"
#include "FactionSystem.h"
#include "Components/FactionComponent.h"

using namespace drft::system;

constexpr int FRIEND_CLOSENESS = 100;
constexpr int ENEMY_CLOSENESS = -100;
constexpr int MAX_CLOSENESS = INT_MAX;

FactionSystem::FactionTable FactionSystem::_factionTable;

void drft::system::FactionSystem::init()
{
	_factionTable["player"]["carrion"] = -1000;
	_factionTable["player"]["parasite"] = -1000;
	_factionTable["player"]["rodent"] = -1000;
	_factionTable["player"]["feline"] = -1000;
	_factionTable["player"]["reptile"] = -1000;
	_factionTable["player"]["golem"] = -1000;

	_factionTable["rodent"]["feline"] = -1000;
	_factionTable["reptile"]["feline"] = -1000;
	_factionTable["carrion"]["rodent"] = -1000;
	_factionTable["parasite"]["rodent"] = -1000;
	_factionTable["parasite"]["feline"] = -1000;

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

drft::system::Relationship drft::system::FactionSystem::resolveRelationship(entt::const_handle entity1, entt::const_handle entity2)
{
	auto faction1 = entity1.try_get<FactionComponent>();
	auto faction2 = entity2.try_get<FactionComponent>();

	if (!faction1 || !faction2) return Relationship::Neutral;

	Closeness closeness = getCloseness(faction1->name, faction2->name);
	
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
