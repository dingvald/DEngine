#pragma once
#include "State.h"

namespace drft
{
	class EntityFactory;

	namespace system
	{
		class SystemScheduler;
	}

	namespace spatial
	{
		class WorldGrid;
	}

	class GameState : public State
	{
	public:
		GameState(StateStack& stack, State::Context context);

		bool handleEvent(const sf::Event& ev) override;
		bool update(const float dt) override;
		void render(sf::RenderTarget& target) override;

	private:
		void init();
		void importSystems();

	private:
		entt::registry _registry;
		std::unique_ptr<system::SystemScheduler> _systems;
		std::unique_ptr<spatial::WorldGrid> _world;
		std::unique_ptr<EntityFactory> _factory;
		std::unique_ptr<entt::dispatcher> _dispatcher;
	};

} // namespace drft


