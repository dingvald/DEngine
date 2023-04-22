#include "pch.h"
#include "System.h"


void drft::system::System::setRegistry(entt::registry& registry)
{
	this->registry = &registry;
}

void drft::system::System::onStart()
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

void drft::system::System::render(sf::RenderTarget& target)
{
}

void drft::system::System::shutdown()
{
}
