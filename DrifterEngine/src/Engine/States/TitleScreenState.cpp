#include "pch.h"
#include "TitleScreenState.h"

drft::TitleScreenState::TitleScreenState(StateStack& stack, StateContext& context, tgui::Group::Ptr gui)
    : State(stack, context, gui)
{
	float centerX = getContext().window.getSize().x / 2.f;
	float centerY = getContext().window.getSize().y / 2.f;
	float threeQuarterY = (centerY + getContext().window.getSize().y) / 2.f;
	sf::Vector2f textPosition = { centerX, threeQuarterY };

	_pressAnyButtonText.setFont(getContext().fonts.get("Terminus"));
	_pressAnyButtonText.setFillColor(sf::Color::White);
	_pressAnyButtonText.setString("Press any button...");
	float textWidth = static_cast<float>(_pressAnyButtonText.getCharacterSize() * _pressAnyButtonText.getString().getSize());
	_pressAnyButtonText.setOrigin({ textWidth / 4.f, 0.f });
	_pressAnyButtonText.setPosition(textPosition);
}

bool drft::TitleScreenState::handleEvent(const sf::Event& ev)
{
	switch (ev.type)
	{
	case sf::Event::KeyPressed:
		requestStackPop();
		requestStackPush(States::MainMenu);
		break;
	}

	return true;
}

bool drft::TitleScreenState::update(const float dt)
{
	float alpha = 255;
	const float twoPI = 2.0f * 3.14152f;
	const float f = 0.5f;
	static float elapsed = 0.0f;
	elapsed += dt;
	if (elapsed > 10.0)
	{
		elapsed = 0.0;
	}
	alpha = 255 * ((std::sinf(twoPI * f * elapsed) + 1.f) / 2.f);

	_pressAnyButtonText.setFillColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(alpha)));
	
    return false;
}

void drft::TitleScreenState::render(sf::RenderTarget& target)
{
	target.draw(_pressAnyButtonText);
}
