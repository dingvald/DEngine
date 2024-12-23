#pragma once
#include "System.h"


// Container class that orders systems by their phase
namespace drft::system
{
	class SystemScheduler
	{
	public:
		SystemScheduler(entt::registry& registry) : _registry(registry) {}

		void startAll() const;
		void initAll() const;

		template <typename T>
		void add()
		{
			static_assert(std::is_base_of<System, T>::value, "In SystemScheduler: type is not a system.");

			auto& dispatcher = _registry.ctx().get<entt::dispatcher&>();
			_systems.emplace_back(std::make_unique<T>(_registry, dispatcher));
		}

		void update() const;
		void render(sf::RenderTarget& target) const;
		void shutdownAll();

	private:
		using SystemPtr = std::unique_ptr<System>;
		using SystemList = std::vector<SystemPtr>;

		SystemList _systems;
		entt::registry& _registry;
	};

} // namespace drft::system


