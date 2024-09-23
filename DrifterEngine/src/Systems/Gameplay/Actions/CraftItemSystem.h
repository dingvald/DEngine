#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class CraftItemSystem : public System
	{
	public:
		using System::System;

		static bool craftItem(entt::handle crafter, const std::string& itemName);
	};

}


