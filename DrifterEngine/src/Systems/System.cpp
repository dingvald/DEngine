#include "pch.h"
#include "System.h"
#include "SystemScheduler.h"


drft::system::System::System(entt::registry& registry, entt::dispatcher& dispatcher)
	: _registry(registry)
	, _dispatcher(dispatcher)
{}