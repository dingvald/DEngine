#pragma	once
#include "Systems/System.h"

namespace drft::system
{
	class MovementSystem : public System
	{
	public:
		void init() override;
		void update(const float dt) override;
		void onUpdateEnd() override;
	};
}