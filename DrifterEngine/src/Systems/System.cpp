#include "pch.h"
#include "System.h"
#include "SystemScheduler.h"


void drft::system::System::setRegistry(entt::registry& registry)
{
	this->registry = &registry;
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

void drft::system::System::update(const float dt)
{
}

void drft::system::System::fixedUpdate()
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
