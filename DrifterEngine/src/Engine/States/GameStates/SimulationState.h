#pragma once
#include <Engine/States/State.h>
#include <Engine/StateStack.h>
#include <Spatial/WorldGrid.h>

namespace drft
{
	namespace system
	{
		class SystemScheduler;
	}
	namespace spatial
	{
		class WorldGrid;
	}

	class SimulationState : public State
	{
	public:
		SimulationState(StateStack& stack, StateContext& context);

		bool handleEvent(const sf::Event& ev) override;
		bool update() override;
		void render(sf::RenderTarget& target) override;
		void onPop() override;

	private:
		void setupRegistryContext();
		void importSystems();

		void loadPlayer();
		void savePlayer();

		void loadRegistry();
		void saveRegistry();
		bool isPlayerAlive() const;

	private:
		spatial::WorldGrid _world;
		std::unique_ptr<system::SystemScheduler> _systems;
		entt::handle _player{};
	};

} // namespace drft


