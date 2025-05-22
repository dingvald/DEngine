#include "pch.h"
#include "ItemTooltip.h"
#include <Defines/CommonGuiColors.h>
#include <Utility/EntityHelpers.h>
#include <Utility/EntityAccessors/GetEntityDescription.h>
#include <Utility/EntityAccessors/GetEntityMaterials.h>
#include <Utility/EntityAccessors/GetEntityWeight.h>
#include <Utility/EntityAccessors/GetEntityName.h>
#include <Utility/StringManipulation.h>
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

	auto separatorLine1 = tgui::SeparatorLine::create();
	separatorLine1->setPosition("0%", tgui::bindBottom(itemNameLabel));
	separatorLine1->setSize(panel->getSize().x, 1);
	separatorLine1->getRenderer()->setColor(guiColor::TranslucentAsh);
	panel->add(separatorLine1);

	auto descriptionLabel = tgui::Label::create();
	descriptionLabel->setPosition("50%", tgui::bindBottom(separatorLine1) + 8);
	descriptionLabel->setText(drft::util::getEntityDescription(item));
	descriptionLabel->setMaximumTextWidth(panel->getInnerSize().x - 8.f);
	descriptionLabel->setTextSize(14);
	descriptionLabel->setOrigin(0.5f, 0.f);
	descriptionLabel->getRenderer()->setTextColor(tgui::Color{100,100,100});
	descriptionLabel->getRenderer()->setTextStyle(tgui::TextStyle::Italic);
	descriptionLabel->setHorizontalAlignment(tgui::HorizontalAlignment::Center);
	panel->add(descriptionLabel);

	auto weightLabel = tgui::Label::create();
	weightLabel->setPosition("50%", tgui::bindBottom(descriptionLabel) + 8);
	weightLabel->setText(std::format("- Weighs {}kg", drft::util::getEntityWeight(item)));
	weightLabel->setTextSize(14);
	weightLabel->setOrigin(0.5f, 0.f);
	weightLabel->setMaximumTextWidth(panel->getInnerSize().x - 8.f);
	weightLabel->getRenderer()->setTextColor(tgui::Color{ 100,100,100 });
	panel->add(weightLabel);

	auto materialsList = tgui::RichTextLabel::create();
	materialsList->setPosition("50%", tgui::bindBottom(weightLabel) + 2);
	materialsList->setTextSize(14);
	materialsList->setOrigin(0.5f, 0.f);
	materialsList->setMaximumTextWidth(panel->getInnerSize().x - 8.f);
	materialsList->getRenderer()->setTextColor(tgui::Color{ 100,100,100 });
	std::stringstream ss;

	auto materials = drft::util::getEntityMaterialPercentages(item);
	
	if (materials.empty())
	{
		ss << "- Base Material";
	}
	else
	{
		ss << drft::GuiHelpers::colorizedString("- Composed of ", tgui::Color{ 100,100,100 }) << "\n";
		for (size_t i = 0; i < materials.size(); i++)
		{
			auto&& [material, percentage] = materials.at(i);
			auto renderData = drft::util::getRenderData(material);
			ss << "\t- " << percentage * 100.f << "% " << drft::GuiHelpers::colorizedString(drft::util::getEntityName(material), renderData.color);
			if (i < materials.size() - 1)
			{
				ss << "\n";
			}
		}
	}

	materialsList->setText(ss.str());
	materialsList->setVerticalAlignment(tgui::VerticalAlignment::Center);
	panel->add(materialsList);

	auto separatorLine2 = tgui::SeparatorLine::create();
	separatorLine2->setPosition("0%", tgui::bindBottom(materialsList) + 8);
	separatorLine2->setSize(panel->getSize().x, 1);
	separatorLine2->getRenderer()->setColor(guiColor::TranslucentAsh);
	panel->add(separatorLine2);

	panel->setHeight(tgui::bindBottom(separatorLine2) - tgui::bindTop(itemNameLabel) + 8);
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
