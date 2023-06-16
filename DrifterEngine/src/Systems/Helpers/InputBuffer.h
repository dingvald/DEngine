#pragma once

namespace drft::system
{
	class InputBuffer
	{
	public:
		InputBuffer(unsigned int maxBufferSize);
		sf::Keyboard::Key popKey();
		void pushKey(sf::Keyboard::Key key);
		bool isEmpty() const;

	private:
		unsigned int _maxBufferSize = 1;
		std::queue<sf::Keyboard::Key> _buffer;
	};
}


