#include "pch.h"
#include "DayNightCycleSystem.h"
#include "Components/CameraComponent.h"
#include "Components/GlobalLightSourceComponent.h"
#include "Events/DayStartEvent.h"
#include "Events/NightStartEvent.h"
#include "Events/SendFloatingMessageEvent.h"
#include "Spatial/Conversions.h"
#include "Utility/SmoothTransition.h"
#include "Systems/Helpers/GetCurrentCamera.h"
#include "Services/DebugInfo.h"

static constexpr int DAY_START_HOUR = 5;
static constexpr int NIGHT_START_HOUR = 23;

static constexpr int SECONDS_PER_TICK = 10;
static constexpr int SECONDS_PER_MINUTE = 60;
static constexpr int MINUTES_PER_HOUR = 60;
static constexpr int HOURS_PER_DAY = 24;

static const sf::Color NIGHT_COLOR = { 30,30,50 };
static const sf::Color DAY_COLOR = { 225,225,225 };

void drft::system::DayNightCycleSystem::init()
{
	_dispatcher.sink<events::GameTickEvent>().connect<&DayNightCycleSystem::onGameTickEvent>(this);
}

void drft::system::DayNightCycleSystem::onFixedUpdate()
{
	const auto color = determineGlobalIllumination();
	auto cameraView = _registry.view<CameraComponent>();
	for (auto entity : cameraView)
	{
		_registry.emplace_or_replace<GlobalLightSourceComponent>(entity, color);
	}

	auto time = std::format("Day {} - {}:{:02} {}", _days, _hours > 12 ? _hours - 12 : _hours, _minutes, _hours >= 12 ? "pm" : "am");
	service::DebugInfo::instance().putInfo("Time", time);
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
	_seconds += SECONDS_PER_TICK;
	if (_seconds >= SECONDS_PER_MINUTE)
	{
		++_minutes;
		_seconds -= SECONDS_PER_MINUTE;
	}
	if (_minutes >= MINUTES_PER_HOUR)
	{
		++_hours;
		_minutes = 0;
	}
	if (_hours >= HOURS_PER_DAY)
	{
		++_days;
		_hours = 0;
	}

	if (_hours == NIGHT_START_HOUR && _minutes == 0 && _seconds == 0)
	{
		auto camera = getCurrentCamera(_registry);
		_dispatcher.trigger(events::NightStartEvent());
		_dispatcher.trigger(events::SendFloatingMessageEvent{
			.message = "Dusk has fallen...",
			.color = sf::Color(125,0,255),
			.position = camera.position,
			.velocity = {0,0},
			.isScreenSpace = true,
			.ttl = 120
			});
	}
	else if (_hours == DAY_START_HOUR && _minutes == 0 && _seconds == 0)
	{
		auto camera = getCurrentCamera(_registry);
		_dispatcher.trigger(events::DayStartEvent());
		_dispatcher.trigger(events::SendFloatingMessageEvent{
			.message = "Dawn has broken...",
			.color = sf::Color::Yellow,
			.position = camera.position,
			.velocity = {0,0},
			.isScreenSpace = true,
			.ttl = 120
			});
	}
}

sf::Color drft::system::DayNightCycleSystem::determineGlobalIllumination() const
{
	sf::Color result = { sf::Color::White };

	// Night
	if ((_hours >= NIGHT_START_HOUR && _hours < 24) || (_hours >= 0 && _hours < DAY_START_HOUR))
	{
		result = NIGHT_COLOR;
	}
	// First Dawn
	else if (_hours >= DAY_START_HOUR && _hours < 8)
	{
		util::SmoothColorTransition color(NIGHT_COLOR, { 150,150,255 }, 
			DAY_START_HOUR * MINUTES_PER_HOUR, 8 * MINUTES_PER_HOUR);
		result = color.compute(_hours * MINUTES_PER_HOUR + _minutes);
	}
	// Early Dawn
	else if (_hours >= 8 && _hours < 10)
	{
		util::SmoothColorTransition color({ 150,150,225 }, DAY_COLOR, 
			8*MINUTES_PER_HOUR, 10 * MINUTES_PER_HOUR);
		result = color.compute(_hours * MINUTES_PER_HOUR + _minutes);
	}
	// Early Dusk
	else if (_hours >= 18 && _hours < 20)
	{
		util::SmoothColorTransition color(DAY_COLOR, { 225,200,100 }, 
			18 * MINUTES_PER_HOUR, 20 * MINUTES_PER_HOUR);
		result = color.compute(_hours * MINUTES_PER_HOUR + _minutes);
	}
	// Late Dusk
	else if (_hours >= 20 && _hours < NIGHT_START_HOUR)
	{
		util::SmoothColorTransition color({ 255,200,100 }, NIGHT_COLOR, 
			20 * MINUTES_PER_HOUR, NIGHT_START_HOUR * MINUTES_PER_HOUR);
		result = color.compute(_hours * MINUTES_PER_HOUR + _minutes);
	}

	return result;
}
