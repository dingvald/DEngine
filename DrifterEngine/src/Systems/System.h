#pragma once

namespace drft::system
{
	class System
	{
	public:
		System(entt::registry& registry, entt::dispatcher& dispatcher);
		virtual ~System() = default;

		virtual void init() {};
		virtual void start() {};

		virtual void updateBegin() {};
		virtual void update() {};
		virtual void updateLate() {};
		virtual void updateEnd() {};

		virtual void render(sf::RenderTarget& target) {};
		
		virtual void shutdown() {};

	protected:
		entt::registry& _registry;
		entt::dispatcher& _dispatcher;
	};

} // namespace drft::system



