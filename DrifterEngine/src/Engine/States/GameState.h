#pragma once
#include "State.h"

namespace drft
{
	class EntityFactory;

	namespace events
	{
		struct RequestStateStackPush;
	}

	namespace system
	{
		class SystemScheduler;
	}

	namespace spatial
	{
		class WorldGrid;
		template<typename T>
		class Grid;
	}

	class GameState : public State
	{
	public:
		GameState(StateStack& stack, StateContext& context);

		bool handleEvent(const sf::Event& ev) override;
		bool update(const float dt) override;
		bool fixedUpdate() override;
		void render(sf::RenderTarget& target) override;
		void onPop() override;

	private:
		void init();
		void connectEventHandlers();
		void loadOrCreateGameSeed();
		void loadOrCreatePlayer();
		void loadEntityPrototypes();
		void setupRegistryContext();
		void loadRegistry();
		void importSystems();
		void onRequestStatePush(const drft::events::RequestStateStackPush& ev);

	private:
		std::unique_ptr<system::SystemScheduler> _systems;
		std::unique_ptr<spatial::WorldGrid> _world;
		std::unique_ptr<EntityFactory> _factory;
		std::unique_ptr<entt::dispatcher> _dispatcher;
		entt::handle _player{};
	};

} // namespace drft


