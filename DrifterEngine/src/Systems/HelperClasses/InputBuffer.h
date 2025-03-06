#pragma once
#include <Keybindings/ModifiedKey.h>

namespace drft::system
{
	class InputBuffer
	{
	public:
		InputBuffer(unsigned int maxBufferSize);

		void update();
		void press(ModifiedKey key);
		void release(ModifiedKey key);

		void mousePress(sf::Mouse::Button button);
		void mouseRelease(sf::Mouse::Button button);

		std::optional<sf::Mouse::Button> popMouse();

		ModifiedKey popKey();

		bool isEmpty() const;

	private:
		struct KeyState
		{
			float timeHeld = 0;
			bool active = false;
		};
		void updateKeyState(KeyState& state);

	private:
		using MouseBuffer = std::deque<sf::Mouse::Button>;
		using KeyBuffer = std::deque<ModifiedKey>;
		
		std::unordered_map<ModifiedKey, KeyState> _pressedKeys;
		std::unordered_map<sf::Mouse::Button, KeyState> _pressedMouse;
		unsigned int _maxBufferSize = 1;
		KeyBuffer _keyBuffer;
		MouseBuffer _mouseBuffer;
	};
}


