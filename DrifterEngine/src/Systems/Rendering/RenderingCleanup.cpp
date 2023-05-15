#include "pch.h"
#include "RenderingCleanup.h"
#include "Components/Components.h"
#include "Components/Tags.h"

void drft::system::RenderingCleanup::init()
{
}

void drft::system::RenderingCleanup::render(sf::RenderTarget& target)
{
	registry->clear<component::Lit>();
	registry->clear<component::tag::InViewport>();
}
