#pragma once
class ItemTooltip
{
public:
	ItemTooltip(entt::const_handle item, tgui::Group::Ptr gui);
	~ItemTooltip();

	void update();

	/// <summary>
	/// Sets the time it takes for the tooltip to appear
	/// </summary>
	/// <param name="delay">- Time in # of update calls</param>
	void setDelayTime(unsigned int delay);
	void setPosition(sf::Vector2i position);

private:
	unsigned int _time = 0u;
	unsigned int _delay = 0u;
	entt::const_handle _item;
	tgui::Group::Ptr _tooltip;
};

