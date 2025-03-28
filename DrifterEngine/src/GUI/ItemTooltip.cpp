#include "pch.h"
#include "ItemTooltip.h"
#include <Defines/CommonGuiColors.h>
#include <Utility/EntityHelpers.h>
#include <Utility/EntityAccessors/GetEntityDescription.h>
#include <Utility/EntityAccessors/GetEntityMaterials.h>
#include <Utility/ColorToHexString.h>
#include <Utility/TGUIHelpers.h>

ItemTooltip::ItemTooltip(entt::const_handle item, tgui::Group::Ptr gui)
{
	_tooltip = tgui::Group::create();
	_tooltip->setVisible(false);
	_tooltip->setIgnoreMouseEvents(true);
	gui->add(_tooltip);

	auto panel = tgui::Panel::create();
	panel->setWidth(256);
	panel->getRenderer()->setBackgroundColor(guiColor::BlackAgate);
	panel->getRenderer()->setBorderColor(guiColor::TranslucentAsh);
	panel->getRenderer()->setBorders({ 1, 1 });
	_tooltip->add(panel);

	auto itemNameLabel = tgui::Label::create();
	itemNameLabel->setPosition("50%", tgui::bindTop(panel));
	itemNameLabel->setText(drft::util::getEntityName(item));
	itemNameLabel->setTextSize(20);
	itemNameLabel->setOrigin(0.5f, 0.0f);
	itemNameLabel->setVerticalAlignment(tgui::VerticalAlignment::Center);
	itemNameLabel->setHorizontalAlignment(tgui::HorizontalAlignment::Center);
	panel->add(itemNameLabel);

	auto descriptionLabel = tgui::Label::create();
	descriptionLabel->setPosition("50%", tgui::bindBottom(itemNameLabel) + 8);
	descriptionLabel->setText(drft::util::getEntityDescription(item));
	descriptionLabel->setMaximumTextWidth(panel->getInnerSize().x - 8.f);
	descriptionLabel->setTextSize(14);
	descriptionLabel->setOrigin(0.5f, 0.f);
	descriptionLabel->getRenderer()->setTextColor(tgui::Color{100,100,100});
	descriptionLabel->setHorizontalAlignment(tgui::HorizontalAlignment::Center);
	panel->add(descriptionLabel);

	auto materialsList = tgui::RichTextLabel::create();
	materialsList->setPosition("50%", tgui::bindBottom(descriptionLabel) + 8);
	materialsList->setTextSize(14);
	materialsList->setOrigin(0.5f, 0.f);
	materialsList->setMaximumTextWidth(panel->getInnerSize().x - 8.f);
	materialsList->getRenderer()->setTextColor(tgui::Color{ 100,100,100 });
	std::stringstream ss;
	ss << drft::GuiHelpers::colorizedString("Composed of ", sf::Color::White);
	auto materials = drft::util::getEntityMaterials(item);
	for (size_t i = 0; i < materials.size(); i++)
	{
		auto renderData = drft::util::getRenderData(materials.at(i));
		ss << drft::GuiHelpers::colorizedString(drft::util::getEntityName(materials.at(i)), renderData.color);
		if (i < materials.size() - 1)
		{
			ss << ", ";
		}
	}

	materialsList->setText(ss.str());
	materialsList->setHorizontalAlignment(tgui::HorizontalAlignment::Center);
	materialsList->setVerticalAlignment(tgui::VerticalAlignment::Center);
	panel->add(materialsList);

	auto separatorLine = tgui::SeparatorLine::create();
	separatorLine->setPosition("0%", tgui::bindBottom(materialsList) + 8);
	separatorLine->setSize(panel->getSize().x, 1);
	separatorLine->getRenderer()->setColor(guiColor::TranslucentAsh);
	panel->add(separatorLine);

	panel->setHeight(tgui::bindBottom(separatorLine) - tgui::bindTop(itemNameLabel) + 8);
}

ItemTooltip::~ItemTooltip()
{
	if (auto parent = _tooltip->getParent())
	{
		parent->remove(_tooltip);
	}
}

void ItemTooltip::update()
{
	if (_time == _delay)
	{
		_time = _delay + 1;
		_tooltip->setVisible(true);
	}
	else if (_time < _delay)
	{
		_time++;
	}
}

void ItemTooltip::setDelayTime(unsigned int delay)
{
	_delay = delay;
}

void ItemTooltip::setPosition(sf::Vector2i position)
{
	tgui::Vector2f pos = { static_cast<float>(position.x), static_cast<float>(position.y) };
	_tooltip->setPosition(pos);
}
