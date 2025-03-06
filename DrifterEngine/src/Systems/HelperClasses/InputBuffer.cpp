#include "pch.h"
#include "InputBuffer.h"

#include <Engine/EngineConstants.h>

static constexpr unsigned int INPUT_BUFFER_MAX_SIZE = 2;
static constexpr float REFRACTORY_PERIOD = 0.08f; // sec
static constexpr float HOLD_TIME = 0.25f; // sec

drft::system::InputBuffer::InputBuffer(unsigned int maxBufferSize)
    : _maxBufferSize(maxBufferSize)
{
}

void drft::system::InputBuffer::update()
{
    for (auto&& [button, state] : _pressedMouse)
    {
        updateKeyState(state);
    }
    for (auto&& [key, state] : _pressedKeys)
    {
        updateKeyState(state);
    }
}

void drft::system::InputBuffer::press(ModifiedKey key)
{
    if (key.key == sf::Keyboard::Scan::Unknown) return;
    if (_keyBuffer.size() >= INPUT_BUFFER_MAX_SIZE) return;

    _keyBuffer.push_back(key);
    _pressedKeys.emplace(key, KeyState{});
}

void drft::system::InputBuffer::release(ModifiedKey key)
{
    if (key.key == sf::Keyboard::Scan::Unknown) return;

    _pressedKeys.erase(key);
    std::erase(_keyBuffer, key);
}

void drft::system::InputBuffer::mousePress(sf::Mouse::Button button)
{
    if (_mouseBuffer.size() >= INPUT_BUFFER_MAX_SIZE) return;

    _mouseBuffer.push_back(button);
    _pressedMouse.emplace(button, KeyState{});
}

void drft::system::InputBuffer::mouseRelease(sf::Mouse::Button button)
{
    _pressedMouse.erase(button);
    std::erase(_mouseBuffer, button);
}

std::optional<sf::Mouse::Button> drft::system::InputBuffer::popMouse()
{
    auto itr = _mouseBuffer.begin();
    while (itr != _mouseBuffer.end())
    {
        if (!_pressedMouse.contains(*itr))
        {
            itr = _mouseBuffer.erase(itr);
        }
        else
        {
            if (_pressedMouse.at(*itr).active)
            {
                sf::Mouse::Button result = *itr;
                return result;
            }
            ++itr;
        }
    }

    return {};
}

ModifiedKey drft::system::InputBuffer::popKey()
{
    auto itr = _keyBuffer.begin();
    while (itr != _keyBuffer.end())
    {
        if (!_pressedKeys.contains(*itr))
        {
            itr = _keyBuffer.erase(itr);
        }
        else
        {
            if (_pressedKeys.at(*itr).active)
            {
                ModifiedKey result = *itr;
                return result;
            }
            ++itr;
        }
    }

    return {};
}

bool drft::system::InputBuffer::isEmpty() const
{
    return _keyBuffer.empty();
}

void drft::system::InputBuffer::updateKeyState(KeyState& state)
{
    state.active = false;
    if (std::abs(state.timeHeld) <= std::numeric_limits<float>::epsilon())
    {
        // just pressed
        state.active = true;
    }
    else if (state.timeHeld >= HOLD_TIME)
    {
        // Held key long enough
        state.active = true;
        state.timeHeld -= REFRACTORY_PERIOD;
    }
    state.timeHeld = std::min(state.timeHeld + SECONDS_PER_FRAME, HOLD_TIME + SECONDS_PER_FRAME);
}
