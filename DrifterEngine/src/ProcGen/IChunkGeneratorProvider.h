#pragma once
#include <EnTT/entt.hpp>

class IChunkGenerator;

class IChunkGeneratorProvider
{
public:
	virtual IChunkGenerator* get(entt::id_type sourceId) = 0;
};