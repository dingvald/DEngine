#pragma once
#include "Systems/System.h"
#include "Events/DayStartEvent.h"
#include "Events/NightStartEvent.h"
#include "Events/GameTickEvent.h"


namespace drft::system
{
    class HorrorSpawningSystem : public System
    {
    public:
        void init() override;
        void update(const float dt) override;

    private:
        void onDayStartEvent(events::DayStartEvent& ev);
        void onNightStartEvent(events::NightStartEvent& ev);
        void onGameTickEvent(events::GameTickEvent& ev);

        void spawnHorror() const;

    private:
        bool _isNight = false;
    };
}


