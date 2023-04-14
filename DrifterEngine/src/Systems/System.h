#pragma once

namespace drft::system
{
	class System
	{
	public:
		System() {};
		virtual ~System() = default;

		void setRegistry(entt::registry& registry);
		virtual void init() = 0;
		virtual void save(std::string filename);
		virtual void load(std::string filename);
		virtual void update(const float dt);
		virtual void render(sf::RenderTarget& target);
		virtual void shutdown();

	protected:
		entt::registry* registry = nullptr;
	};

} // namespace drft::system



