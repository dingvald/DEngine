#pragma once

namespace drft::system
{
	class InputBuffer
	{
	public:
		InputBuffer(unsigned int maxBufferSize);

		void update(const float dt);
		void press(sf::Keyboard::Key key);
		void release(sf::Keyboard::Key key);

		sf::Keyboard::Key pop();

		bool isEmpty() const;

	private:
		using KeyBuffer = std::deque<sf::Keyboard::Key>;
		struct KeyState
		{
			float timeHeld = 0;
			bool active = false;
		};
		std::unordered_map<sf::Keyboard::Key, KeyState> _pressedKeys;
		unsigned int _maxBufferSize = 1;
		KeyBuffer _buffer;
	};
}


