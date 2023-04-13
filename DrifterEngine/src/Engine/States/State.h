#pragma once
#include "Utility/ResourceHolder.h"
#include "StateIdentifiers.h"

namespace drft
{
	class StateStack;

	class State
	{
	public:
		using Ptr = std::unique_ptr<State>;

		struct Context
		{
			using TextureHolder = ResourceHolder<sf::Texture, std::string>;
			using FontHolder = ResourceHolder<sf::Font, std::string>;

			Context(sf::RenderWindow& window, TextureHolder& textures, FontHolder& fonts)
				: window(&window)
				, textures(&textures)
				, fonts(&fonts)
			{}

			sf::RenderWindow* window;
			TextureHolder* textures;
			FontHolder* fonts;
			
		};

	public:
		State(StateStack& stack, Context context);
		virtual ~State();

		virtual bool handleEvent(const sf::Event& ev);
		virtual bool update(const float dt) = 0;
		virtual void render(sf::RenderTarget& target) = 0;

	protected:
		void requestStackPush(States stateId);
		void requestStackPop();
		void requestStackClear();

		Context getContext() const;

	private:
		StateStack* _stack;
		Context _context;
	};
}



