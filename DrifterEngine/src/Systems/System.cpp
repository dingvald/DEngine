#include "pch.h"
#include "System.h"
#include "SystemScheduler.h"


drft::system::System::System(entt::registry& registry, entt::dispatcher& dispatcher)
	: _registry(registry)
	, _dispatcher(dispatcher)
{
}

void drft::system::System::init()
{
}

void drft::system::System::onStart(bool)
{
}

void drft::system::System::save(cereal::JSONOutputArchive& oarchive)
{
}

void drft::system::System::load(cereal::JSONInputArchive& iarchive)
{
}

void drft::system::System::onUpdateBegin()
{
}

void drft::system::System::onUpdate(const float dt)
{
}

void drft::system::System::onUpdateLate(const float dt)
{
}

void drft::system::System::onFixedUpdateBegin()
{
}

void drft::system::System::onFixedUpdate()
{
}

void drft::system::System::onFixedUpdateLate()
{
}

void drft::system::System::render(sf::RenderTarget& target)
{
}

void drft::system::System::onUpdateEnd()
{
}

void drft::system::System::onFixedUpdateEnd()
{
}

void drft::system::System::shutdown()
{
}
