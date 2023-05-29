#pragma once
#include "System.h"


// Container class that orders systems by their phase
namespace drft::system
{
	enum Phase : int
	{
		Reactive = -100,
		OnPreUpdate = 0,
		OnProcessInput = 100,
		OnUpdate = 200,
		OnPostUpdate = 300,
		OnValidation = 400,
		OnFixedUpdate = 5000,
		OnRender = 10000
	};

	namespace
	{
		struct SystemPriorityPair
		{
			std::unique_ptr<System> system = nullptr;
			int priority = 0;
		};
	}

	class SystemScheduler
	{
	public:
		SystemScheduler(entt::registry& registry) : _registry(registry) {}

		void startAll(bool isNewGame) const;
		void initAll() const;
		void loadAll(cereal::JSONInputArchive& iarchive);

		template <typename T>
		void add(int priority)
		{
			static_assert(std::is_base_of<System, T>::value, "In SystemScheduler: type is not a system.");

			if (priority >= static_cast<int>(Phase::OnRender))
			{
				_systems[PhaseCategory::OnRender].push_back({ std::make_unique<T>(), priority });
				_systems[PhaseCategory::OnRender].back().system->setRegistry(_registry);
				std::stable_sort(_systems[PhaseCategory::OnRender].begin(), _systems[PhaseCategory::OnRender].end(),
					[](const SystemPriorityPair& lhs, const SystemPriorityPair& rhs)
					{
						return lhs.priority < rhs.priority;
					});
			}
			else if (priority < static_cast<int>(Phase::OnPreUpdate))
			{
				_systems[PhaseCategory::Reactive].push_back({ std::make_unique<T>(), priority });
				_systems[PhaseCategory::Reactive].back().system->setRegistry(_registry);
			}
			else if (priority < static_cast<int>(Phase::OnFixedUpdate))
			{
				_systems[PhaseCategory::OnUpdate].push_back({ std::make_unique<T>(), priority });
				_systems[PhaseCategory::OnUpdate].back().system->setRegistry(_registry);
				std::stable_sort(_systems[PhaseCategory::OnUpdate].begin(), _systems[PhaseCategory::OnUpdate].end(),
					[](const SystemPriorityPair& lhs, const SystemPriorityPair& rhs)
					{
						return lhs.priority < rhs.priority;
					});
			}
			else
			{
				_systems[PhaseCategory::OnFixedUpdate].push_back({ std::make_unique<T>(), priority });
				_systems[PhaseCategory::OnFixedUpdate].back().system->setRegistry(_registry);
				std::stable_sort(_systems[PhaseCategory::OnFixedUpdate].begin(), _systems[PhaseCategory::OnFixedUpdate].end(),
					[](const SystemPriorityPair& lhs, const SystemPriorityPair& rhs)
					{
						return lhs.priority < rhs.priority;
					});
			}
		}

		void update(const float dt) const;
		void updateEnd() const;
		void fixedUpdate() const;
		void fixedUpdateEnd() const;
		void render(sf::RenderTarget& target) const;

		void saveAll(cereal::JSONOutputArchive& oarchive);
		void shutdownAll();

	private:
		using SystemList = std::vector< SystemPriorityPair >;

		std::unordered_map<PhaseCategory, SystemList> _systems;
		entt::registry& _registry;
	};

} // namespace drft::system


