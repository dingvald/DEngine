#pragma once
#include <EnTT/entt.hpp>

class IChunkGenerator;

class IChunkGeneratorProvider
{
public:
	virtual IChunkGenerator* tryGetGenerator(entt::id_type sourceId) = 0;
};