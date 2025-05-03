#pragma once
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"
#include <Components/ItemComponent.h>

struct DateAndTimeTrackerComponent
{
	int days = 0;
	int hours = 0;
	int minutes = 0;
	int seconds = 0;

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "date_and_time_tracker";
	static void bind()
	{
		using namespace entt::literals;
		snapshot::reflectComponent<DateAndTimeTrackerComponent, NAME>()
			.prop("serialize"_hs);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, DateAndTimeTrackerComponent& d)
	{
		archive(d.days, d.hours, d.minutes, d.seconds);
	}
}