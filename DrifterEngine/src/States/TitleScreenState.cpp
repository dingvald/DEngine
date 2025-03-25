#include "pch.h"
#include "TitleScreenState.h"

#include <Engine/EngineConstants.h>

#include <Utility/Math.h>

static const char* PressAnyButtonLabel = "PressAnyButton";

static const int CycleTimeInFrames = TARGET_UPDATES_PER_SECOND * 4; // Four seconds

drft::TitleScreenState::TitleScreenState(StateStack& stack, StateContext& context)
    : State(stack, context)
{
	auto label = tgui::Label::create("Press Any Button");
	label->setTextSize(32);
	label->setPosition("50%", "70%");
	label->setOrigin(0.5f, 0.5f);
	label->setHorizontalAlignment(tgui::HorizontalAlignment::Center);
	
	_guiGroup->add(label, PressAnyButtonLabel);
}

bool drft::TitleScreenState::handleEvent(const sf::Event& ev)
{
	if (ev.is<sf::Event::KeyPressed>()
		|| ev.is<sf::Event::MouseButtonPressed>())
	{
		requestStackPop();
		requestStackPush(States::MainMenu);
		return true;
	}

	return false;
}

bool drft::TitleScreenState::update()
{
	float alpha = 0;
	static int elapsed = 0;
	const float twoPI = 2.0f * 3.14152f;
	const float omega = twoPI / CycleTimeInFrames;

	alpha = (std::sinf(omega * elapsed) + 1.f) / 2.f;

	auto text = _guiGroup->get(PressAnyButtonLabel);
	text->getRenderer()->setOpacity(alpha);

	elapsed++;
	if (elapsed >= CycleTimeInFrames)
	{
		elapsed = 0;
	}
	
    return false;
}
