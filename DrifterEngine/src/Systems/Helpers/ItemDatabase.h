#pragma once

class ItemDatabase
{
public:
	static entt::entity getEntityFromItemID(unsigned long itemID);
	static unsigned long getItemIDFromEntity(entt::const_handle entity);
};

