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
		System() {};
		virtual ~System() = default;

		void setRegistry(entt::registry& registry);
		void setScheduler(system::SystemScheduler& scheduler);
		virtual void onStart();
		virtual void init() = 0;

		virtual void save(cereal::JSONOutputArchive& oarchive);
		virtual void load(cereal::JSONInputArchive& iarchive);

		virtual void update(const float dt);
		virtual void fixedUpdate();
		virtual void render(sf::RenderTarget& target);
		virtual void onUpdateEnd();
		virtual void onFixedUpdateEnd();

		virtual void shutdown();

	protected:
		entt::registry* registry = nullptr;

	private:
		SystemScheduler* _scheduler = nullptr;
	};

} // namespace drft::system



