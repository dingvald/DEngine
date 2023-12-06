#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class HotbarSystem : public System
	{
		virtual void init() override;
		virtual void onStart(bool isNewGame) override;
		virtual void update(float dt) override;
		virtual void onUpdateEnd() override;
	};
}


