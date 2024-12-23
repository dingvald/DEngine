#include "pch.h"
#include "DayNightCycleSystem.h"
#include "Components/CameraComponent.h"
#include "Components/GlobalLightSourceComponent.h"
#include <Components/DateAndTimeTrackerComponent.h>
#include "Events/DayStartEvent.h"
#include "Events/NightStartEvent.h"
#include "Events/SendFloatingMessageEvent.h"
#include "Spatial/Conversions.h"
#include <Spatial/Helpers.h>
#include "Utility/SmoothTransition.h"
#include "Systems/Helpers/GetCurrentCamera.h"
#include "Services/DebugInfo.h"

static constexpr int DAY_START_HOUR = 5;
static constexpr int NIGHT_START_HOUR = 23;

static constexpr int SECONDS_PER_TICK = 20;
static constexpr int SECONDS_PER_MINUTE = 60;
static constexpr int MINUTES_PER_HOUR = 60;
static constexpr int HOURS_PER_DAY = 24;

static const sf::Color NIGHT_COLOR = { 30,30,50 };
static const sf::Color DAY_COLOR = { 225,225,225 };

void drft::system::DayNightCycleSystem::init()
{
	_dispatcher.sink<events::GameTickEvent>()
		.connect<&DayNightCycleSystem::onGameTickEvent>(this);
	_registry.on_construct<DateAndTimeTrackerComponent>()
		.connect<&DayNightCycleSystem::onConstructDateAndTimeTracker>(this);
}

void drft::system::DayNightCycleSystem::start()
{
	if (_dateAndTimeTracker == entt::null)
	{
		_dateAndTimeTracker = _registry.create();
		_registry.emplace<DateAndTimeTrackerComponent>(_dateAndTimeTracker, 0, 12, 0, 0);
	}
}

void drft::system::DayNightCycleSystem::update()
{
	const auto color = determineGlobalIllumination();
	auto cameraView = _registry.view<CameraComponent>();
	for (auto entity : cameraView)
	{
		_registry.emplace_or_replace<GlobalLightSourceComponent>(entity, color);
	}

	auto& time = _registry.get<DateAndTimeTrackerComponent>(_dateAndTimeTracker);
	auto timeMessage = std::format("Day {} - {}:{:02} {}", 
		time.days, 
		time.hours > 12 ? time.hours - 12 : time.hours, 
		time.minutes, 
		time.hours >= 12 ? "pm" : "am");
	service::DebugInfo::instance().putInfo("Time", timeMessage);
}

void drft::system::DayNightCycleSystem::onGameTickEvent(const events::GameTickEvent& ev)
{
	auto& time = _registry.get<DateAndTimeTrackerComponent>(_dateAndTimeTracker);
	time.seconds += SECONDS_PER_TICK;
	if (time.seconds >= SECONDS_PER_MINUTE)
	{
		++time.minutes;
		time.seconds -= SECONDS_PER_MINUTE;
	}
	if (time.minutes >= MINUTES_PER_HOUR)
	{
		++time.hours;
		time.minutes = 0;
	}
	if (time.hours >= HOURS_PER_DAY)
	{
		++time.days;
		time.hours = 0;
	}

	if (time.hours == NIGHT_START_HOUR && time.minutes == 0 && time.seconds == 0)
	{
		auto camera = getCurrentCamera(_registry);
		_dispatcher.trigger(events::NightStartEvent());
		_dispatcher.trigger(events::SendFloatingMessageEvent{
			.message = "Dusk has fallen...",
			.color = sf::Color(125,0,255),
			.position = spatial::toXY(spatial::toFloatSpace(spatial::asTileSpace(camera.position))),
			.velocity = {0,0},
			.isScreenSpace = true,
			.ttl = 120
			});
	}
	else if (time.hours == DAY_START_HOUR && time.minutes == 0 && time.seconds == 0)
	{
		auto camera = getCurrentCamera(_registry);
		_dispatcher.trigger(events::DayStartEvent());
		_dispatcher.trigger(events::SendFloatingMessageEvent{
			.message = "Dawn has broken...",
			.color = sf::Color::Yellow,
			.position = spatial::toXY(spatial::toFloatSpace(spatial::asTileSpace(camera.position))),
			.velocity = {0,0},
			.isScreenSpace = true,
			.ttl = 120
			});
	}
}

void drft::system::DayNightCycleSystem::onConstructDateAndTimeTracker(entt::registry& registry, entt::entity entity)
{
	_dateAndTimeTracker = entity;
}

sf::Color drft::system::DayNightCycleSystem::determineGlobalIllumination() const
{
	sf::Color result = { sf::Color::White };

	auto& time = _registry.get<DateAndTimeTrackerComponent>(_dateAndTimeTracker);
	// Night
	if ((time.hours >= NIGHT_START_HOUR && time.hours < 24) || (time.hours >= 0 && time.hours < DAY_START_HOUR))
	{
		result = NIGHT_COLOR;
	}
	// First Dawn
	else if (time.hours >= DAY_START_HOUR && time.hours < 8)
	{
		util::SmoothColorTransition color(NIGHT_COLOR, { 150,150,255 }, 
			DAY_START_HOUR * MINUTES_PER_HOUR, 8 * MINUTES_PER_HOUR);
		result = color.compute(time.hours * MINUTES_PER_HOUR + time.minutes);
	}
	// Early Dawn
	else if (time.hours >= 8 && time.hours < 10)
	{
		util::SmoothColorTransition color({ 150,150,225 }, DAY_COLOR, 
			8*MINUTES_PER_HOUR, 10 * MINUTES_PER_HOUR);
		result = color.compute(time.hours * MINUTES_PER_HOUR + time.minutes);
	}
	// Early Dusk
	else if (time.hours >= 18 && time.hours < 20)
	{
		util::SmoothColorTransition color(DAY_COLOR, { 225,200,100 }, 
			18 * MINUTES_PER_HOUR, 20 * MINUTES_PER_HOUR);
		result = color.compute(time.hours * MINUTES_PER_HOUR + time.minutes);
	}
	// Late Dusk
	else if (time.hours >= 20 && time.hours < NIGHT_START_HOUR)
	{
		util::SmoothColorTransition color({ 255,200,100 }, NIGHT_COLOR, 
			20 * MINUTES_PER_HOUR, NIGHT_START_HOUR * MINUTES_PER_HOUR);
		result = color.compute(time.hours * MINUTES_PER_HOUR + time.minutes);
	}

	return result;
}
