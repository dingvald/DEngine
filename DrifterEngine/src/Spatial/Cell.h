#pragma once

namespace drft::spatial
{
	class Cell
	{
	public:
		void placeEntity(entt::entity entity);
		void removeEntity(entt::entity entity);
		const std::vector<entt::entity>& getEntities() const;
		bool isEmpty() const;

	private:
		std::vector<entt::entity> _entities;
	};

}


