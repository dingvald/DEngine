#include "pch.h"
#include "SkillsScreenState.h"

#include <Ability/AbilityRegistry.h>
#include <Defines/CommonGuiColors.h>
#include <Components/Actions/OpenSkillsScreenAction.h>
#include <Components/AbilitiesComponent.h>
#include <Utility/TGUIHelpers.h>
#include <Utility/StandardLogger.h>
#include <Utility/StringManipulation.h>

#pragma optimize("", off)

static const char* w_SkillName = "skill_name";
static const char* w_SkillLevel = "skill_level";
static const char* w_ProgressBar = "progress_bar";

static const char* w_Abilities = "abilities";
static const char* w_AbilityIcon = "ability_icon";

drft::SkillsScreenState::SkillsScreenState(StateStack& stack, StateContext& context)
	: State(stack, context)
{
	determineSessionEntity();

	auto panel = tgui::Panel::create();
	panel->setOrigin(0.5f, 0.5f);
	panel->setPosition("50%, 50%");
	panel->setSize(tgui::bindSize(_guiGroup) * 0.5f);
	panel->getRenderer()->setBackgroundColor(guiColor::BlackAgate);
	_guiGroup->add(panel);

	auto titleBar = tgui::Label::create();
	titleBar->setSize({ tgui::bindWidth(panel), 32 });
	titleBar->setOrigin(0.5f, 1.f);
	titleBar->setPosition(tgui::bindPosX(panel), tgui::bindTop(panel) - 8);
	titleBar->setText("Skills and Abilities");
	titleBar->setTextSize(20);
	titleBar->setVerticalAlignment(tgui::VerticalAlignment::Bottom);
	titleBar->setHorizontalAlignment(tgui::HorizontalAlignment::Center);
	titleBar->getRenderer()->setBackgroundColor(guiColor::BlackAgate);
	_guiGroup->add(titleBar);

	auto split = tgui::SplitContainer::create();
	split->setOrigin(0.5f, 0.5f);
	split->setPosition("50%, 50%");
	split->setSize(tgui::bindSize(panel));
	split->setOrientation(tgui::Orientation::Horizontal);
	split->setSplitterWidth(1.f);
	split->setSplitterOffset("40%");
	split->setMinValidSplitterOffset("40%");
	split->setMaxValidSplitterOffset("65%");
	split->getRenderer()->setSplitterColor(guiColor::TranslucentAsh);
	panel->add(split);

	auto skillsList = tgui::PanelListBox::create();
	skillsList->setItemsHeight(48.f);
	skillsList->getRenderer()->setBackgroundColor(tgui::Color::Transparent);
	split->add(skillsList);

	setupPanelTemplate(skillsList->getPanelTemplate());

	refreshSkillsList(skillsList);

	auto abilities = tgui::HorizontalWrap::create();
	abilities->getRenderer()->setPadding({ 16 });
	abilities->getRenderer()->setSpaceBetweenWidgets(16);
	split->add(abilities, w_Abilities);

	_templateAbilityIcon = tgui::Panel::create();
	setupTemplateAbilityIcon(_templateAbilityIcon);
}

bool drft::SkillsScreenState::handleEvent(const sf::Event& ev)
{
	if (const auto keypressed = ev.getIf<sf::Event::KeyPressed>())
	{
		if (keypressed->code == sf::Keyboard::Key::Escape)
		{
			requestStackPop();
			return true;
		}
	}

	return false;
}

void drft::SkillsScreenState::determineSessionEntity()
{
	auto craftingView = getContext().registry.view<OpenSkillsScreenAction>();
	_sessionEntity = { getContext().registry, craftingView.front() };
	_sessionEntity.remove<OpenSkillsScreenAction>();

	_skills = SkillsWrapper{ _sessionEntity };
}

void drft::SkillsScreenState::setupPanelTemplate(tgui::Panel::Ptr templatePanel)
{
	templatePanel->getRenderer()->setBackgroundColor(tgui::Color::Transparent);

	auto skillName = tgui::Label::create();
	templatePanel->add(skillName, w_SkillName);
	skillName->setVerticalAlignment(tgui::VerticalAlignment::Center);
	skillName->setOrigin(0.f, 0.5f);
	skillName->setTextSize(20);
	skillName->setText("max_length_str");
	skillName->setPosition(8.f, "50%");

	auto progressBar = tgui::ProgressBar::create();
	templatePanel->add(progressBar, w_ProgressBar);
	progressBar->setOrigin(0.f, 0.5f);
	progressBar->setPosition(tgui::bindRight(skillName) + 4.f, "50%");
	progressBar->setMinimum(0);
	progressBar->setMaximum(100);
	progressBar->setSize(256, 14);
	progressBar->getRenderer()->setBorderColor(guiColor::TranslucentAsh);
	progressBar->getRenderer()->setFillColor(guiColor::MGSHudBlue);
	progressBar->getRenderer()->setBackgroundColor(guiColor::TranslucentAsh);

	auto skillLevel = tgui::Label::create();
	templatePanel->add(skillLevel, w_SkillLevel);
	skillLevel->setVerticalAlignment(tgui::VerticalAlignment::Center);
	skillLevel->setOrigin(0.f, 0.5f);
	skillLevel->setTextSize(24);
	skillLevel->setPosition(tgui::bindRight(progressBar) + 24.f, "50%");
}

void drft::SkillsScreenState::refreshSkillsList(tgui::PanelListBox::Ptr skillsList)
{
	skillsList->removeAllItems();

	for (auto&& skill : _skills.getSkills())
	{
		auto newPanel = skillsList->addItem(skill.name());

		auto skillName = newPanel->get<tgui::Label>(w_SkillName);
		skillName->setText(util::capitalizeAll(util::removeUnderscores(skill.name())));
		skillName->setIgnoreMouseEvents(true);

		auto skillLevel = newPanel->get<tgui::Label>(w_SkillLevel);
		skillLevel->setText(std::to_string(skill.level()));
		skillLevel->setIgnoreMouseEvents(true);

		auto progressBar = newPanel->get<tgui::ProgressBar>(w_ProgressBar);
		int progressValue = skill.getProgress() * 100;
		progressBar->setValue(progressValue);
		progressBar->setIgnoreMouseEvents(true);

		auto button = GuiHelpers::buttonizePanel(newPanel);
		button->getRenderer()->setBackgroundColor(tgui::Color::Transparent);

		button->onPress([this, skillId = skill.id()]() { 
			refreshAbilities(_guiGroup->get<tgui::HorizontalWrap>(w_Abilities), skillId); 
		});
	}
}

void drft::SkillsScreenState::setupTemplateAbilityIcon(tgui::Panel::Ptr temp)
{
	temp->setSize({ 64, 64 });
	temp->getRenderer()->setBorderColor(guiColor::TranslucentAsh);
	temp->getRenderer()->setBorders(1);

	auto icon = tgui::Picture::create();
	icon->setSize(tgui::bindSize(temp));
	temp->add(icon, w_AbilityIcon);
}

void drft::SkillsScreenState::refreshAbilities(tgui::HorizontalWrap::Ptr abilities, entt::id_type skillId)
{
	abilities->removeAllWidgets();

	auto abilitiesComponent = _sessionEntity.try_get<AbilitiesComponent>();
	if (!abilitiesComponent) return;

	TextureAtlas& textures = getContext().registry.ctx().get<TextureAtlas>();
	for (auto&& abilityId : abilitiesComponent->abilities)
	{
		const auto& ability = AbilityRegistry::get(abilityId);
		if (ability.getAssociatedSkill() != skillId) continue;

		auto templateCopy = tgui::Panel::copy(_templateAbilityIcon);

		// Set Icon
		auto iconData = ability.getIconData();
		auto uv = textures.getUV(iconData.textureId, iconData.uvSize, iconData.uv);
		auto texture = GuiHelpers::createTGUITextureFromUV(ability.getName(), uv);
		texture.setColor(iconData.color);

		auto icon = templateCopy->get<tgui::Picture>(w_AbilityIcon);
		icon->getRenderer()->setTexture(texture);

		abilities->add(templateCopy, ability.getName());
	}
}