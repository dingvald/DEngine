#include "pch.h"
#include "SystemScheduler.h"

void drft::system::SystemScheduler::startAll(bool isNewGame) const
{
	std::cout << "Starting Systems..." << std::endl;
	for (auto& system : _systems)
	{
		std::string typeName = typeid(*system).name();
		std::cout << "Starting " << typeName << "..." << std::endl;
		system->onStart(isNewGame);
	}
}

void drft::system::SystemScheduler::initAll() const
{
	std::cout << "Initializing Systems..." << std::endl;
	for (auto& system : _systems)
	{
		std::string typeName = typeid(*system).name();
		std::cout << "Initializing " << typeName << "..." << std::endl;
		system->init();
	}
}

void drft::system::SystemScheduler::loadAll(cereal::JSONInputArchive& iarchive)
{
	for (auto& system : _systems)
	{
		std::string typeName = typeid(*system).name();
		std::cout << "Loading " << typeName << "..." << std::endl;
		system->load(iarchive);
	}
}

void drft::system::SystemScheduler::update(const float dt) const
{
	for (auto& system : _systems)
	{
		system->onUpdateBegin();
	}
	for (auto& system : _systems)
	{
		system->onUpdate(dt);
	}
	for (auto& system : _systems)
	{
		system->onUpdateLate(dt);
	}
}

void drft::system::SystemScheduler::updateEnd() const
{
	for (auto& system : _systems)
	{
		system->onUpdateEnd();
	}
}

void drft::system::SystemScheduler::fixedUpdate() const
{
	for (auto& system : _systems)
	{
		system->onFixedUpdateBegin();
	}
	for (auto& system : _systems)
	{
		system->onFixedUpdate();
	}
	for (auto& system : _systems)
	{
		system->onFixedUpdateLate();
	}
}

void drft::system::SystemScheduler::fixedUpdateEnd() const
{
	for (auto& system : _systems)
	{
		system->onFixedUpdateEnd();
	}
}

void drft::system::SystemScheduler::render(sf::RenderTarget& target) const
{
	for (auto& system : _systems)
	{
		system->render(target);
	}
}

void drft::system::SystemScheduler::saveAll(cereal::JSONOutputArchive& oarchive)
{
	for (auto& system : _systems)
	{
		std::string typeName = typeid(*system).name();
		std::cout << "Saving " << typeName << "..." << std::endl;
		system->save(oarchive);
	}
}

void drft::system::SystemScheduler::shutdownAll()
{
	std::cout << "Shutting Systems Down..." << std::endl;
	for (auto& system : _systems)
	{
		std::string typeName = typeid(*system).name();
		std::cout << "Shutdown " << typeName << "..." << std::endl;
		system->shutdown();
	}
}
