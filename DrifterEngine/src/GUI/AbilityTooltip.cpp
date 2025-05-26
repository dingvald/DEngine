#include "pch.h"
#include "AbilityTooltip.h"
#include <Defines/CommonGuiColors.h>
#include <Ability/IAbility.h>
#include <Utility/TGUIHelpers.h>
#include <Utility/StringManipulation.h>

static const sf::Color TooltipStatNameColor = { 119, 179, 110 };

namespace Internal
{
    static std::string buildAbilityRangeString(const drft::IAbility& ability, entt::const_handle actor)
    {
        auto range = ability.getRange(actor);
        if (range.getMin() == range.getMax())
        {
            return std::format("{} meters", drft::GuiHelpers::colorizedString(std::to_string(range.getMax()), guiColor::VariableGreen));
        }
        else
        {
            return std::format(
                "{}-{} meters", 
                drft::GuiHelpers::colorizedString(std::to_string(range.getMin()), guiColor::VariableGreen),
                drft::GuiHelpers::colorizedString(std::to_string(range.getMax()), guiColor::VariableGreen));
        }
    }
    static std::string buildUseSpeedString(const drft::IAbility& ability, entt::const_handle)
    {
        if (ability.getTimeCost() == 0) return "instant";
        return std::to_string(ability.getTimeCost());
    }
    static std::string buildTargetingTypeString(const drft::IAbility& ability, entt::const_handle actor)
    {
        auto targeting = ability.getTargetingType();
        switch (targeting)
        {
        case drft::AbilityTargetingType::Self:
            return "self";
        case drft::AbilityTargetingType::Auto:
            return "auto";
        case drft::AbilityTargetingType::SelectDirection:
            return "directional";
        case drft::AbilityTargetingType::SelectSquare:
            return "selection";
        default:
            break;
        }
    }
    static tgui::RichTextLabel::Ptr buildResourceCostWidget(drft::AbilityResourceType resourceType, float cost, tgui::Widget::Ptr widgetAbove)
    {
        auto resourceCostLabel = tgui::RichTextLabel::create();
        resourceCostLabel->setPosition(4, tgui::bindBottom(widgetAbove));
        resourceCostLabel->setText(
            drft::GuiHelpers::colorizedString(std::format("{} Cost: ", drft::AbilityResourceCostToString.at(resourceType)), TooltipStatNameColor)
            + std::format("{:.1f}", cost));
        resourceCostLabel->setTextSize(14);
        resourceCostLabel->getRenderer()->setTextColor(tgui::Color{ 100,100,100 });
        resourceCostLabel->setHorizontalAlignment(tgui::HorizontalAlignment::Left);
        return resourceCostLabel;
    }
}

AbilityTooltip::AbilityTooltip(const drft::IAbility& ability, entt::const_handle actor, tgui::Group::Ptr gui)
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

    auto nameLabel = tgui::RichTextLabel::create();
    nameLabel->setPosition("50%", tgui::bindTop(panel));
    nameLabel->setText(drft::util::capitalizeAll(ability.getName()));
    nameLabel->setTextSize(20);
    nameLabel->setOrigin(0.5f, 0.0f);
    nameLabel->setVerticalAlignment(tgui::VerticalAlignment::Center);
    nameLabel->setHorizontalAlignment(tgui::HorizontalAlignment::Center);
    panel->add(nameLabel);

    auto separatorLine = tgui::SeparatorLine::create();
    separatorLine->setPosition("0%", tgui::bindBottom(nameLabel));
    separatorLine->setSize(panel->getSize().x, 1);
    separatorLine->getRenderer()->setColor(guiColor::TranslucentAsh);
    panel->add(separatorLine);

    // Use mode
    auto useModeLabel = tgui::RichTextLabel::create();
    useModeLabel->setPosition(4, tgui::bindBottom(separatorLine));
    useModeLabel->setText(
        drft::GuiHelpers::colorizedString("Use Mode: ", TooltipStatNameColor)
        + drft::AbilityUseModeToString.at(ability.getUseMode()));
    useModeLabel->setMaximumTextWidth(panel->getInnerSize().x - 8.f);
    useModeLabel->setTextSize(14);
    useModeLabel->getRenderer()->setTextColor(tgui::Color{ 100,100,100 });
    useModeLabel->setHorizontalAlignment(tgui::HorizontalAlignment::Left);
    panel->add(useModeLabel);

    // Targeting Type
    auto targetingTypeLabel = tgui::RichTextLabel::create();
    targetingTypeLabel->setPosition(4, tgui::bindBottom(useModeLabel));
    targetingTypeLabel->setText(
        drft::GuiHelpers::colorizedString("Targeting: ", TooltipStatNameColor)
        + Internal::buildTargetingTypeString(ability, actor));
    targetingTypeLabel->setMaximumTextWidth(panel->getInnerSize().x - 8.f);
    targetingTypeLabel->setTextSize(14);
    targetingTypeLabel->getRenderer()->setTextColor(tgui::Color{ 100,100,100 });
    targetingTypeLabel->setHorizontalAlignment(tgui::HorizontalAlignment::Left);
    panel->add(targetingTypeLabel);

    // Costs
    auto resourceCosts = ability.getResourceCosts(actor);
    tgui::Widget::Ptr widgetAbove = targetingTypeLabel;
    for (auto&& [resourceType, cost] : resourceCosts)
    {
        auto newWidget = Internal::buildResourceCostWidget(resourceType, cost, widgetAbove);
        newWidget->setMaximumTextWidth(panel->getInnerSize().x - 8.f);
        panel->add(newWidget);
        widgetAbove = newWidget;
    }

    // Range
    auto rangeLabel = tgui::RichTextLabel::create();
    rangeLabel->setPosition(4, tgui::bindBottom(widgetAbove));
    rangeLabel->setText(
        drft::GuiHelpers::colorizedString("Range: ", TooltipStatNameColor)
        + Internal::buildAbilityRangeString(ability, actor));
    rangeLabel->setMaximumTextWidth(panel->getInnerSize().x - 8.f);
    rangeLabel->setTextSize(14);
    rangeLabel->getRenderer()->setTextColor(tgui::Color{ 100,100,100 });
    rangeLabel->setHorizontalAlignment(tgui::HorizontalAlignment::Left);
    panel->add(rangeLabel);

    // Use speed
    auto useSpeedLabel = tgui::RichTextLabel::create();
    useSpeedLabel->setPosition(4, tgui::bindBottom(rangeLabel));
    useSpeedLabel->setText(
        drft::GuiHelpers::colorizedString("Use Speed: ", TooltipStatNameColor)
        + Internal::buildUseSpeedString(ability, actor));
    useSpeedLabel->setMaximumTextWidth(panel->getInnerSize().x - 8.f);
    useSpeedLabel->setTextSize(14);
    useSpeedLabel->getRenderer()->setTextColor(tgui::Color{ 100,100,100 });
    useSpeedLabel->setHorizontalAlignment(tgui::HorizontalAlignment::Left);
    panel->add(useSpeedLabel);

    // Description
    auto descriptionLabel = tgui::RichTextLabel::create();
    descriptionLabel->setPosition(4, tgui::bindBottom(useSpeedLabel));
    descriptionLabel->setText(
        drft::GuiHelpers::colorizedString("Description: ", TooltipStatNameColor)
        + ability.getContextualDescription(actor));
    descriptionLabel->setMaximumTextWidth(panel->getInnerSize().x - 8.f);
    descriptionLabel->setTextSize(14);
    descriptionLabel->getRenderer()->setTextColor(tgui::Color{100,100,100});
    descriptionLabel->setHorizontalAlignment(tgui::HorizontalAlignment::Left);
    panel->add(descriptionLabel);

    panel->setHeight(tgui::bindBottom(descriptionLabel) - tgui::bindTop(nameLabel) + 2);
}

AbilityTooltip::~AbilityTooltip()
{
    if (auto parent = _tooltip->getParent())
        parent->remove(_tooltip);
}

void AbilityTooltip::update()
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

void AbilityTooltip::setDelayTime(unsigned int delay)
{
    _delay = delay;
}

void AbilityTooltip::setPosition(sf::Vector2i position)
{
    tgui::Vector2f pos = { static_cast<float>(position.x), static_cast<float>(position.y) };
    _tooltip->setPosition(pos);
}