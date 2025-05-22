#include "pch.h"
#include "SkillsScreenState.h"

#include <Defines/CommonGuiColors.h>
#include <Components/Actions/OpenSkillsScreenAction.h>
#include <Utility/TGUIHelpers.h>
#include <Utility/StandardLogger.h>
#include <Utility/StringManipulation.h>

#pragma optimize("", off)

static const char* w_SkillName = "skill_name";
static const char* w_SkillLevel = "skill_level";
static const char* w_ProgressBar = "progress_bar";
static const char* w_Abilities = "abilities";


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

	auto craftingTitle = tgui::Label::create();
	craftingTitle->setSize({ tgui::bindWidth(panel), 32 });
	craftingTitle->setOrigin(0.5f, 1.f);
	craftingTitle->setPosition(tgui::bindPosX(panel), tgui::bindTop(panel) - 8);
	craftingTitle->setText("Skills and Abilities");
	craftingTitle->setTextSize(20);
	craftingTitle->setVerticalAlignment(tgui::VerticalAlignment::Bottom);
	craftingTitle->setHorizontalAlignment(tgui::HorizontalAlignment::Center);
	craftingTitle->getRenderer()->setBackgroundColor(guiColor::BlackAgate);
	_guiGroup->add(craftingTitle);

	auto split = tgui::SplitContainer::create();
	split->setOrigin(0.5f, 0.5f);
	split->setPosition("50%, 50%");
	split->setSize(tgui::bindSize(panel));
	split->setOrientation(tgui::Orientation::Horizontal);
	split->setSplitterWidth(1.f);
	split->setSplitterOffset("35%");
	split->setMinValidSplitterOffset("35%");
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
	abilities->setOrigin(0.5f, 0.5f);
	abilities->getRenderer()->setPadding({ 8 });
	abilities->getRenderer()->setSpaceBetweenWidgets(16);
	split->add(abilities, w_Abilities);
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
	skillName->setTextSize(16);
	skillName->setPosition(8.f, "50%");

	auto progressBar = tgui::ProgressBar::create();
	templatePanel->add(progressBar, w_ProgressBar);
	progressBar->setOrigin(0.f, 0.5f);
	progressBar->setPosition(128.f, "50%");
	progressBar->setMinimum(0);
	progressBar->setMaximum(100);
	progressBar->setSize(256, 10);
	progressBar->getRenderer()->setBorderColor(guiColor::TranslucentAsh);
	progressBar->getRenderer()->setFillColor(guiColor::MGSHudBlue);
	progressBar->getRenderer()->setBackgroundColor(guiColor::TranslucentAsh);

	auto skillLevel = tgui::Label::create();
	templatePanel->add(skillLevel, w_SkillLevel);
	skillLevel->setVerticalAlignment(tgui::VerticalAlignment::Center);
	skillLevel->setOrigin(0.f, 0.5f);
	skillLevel->setTextSize(24);
	skillLevel->setPosition(tgui::bindRight(progressBar) + 16, "50%");
}

void drft::SkillsScreenState::refreshSkillsList(tgui::PanelListBox::Ptr skillsList)
{
	for (auto&& skill : _skills.getSkills())
	{
		auto panel = skillsList->addItem(skill.name());

		auto skillName = panel->get<tgui::Label>(w_SkillName);
		skillName->setText(util::capitalizeAll(util::removeUnderscores(skill.name())));
		skillName->setIgnoreMouseEvents(true);

		auto skillLevel = panel->get<tgui::Label>(w_SkillLevel);
		skillLevel->setText(std::to_string(skill.level()));
		skillLevel->setIgnoreMouseEvents(true);

		auto progressBar = panel->get<tgui::ProgressBar>(w_ProgressBar);
		int progressValue = skill.getProgress() * 100;
		progressBar->setValue(progressValue);
		progressBar->setIgnoreMouseEvents(true);

		auto button = GuiHelpers::buttonizePanel(panel);
		button->getRenderer()->setBackgroundColor(tgui::Color::Transparent);
		button->onPress([this, skillId = skill.id()]() {refreshAbilities(_guiGroup->get<tgui::HorizontalWrap>(w_Abilities), skillId);});
	}
}

void drft::SkillsScreenState::refreshAbilities(tgui::HorizontalWrap::Ptr abilities, entt::id_type skillId)
{
	LOG_MSG("Refresh for skill with id {}", skillId);
}
