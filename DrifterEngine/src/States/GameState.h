#pragma once
#include "State.h"
#include "Systems/HelperClasses/InputBuffer.h"
#include <Engine/StateStack.h>
#include <ProcGen/GenerationRegistries.h>

class SolarSystem;

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
	}

	class GameState : public State
	{
	public:
		GameState(StateStack& stack, StateContext& context);

		bool handleEvent(const sf::Event& ev) override;
		bool update(const float dt) override;
		bool fixedUpdate() override;
		void render(sf::RenderTarget& target) override;
		void guiRender(sf::RenderTarget& target) override;
		void onPop() override;

	private:
		void registerGameStates();
		void connectEventHandlers();
		void loadOrCreateUniverseGenerator();
		void loadGenerationRegistries();
		void setupRegistryContext();
		void setupActionMap();
		void onRequestStatePush(const drft::events::RequestStateStackPush& ev);

	private:
		StateStack _gameStateStack;
		system::InputBuffer _inputBuffer{ 3 };

		std::unique_ptr<SolarSystem> _solarSystem;
		std::unique_ptr<entt::dispatcher> _dispatcher;

		GenerationRegistries _generationRegistries;
	};

} // namespace drft


