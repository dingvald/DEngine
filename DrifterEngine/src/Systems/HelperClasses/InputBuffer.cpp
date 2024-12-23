#include "pch.h"
#include "InputBuffer.h"
#include <Services/DebugInfo.h>

static constexpr unsigned int INPUT_BUFFER_MAX_SIZE = 2;
static constexpr float REFRACTORY_PERIOD = 0.10f; // sec
static constexpr float HOLD_TIME = 0.25f; // sec

drft::system::InputBuffer::InputBuffer(unsigned int maxBufferSize)
    : _maxBufferSize(maxBufferSize)
{
}

void drft::system::InputBuffer::update()
{
    for (auto&& [key, state] : _pressedKeys)
    {
        state.active = false;
        if (std::abs(state.timeHeld) <= std::numeric_limits<float>::epsilon())
        {
            state.active = true;
        }
        else if (state.timeHeld >= HOLD_TIME)
        {
            // Held key long enough
            state.active = true;
            state.timeHeld -= REFRACTORY_PERIOD;
        }

        state.timeHeld = std::min(state.timeHeld + 1.0f, HOLD_TIME + 1.0f);
    }

    service::DebugInfo::instance().putInfo("Input Buffer", std::to_string(_buffer.size()));
}

void drft::system::InputBuffer::press(sf::Keyboard::Key key)
{
    if (_buffer.size() >= INPUT_BUFFER_MAX_SIZE) return;

    _buffer.push_back(key);
    _pressedKeys.emplace(key, KeyState{});
}

void drft::system::InputBuffer::release(sf::Keyboard::Key key)
{
    _pressedKeys.erase(key);
}

sf::Keyboard::Key drft::system::InputBuffer::pop()
{
    auto itr = _buffer.begin();
    while (itr != _buffer.end())
    {
        if (!_pressedKeys.contains(*itr))
        {
            itr = _buffer.erase(itr);
        }
        else
        {
            if (_pressedKeys.at(*itr).active)
            {
                sf::Keyboard::Key result = *itr;
                _buffer.erase(itr);
                return result;
            }
            ++itr;
        }
    }

    return sf::Keyboard::Key::Unknown;
}

bool drft::system::InputBuffer::isEmpty() const
{
    return _buffer.empty();
}
