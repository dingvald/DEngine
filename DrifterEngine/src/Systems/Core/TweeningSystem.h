#pragma once
#include <Systems/System.h>

struct Tween;

namespace drft::system
{
	class TweeningSystem : public System
	{
	public:
		using System::System;

		static void tween(entt::handle entity, Tween tween);

	private:
		void fixedUpdate() override;
	};
}