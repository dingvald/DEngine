#pragma once
#include <Keybindings/ModifiedKey.h>

namespace drft::system
{
	class InputBuffer
	{
	public:
		InputBuffer(unsigned int maxBufferSize);

		void update(const float dt);
		void press(ModifiedInput input);
		void release(ModifiedInput input);

		ModifiedInput pop();

		bool isEmpty() const;

	private:
		struct InputState
		{
			float timeHeld = 0;
			bool active = false;
		};
		void updateInputState(const ModifiedInput& input, InputState& state, const float dt);

	private:
		using ModifiedInputBuffer = std::deque<ModifiedInput>;
		
		std::unordered_map<ModifiedInput, InputState> _activeInputs;
		unsigned int _maxBufferSize = 1;
		ModifiedInputBuffer _inputBuffer;
	};
}


