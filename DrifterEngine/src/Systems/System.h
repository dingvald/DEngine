#pragma once

namespace drft::system
{
	class SystemScheduler;

	enum class PhaseCategory : int
	{
		None,
		Reactive,
		OnUpdate,
		OnFixedUpdate,
		OnRender
	};

	class System
	{
	public:
		System(entt::registry& registry, entt::dispatcher& dispatcher);
		virtual ~System() = default;

		virtual void init();
		virtual void onStart();

		virtual void onUpdateBegin();
		virtual void onUpdate(const float dt);
		virtual void onUpdateLate(const float dt);
		virtual void onUpdateEnd();

		virtual void onFixedUpdateBegin();
		virtual void onFixedUpdate();
		virtual void onFixedUpdateLate();
		virtual void onFixedUpdateEnd();

		virtual void render(sf::RenderTarget& target);
		
		virtual void shutdown();

	protected:
		entt::registry& _registry;
		entt::dispatcher& _dispatcher;
	};

} // namespace drft::system



