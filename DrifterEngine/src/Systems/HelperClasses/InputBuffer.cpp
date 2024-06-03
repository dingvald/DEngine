#include "pch.h"
#include "InputBuffer.h"

drft::system::InputBuffer::InputBuffer(unsigned int maxBufferSize)
    : _maxBufferSize(maxBufferSize)
{
}

sf::Keyboard::Key drft::system::InputBuffer::popKey()
{
    auto key = _buffer.front();
    _buffer.pop();

    return key;
}

void drft::system::InputBuffer::pushKey(sf::Keyboard::Key key)
{
    if (_buffer.size() >= _maxBufferSize) return;

    _buffer.push(key);
}

bool drft::system::InputBuffer::isEmpty() const
{
    return _buffer.empty();
}
