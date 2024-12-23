#include "pch.h"
#include "SystemScheduler.h"

void drft::system::SystemScheduler::startAll() const
{
	std::cout << "Starting Systems..." << std::endl;
	for (auto& system : _systems)
	{
		std::string typeName = typeid(*system).name();
		std::cout << "Starting " << typeName << "..." << std::endl;
		system->start();
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


void drft::system::SystemScheduler::update() const
{
	for (auto& system : _systems)
	{
		system->updateBegin();
	}
	for (auto& system : _systems)
	{
		system->update();
	}
	for (auto& system : _systems)
	{
		system->updateLate();
	}
	for (auto& system : _systems)
	{
		system->updateEnd();
	}
}

void drft::system::SystemScheduler::render(sf::RenderTarget& target) const
{
	for (auto& system : _systems)
	{
		system->render(target);
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
