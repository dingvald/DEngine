#include "Systems/System.h"
#include "Events/TurnStartEvent.h"

namespace drft::system
{
	class HealthSystem : public System
	{
	public:
		void init() override;
		void update(const float dt) override;
		void onUpdateEnd() override;

	private:
		void onTurnStartEvent(events::TurnStartEvent& ev);
		void onLevelUp(entt::registry& registry, entt::entity entity);
	};
}