#include "pch.h"
#include "InputBuffer.h"

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
}

void drft::system::InputBuffer::press(ModifiedKey key)
{
    if (key.key == sf::Keyboard::Scan::Unknown) return;
    if (_buffer.size() >= INPUT_BUFFER_MAX_SIZE) return;

    _buffer.push_back(key);
    _pressedKeys.emplace(key, KeyState{});
}

void drft::system::InputBuffer::release(ModifiedKey key)
{
    if (key.key == sf::Keyboard::Scan::Unknown) return;

    _pressedKeys.erase(key);
}

ModifiedKey drft::system::InputBuffer::pop()
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
                ModifiedKey result = *itr;
                _buffer.erase(itr);
                return result;
            }
            ++itr;
        }
    }

    return {};
}

bool drft::system::InputBuffer::isEmpty() const
{
    return _buffer.empty();
}
