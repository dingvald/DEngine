#pragma once
#include <EnTT/entt.hpp>

class IChunkDataSource;

class IChunkDataSourceProvider
{
public:
	virtual IChunkDataSource* tryGetDataSource(entt::id_type sourceId) = 0;
};