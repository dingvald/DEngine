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
		virtual void update(const float dt) {};
		virtual void updateEnd() {};

		virtual void fixedUpdateBegin() {};
		virtual void fixedUpdate() {};
		virtual void fixedUpdateEnd() {};

		virtual void render(sf::RenderTarget& target) {};
		
		virtual void shutdown() {};

	protected:
		entt::registry& _registry;
		entt::dispatcher& _dispatcher;
	};

} // namespace drft::system



