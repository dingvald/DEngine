#pragma once
#include "Systems/System.h"


namespace drft::system
{
	class CullingSystem : public System
	{
	public:
		void init() override;
		void fixedUpdate() override;
		void onFixedUpdateEnd() override;
	};
}


