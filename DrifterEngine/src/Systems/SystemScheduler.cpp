#include "pch.h"
#include "SystemScheduler.h"

void drft::system::SystemScheduler::initAll() const
{
	std::cout << "Initializing Systems..." << std::endl;
	for (auto& [_, systems] : _systems)
	{
		for (auto& [system, _] : systems)
		{
			system->init();
			std::string typeName = typeid(*system).name();
			std::cout << "Initializing " << typeName << "..." << std::endl;
		}
	}
}

void drft::system::SystemScheduler::update(const float dt) const
{
	for (auto& [system, _] : _systems.at(Phase::OnUpdate))
	{
		system->update(dt);
	}
	for (auto& [system, _] : _systems.at(Phase::OnRender))
	{
		system->update(dt);
	}
}

void drft::system::SystemScheduler::render(sf::RenderTarget& target) const
{
	for (auto& [system, _] : _systems.at(Phase::OnRender))
	{
		system->render(target);
	}
}
