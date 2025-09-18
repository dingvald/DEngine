#include "pch.h"
#include "ItemTooltip.h"
#include <Defines/CommonGuiColors.h>
#include <Components/SharpnessComponent.h>
#include <Utility/EntityHelpers.h>
#include <Utility/EntityAccessors/GetEntityDescription.h>
#include <Utility/EntityAccessors/GetEntityMaterials.h>
#include <Utility/EntityAccessors/GetEntityWeight.h>
#include <Utility/EntityAccessors/GetEntityName.h>
#include <Utility/TGUIHelpers.h>
#include <Systems/Helpers/GetItemAttackValues.h>

namespace Internal
{
	static inline const std::unordered_map<SharpnessLevel, tgui::Color> SharpnessColorMap =
	{
		{ SharpnessLevel::Red,    tgui::Color::Red },
		{ SharpnessLevel::Orange, tgui::Color{ 255, 165, 0} },
		{ SharpnessLevel::Yellow, tgui::Color::Yellow },
		{ SharpnessLevel::Green,  tgui::Color::Green },
		{ SharpnessLevel::Blue,   tgui::Color::Blue },
		{ SharpnessLevel::Purple, tgui::Color{ 165, 0, 255 } },
		{ SharpnessLevel::White,  tgui::Color::White }
	};

	static tgui::Panel::Ptr createSharpnessBar(const SharpnessComponent& sharpComponent, const tgui::Layout2d& size)
	{
		auto container = tgui::Panel::create();
		container->setSize(size);
		container->setOrigin(0.f, 0.5f);
		container->getRenderer()->setBorderColor(guiColor::TranslucentAsh);
		container->getRenderer()->setBorders({ 2, 2 });

		float totalWidth = 0.f;
		for (size_t i = 0; i < sharpComponent.levels.size(); i++)
		{
			auto bar = tgui::Panel::create();
			
			float width = (sharpComponent.levels[i] / 100.f) * container->getSize().x;
			totalWidth += width;
			bar->setSize(totalWidth, "100%");

			bar->getRenderer()->setBackgroundColor(SharpnessColorMap.at(static_cast<SharpnessLevel>(i)));
			
			container->add(bar);
			container->moveWidgetToBack(bar);
		}

		return container;
	}
}

ItemTooltip::ItemTooltip(entt::const_handle item, entt::const_handle actor, tgui::Group::Ptr gui)
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

	tgui::Widget::Ptr lastWidget = itemNameLabel;

	auto separatorLine1 = tgui::SeparatorLine::create();
	separatorLine1->setPosition("0%", tgui::bindBottom(lastWidget));
	separatorLine1->setSize(panel->getSize().x, 1);
	separatorLine1->getRenderer()->setColor(guiColor::TranslucentAsh);
	panel->add(separatorLine1);
	lastWidget = separatorLine1;

	auto weightLabel = tgui::RichTextLabel::create();
	weightLabel->setPosition("50%", tgui::bindBottom(lastWidget) + 4);
	weightLabel->setText(
		drft::GuiHelpers::colorizedString("Weight: ", guiColor::TooltipStatNameColor)
		+ std::format("{:.1f} kg", drft::util::getEntityWeight(item)));
	weightLabel->setTextSize(14);
	weightLabel->setOrigin(0.5f, 0.f);
	weightLabel->setMaximumTextWidth(panel->getInnerSize().x - 8.f);
	weightLabel->getRenderer()->setTextColor(tgui::Color{ 100,100,100 });
	panel->add(weightLabel);
	lastWidget = weightLabel;

	AttackValues attackValues = getItemAttackValues(item, actor);

	auto damageLabel = tgui::RichTextLabel::create();
	damageLabel->setPosition("50%", tgui::bindBottom(lastWidget));
	damageLabel->setText(
		drft::GuiHelpers::colorizedString("Damage: ", guiColor::TooltipStatNameColor)
		+ std::format("{:.1f} - {:.1f} ", attackValues.baseDamageRange.getMin(), attackValues.baseDamageRange.getMax()));
	damageLabel->setTextSize(14);
	damageLabel->setOrigin(0.5f, 0.f);
	damageLabel->setMaximumTextWidth(panel->getInnerSize().x - 8.f);
	damageLabel->getRenderer()->setTextColor(tgui::Color{ 100,100,100 });
	panel->add(damageLabel);
	lastWidget = damageLabel;

	if (auto* sharpness = item.try_get<SharpnessComponent>())
	{
		auto sharpnessLabel = tgui::RichTextLabel::create();
		sharpnessLabel->setPosition("50%", tgui::bindBottom(lastWidget));
		sharpnessLabel->setText(drft::GuiHelpers::colorizedString("Sharpness: ", guiColor::TooltipStatNameColor));
		sharpnessLabel->setTextSize(14);
		sharpnessLabel->setOrigin(0.5f, 0.f);
		sharpnessLabel->setMaximumTextWidth(panel->getInnerSize().x - 8.f);
		sharpnessLabel->getRenderer()->setTextColor(tgui::Color{ 100,100,100 });
		panel->add(sharpnessLabel);
		lastWidget = sharpnessLabel;

		auto sharpnessBar = Internal::createSharpnessBar(*sharpness, {panel->getSize().x / 2, 10});
		sharpnessBar->setPosition(tgui::bindPosX(lastWidget) - 32.f, tgui::bindPosY(lastWidget) + 10.f);
		panel->add(sharpnessBar);
	}

	auto materialsList = tgui::RichTextLabel::create();
	materialsList->setPosition("50%", tgui::bindBottom(lastWidget));
	materialsList->setTextSize(14);
	materialsList->setOrigin(0.5f, 0.f);
	materialsList->setMaximumTextWidth(panel->getInnerSize().x - 8.f);
	materialsList->getRenderer()->setTextColor(tgui::Color{ 100,100,100 });
	std::stringstream ss;

	auto materials = drft::util::getEntityMaterialPercentages(item);
	ss << drft::GuiHelpers::colorizedString("Composed of: ", guiColor::TooltipStatNameColor);
	if (materials.empty())
	{
		auto renderData = drft::util::getRenderData(item);
		ss << std::format("100% {}", drft::GuiHelpers::colorizedString(drft::util::getEntityName(item), renderData.color));
	}
	else
	{
		for (auto&& [material, percentage] : materials)
		{
			auto renderData = drft::util::getRenderData(material);
			ss << std::format("{:.0f}% {}", percentage * 100.f, drft::GuiHelpers::colorizedString(drft::util::getEntityName(material), renderData.color));
		}
	}
	
	materialsList->setText(ss.str());
	materialsList->setVerticalAlignment(tgui::VerticalAlignment::Center);
	panel->add(materialsList);
	lastWidget = materialsList;

	auto descriptionLabel = tgui::RichTextLabel::create();
	descriptionLabel->setPosition("50%", tgui::bindBottom(lastWidget));
	descriptionLabel->setText(
		drft::GuiHelpers::colorizedString("Description: ", guiColor::TooltipStatNameColor)
		+ drft::util::getEntityDescription(item));
	descriptionLabel->setMaximumTextWidth(panel->getInnerSize().x - 8.f);
	descriptionLabel->setTextSize(14);
	descriptionLabel->setOrigin(0.5f, 0.f);
	descriptionLabel->getRenderer()->setTextColor(tgui::Color{ 100,100,100 });
	panel->add(descriptionLabel);
	lastWidget = descriptionLabel;

	panel->setHeight(tgui::bindBottom(lastWidget) - tgui::bindTop(itemNameLabel) + 8);
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
