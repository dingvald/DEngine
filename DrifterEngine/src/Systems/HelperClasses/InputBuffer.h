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

		ModifiedKey pop();

		bool isEmpty() const;

	private:
		using KeyBuffer = std::deque<ModifiedKey>;
		struct KeyState
		{
			float timeHeld = 0;
			bool active = false;
		};
		std::unordered_map<ModifiedKey, KeyState> _pressedKeys;
		unsigned int _maxBufferSize = 1;
		KeyBuffer _buffer;
	};
}


