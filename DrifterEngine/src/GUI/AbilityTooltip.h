#pragma once

namespace drft
{
    class IAbility;
}


class AbilityTooltip
{
public:
    AbilityTooltip(const drft::IAbility& ability, entt::const_handle actor, tgui::Group::Ptr gui);
    ~AbilityTooltip();

    void update();
    void setDelayTime(unsigned int delay);
    void setPosition(sf::Vector2i position, sf::Vector2f origin = {0.f, 0.f});

private:
    tgui::Group::Ptr _tooltip;
    unsigned int _delay = 0;
    unsigned int _time = 0;
};