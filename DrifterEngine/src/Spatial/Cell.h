#pragma once

namespace drft::spatial
{
	class Cell
	{
	public:
		void placeEntity(entt::entity entity);
		void removeEntity(entt::entity entity);
		std::vector<entt::entity> getEntities() const;
		bool empty();

	private:
		std::vector<entt::entity> _entities;
	};

} // namespace drft::spatial


