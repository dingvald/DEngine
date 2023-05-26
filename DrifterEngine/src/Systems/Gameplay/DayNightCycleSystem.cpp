#include "pch.h"
#include "DayNightCycleSystem.h"
#include "Components/Components.h"
#include "Events/DayStartEvent.h"
#include "Events/NightStartEvent.h"
#include "Utility/SmoothTransition.h"

static constexpr int DAY_START_HOUR = 5;
static constexpr int NIGHT_START_HOUR = 23;

void drft::system::DayNightCycleSystem::init()
{
	auto& dispatcher = registry->ctx().get<entt::dispatcher&>();
	dispatcher.sink<events::GameTickEvent>().connect<&DayNightCycleSystem::onGameTickEvent>(this);
}

void drft::system::DayNightCycleSystem::fixedUpdate()
{
	const auto color = determineSunColor();
	auto cameraView = registry->view<component::Camera>();
	for (auto entity : cameraView)
	{
		registry->emplace_or_replace<component::GlobalLightSource>(entity, color);
	}
}

void drft::system::DayNightCycleSystem::save(cereal::JSONOutputArchive& oarchive)
{
	oarchive(cereal::make_nvp("Days", _days));
	oarchive(cereal::make_nvp("Hours", _hours));
	oarchive(cereal::make_nvp("Minutes", _minutes));
	oarchive(cereal::make_nvp("Seconds", _seconds));
}

void drft::system::DayNightCycleSystem::load(cereal::JSONInputArchive& iarchive)
{
	iarchive(cereal::make_nvp("Days", _days));
	iarchive(cereal::make_nvp("Hours", _hours));
	iarchive(cereal::make_nvp("Minutes", _minutes));
	iarchive(cereal::make_nvp("Seconds", _seconds));
}

void drft::system::DayNightCycleSystem::onGameTickEvent(const events::GameTickEvent& ev)
{
	_seconds += 30;
	if (_seconds >= 60)
	{
		++_minutes;
		_seconds -= 60;
	}
	if (_minutes >= 60)
	{
		++_hours;
		_minutes = 0;
	}
	if (_hours >= 24)
	{
		++_days;
		_hours = 0;
	}

	if (_hours == NIGHT_START_HOUR && _minutes == 0 && _seconds == 0)
	{
		auto& dispatcher = registry->ctx().get<entt::dispatcher&>();
		dispatcher.trigger(events::NightStartEvent());
	}
	else if (_hours == DAY_START_HOUR && _minutes == 0 && _seconds == 0)
	{
		auto& dispatcher = registry->ctx().get<entt::dispatcher&>();
		dispatcher.trigger(events::DayStartEvent());
	}
}

sf::Color drft::system::DayNightCycleSystem::determineSunColor() const
{
	sf::Color result = { sf::Color::White };

	// Night
	if ((_hours >= NIGHT_START_HOUR && _hours < 24) || (_hours >= 0 && _hours < DAY_START_HOUR))
	{
		result = sf::Color(30, 30, 50);
	}
	// First Dawn
	else if (_hours >= DAY_START_HOUR && _hours < 8)
	{
		util::SmoothColorTransition color({ 20,20,40 }, { 150,150,255 }, 5*60, 7*60);
		result = color.compute(_hours*60 + _minutes);
	}
	// Early Dawn
	else if (_hours >= 8 && _hours < 10)
	{
		util::SmoothColorTransition color({ 150,150,255 }, { 255,255,255 }, 8*60, 9*60);
		result = color.compute(_hours*60 + _minutes);
	}
	// Early Dusk
	else if (_hours >= 18 && _hours < 20)
	{
		util::SmoothColorTransition color({ 255,255,255 }, { 255,200,100 }, 18*60, 19*60);
		result = color.compute(_hours*60 + _minutes);
	}
	// Late Dusk
	else if (_hours >= 20 && _hours < NIGHT_START_HOUR)
	{
		util::SmoothColorTransition color({ 255,200,100 }, { 30,30,50 }, 20*60, 21*60);
		result = color.compute(_hours*60 + _minutes);
	}

	return result;
}
