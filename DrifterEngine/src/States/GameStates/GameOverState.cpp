#include "pch.h"
#include "GameOverState.h"
#include <Engine/EngineConstants.h>

static const char* w_BackgroundPanel = "background";
static const char* w_CenterPanel = "center";
static const char* w_Text = "text";

drft::GameOverState::GameOverState(StateStack& stack, StateContext& context)
	: State(stack, context)
{
	auto background = tgui::Panel::create();
	background->setSize("100%", "100%");
	background->getRenderer()->setBackgroundColor(tgui::Color{ 255, 0, 0, 5 });

	auto center = tgui::Panel::create();
	center->setSize("100%", "10%");
	center->getRenderer()->setBackgroundColor(tgui::Color{ 0,0,0,0 });
	center->setOrigin(0.5f, 0.5f);
	center->setPosition("50%", "50%");

	auto label = tgui::Label::create();
	label->setTextSize(32);
	label->setText("You Died");
	label->setPosition("50%", "50%");
	label->setOrigin(0.5f, 0.5f);
	label->getRenderer()->setTextColor(tgui::Color::White);
	label->setScale(0.1);

	_guiGroup->add(background, w_BackgroundPanel);
	_guiGroup->add(center, w_CenterPanel);
	_guiGroup->add(label, w_Text);
}

bool drft::GameOverState::handleEvent(const sf::Event& ev)
{
	if (const auto keypressed = ev.getIf<sf::Event::KeyPressed>())
	{
		if (keypressed->code == sf::Keyboard::Key::Escape)
		{
			requestStackClear();
			return true;
		}
	}
	return false;
}

bool drft::GameOverState::fixedUpdate()
{
	updateBackgroundPanel();
	updateCenterPanel();
	updateText();

	_elapsed = std::min(_elapsed += SECONDS_PER_FRAME, 10.f);

	return true;
}

void drft::GameOverState::updateBackgroundPanel() const
{
	auto background = _guiGroup->get<tgui::Panel>(w_BackgroundPanel);
	const uint8_t alpha = std::min(static_cast<uint8_t>(_elapsed * 15), static_cast<uint8_t>(30));
	background->getRenderer()->setBackgroundColor(tgui::Color{ 255, 0, 0, alpha });
}

void drft::GameOverState::updateCenterPanel() const
{
	auto center = _guiGroup->get<tgui::Panel>(w_CenterPanel);
	const uint8_t alpha = std::min(static_cast<uint8_t>(_elapsed * 15), static_cast<uint8_t>(80));
	center->getRenderer()->setBackgroundColor(tgui::Color{ 0, 0, 0, alpha });
}

void drft::GameOverState::updateText() const
{
	auto text = _guiGroup->get<tgui::Label>(w_Text);
	const float scale = std::min(_elapsed * 0.5f, 2.5f);
	text->setScale(scale);
}
