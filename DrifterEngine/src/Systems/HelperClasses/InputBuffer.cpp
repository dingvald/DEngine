#include "pch.h"
#include "InputBuffer.h"

#include <Engine/EngineConstants.h>

static constexpr unsigned int INPUT_BUFFER_MAX_SIZE = 2;
static constexpr float REFRACTORY_PERIOD = 0.05f; // sec
static constexpr float HOLD_TIME = 0.25f; // sec

drft::system::InputBuffer::InputBuffer(unsigned int maxBufferSize)
    : _maxBufferSize(maxBufferSize)
{
}

void drft::system::InputBuffer::update()
{
    for (auto&& [key, state] : _activeInputs)
    {
        updateInputState(key, state);
    }
}

void drft::system::InputBuffer::press(ModifiedInput key)
{
    if (key.value.index() == 0) return;
    if (_inputBuffer.size() >= INPUT_BUFFER_MAX_SIZE) return;

    _inputBuffer.push_back(key);
    _activeInputs.emplace(key, InputState{});
}

void drft::system::InputBuffer::release(ModifiedInput key)
{
    if (key.value.index() == 0) return;

    _activeInputs.erase(key);
    std::erase(_inputBuffer, key);
}

ModifiedInput drft::system::InputBuffer::pop()
{
    auto itr = _inputBuffer.begin();
    while (itr != _inputBuffer.end())
    {
        if (!_activeInputs.contains(*itr))
        {
            itr = _inputBuffer.erase(itr);
        }
        else
        {
            if (_activeInputs.at(*itr).active)
            {
                ModifiedInput result = *itr;
                return result;
            }
            ++itr;
        }
    }

    return {};
}

bool drft::system::InputBuffer::isEmpty() const
{
    return _inputBuffer.empty();
}

void drft::system::InputBuffer::updateInputState(const ModifiedInput& input, InputState& state)
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
    state.timeHeld = std::min(state.timeHeld + SECONDS_PER_FRAME, HOLD_TIME);
}
