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

void drft::system::SystemScheduler::loadAll(cereal::JSONInputArchive& iarchive)
{
	for (auto& [_, systems] : _systems)
	{
		for (auto& [system, _] : systems)
		{
			system->load(iarchive);
			std::string typeName = typeid(*system).name();
			std::cout << "Loading " << typeName << "..." << std::endl;
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

void drft::system::SystemScheduler::saveAll(cereal::JSONOutputArchive& oarchive)
{
	for (auto& [_, systems] : _systems)
	{
		for (auto& [system, _] : systems)
		{
			system->save(oarchive);
			std::string typeName = typeid(*system).name();
			std::cout << "Saving " << typeName << "..." << std::endl;
		}
	}
}

void drft::system::SystemScheduler::shutdownAll()
{
	std::cout << "Shutting Systems Down..." << std::endl;
	for (auto& [_, systems] : _systems)
	{
		for (auto& [system, _] : systems)
		{
			system->shutdown();
			std::string typeName = typeid(*system).name();
			std::cout << "Shutdown " << typeName << "..." << std::endl;
		}
	}
}
