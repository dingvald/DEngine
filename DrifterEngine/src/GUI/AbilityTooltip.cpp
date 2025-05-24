#include "pch.h"
#include "AbilityTooltip.h"
#include <Defines/CommonGuiColors.h>
#include <Ability/IAbility.h>

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
    nameLabel->setText(ability.getName());
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

    auto descriptionLabel = tgui::RichTextLabel::create();
    descriptionLabel->setPosition("50%", tgui::bindBottom(separatorLine) + 8);
    descriptionLabel->setText(ability.getContextualDescription(actor));
    descriptionLabel->setMaximumTextWidth(panel->getInnerSize().x - 8.f);
    descriptionLabel->setTextSize(14);
    descriptionLabel->setOrigin(0.5f, 0.f);
    descriptionLabel->getRenderer()->setTextColor(tgui::Color{100,100,100});
    descriptionLabel->getRenderer()->setTextStyle(tgui::TextStyle::Italic);
    descriptionLabel->setHorizontalAlignment(tgui::HorizontalAlignment::Center);
    panel->add(descriptionLabel);

    panel->setHeight(tgui::bindBottom(descriptionLabel) - tgui::bindTop(nameLabel) + 16);
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