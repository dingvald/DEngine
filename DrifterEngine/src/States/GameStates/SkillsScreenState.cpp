#include "pch.h"
#include "SkillsScreenState.h"

#include <Defines/CommonGuiColors.h>
#include <Components/Actions/OpenSkillsScreenAction.h>

#pragma optimize("", off)

static const char* w_SkillName = "skill_name";
static const char* w_SkillLevel = "skill_level";
static const char* w_ProgressBar = "progress_bar";


drft::SkillsScreenState::SkillsScreenState(StateStack& stack, StateContext& context)
	: State(stack, context)
{
	determineSessionEntity();

	auto list = tgui::PanelListBox::create();
	list->setOrigin(0.5f, 0.5f);
	list->setPosition("50%, 50%");
	list->setSize(tgui::bindSize(_guiGroup) * 0.5f);
	list->setItemsHeight(48.f);
	list->getRenderer()->setBackgroundColor(guiColor::BlackAgate);
	_guiGroup->add(list, "skills_screen");

	auto craftingTitle = tgui::Label::create();
	craftingTitle->setSize({ tgui::bindWidth(list), 32 });
	craftingTitle->setOrigin(0.5f, 1.f);
	craftingTitle->setPosition(tgui::bindPosX(list), tgui::bindTop(list) - 8);
	craftingTitle->setText("Skills");
	craftingTitle->setTextSize(20);
	craftingTitle->setVerticalAlignment(tgui::VerticalAlignment::Bottom);
	craftingTitle->setHorizontalAlignment(tgui::HorizontalAlignment::Center);
	craftingTitle->getRenderer()->setBackgroundColor(guiColor::BlackAgate);
	_guiGroup->add(craftingTitle);

	auto templatePanel = list->getPanelTemplate();
	setupPanelTemplate(templatePanel);

	refreshSkillsList(list);
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
	auto skillName = tgui::Label::create();
	templatePanel->add(skillName, w_SkillName);
	skillName->setVerticalAlignment(tgui::VerticalAlignment::Center);
	skillName->setOrigin(0.f, 0.5f);
	skillName->setTextSize(16);
	skillName->setPosition("1%", "50%");

	auto progressBar = tgui::ProgressBar::create();
	templatePanel->add(progressBar, w_ProgressBar);
	progressBar->setOrigin(0.f, 0.5f);
	progressBar->setPosition("10%", "50%");
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
	skillLevel->setTextSize(16);
	skillLevel->setPosition(tgui::bindRight(progressBar) + 8, "50%");
}

void drft::SkillsScreenState::refreshSkillsList(tgui::PanelListBox::Ptr skillsList)
{
	for (auto&& skill : _skills.getSkills())
	{
		auto panel = skillsList->addItem(skill->name());

		auto skillName = panel->get<tgui::Label>(w_SkillName);
		skillName->setText(skill->name());

		auto skillLevel = panel->get<tgui::Label>(w_SkillLevel);
		skillLevel->setText(std::to_string(skill->level()));

		auto progressBar = panel->get<tgui::ProgressBar>(w_ProgressBar);
		int progressValue = skill->getProgress() * 100;
		progressBar->setValue(progressValue);
	}
}
