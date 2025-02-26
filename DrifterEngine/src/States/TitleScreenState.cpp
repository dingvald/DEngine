#include "pch.h"
#include "TitleScreenState.h"

static const char* PressAnyButtonLabel = "PressAnyButton";

drft::TitleScreenState::TitleScreenState(StateStack& stack, StateContext& context)
    : State(stack, context)
{
	auto label = tgui::Label::create("Press Any Button");
	label->setTextSize(32);
	label->setPosition("50%, 80%");
	
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
	float alpha = 255;
	const float twoPI = 2.0f * 3.14152f;
	const float f = 0.5f;
	static float elapsed = 0.0f;
	elapsed++;
	if (elapsed > 10.0)
	{
		elapsed = 0.0;
	}
	alpha = ((std::sinf(twoPI * f * elapsed) + 1.f) / 2.f);

	auto text = _guiGroup->get(PressAnyButtonLabel);
	text->getRenderer()->setOpacity(alpha);
	
    return false;
}
