#pragma once
#include "State.h"
#include "Systems/HelperClasses/InputBuffer.h"
#include <Engine/StateStack.h>

namespace drft
{
	class EntityFactory;

	namespace gen
	{
		class WorldGenerator;
	}
	
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
	}

	class GameState : public State
	{
	public:
		GameState(StateStack& stack, StateContext& context);

		bool handleEvent(const sf::Event& ev) override;
		bool update() override;
		void render(sf::RenderTarget& target) override;
		void onPop() override;

	private:
		void registerGameStates();
		void connectEventHandlers();
		void loadOrCreateWorldGenerator();
		void loadEntityPrototypes();
		void setupRegistryContext();
		void setupActionMap();
		void onRequestStatePush(const drft::events::RequestStateStackPush& ev);

	private:
		StateStack _gameStateStack;
		system::InputBuffer _inputBuffer{ 3 };

		std::unique_ptr<gen::WorldGenerator> _worldGenerator;
		std::unique_ptr<EntityFactory> _factory;
		std::unique_ptr<entt::dispatcher> _dispatcher;
	};

} // namespace drft


