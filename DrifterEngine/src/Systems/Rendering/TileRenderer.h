#pragma once
#include "Systems/System.h"
#include "Utility/SpriteBatch.h"

namespace drft::system
{
	class TileRenderer : public System
	{
    public:
        virtual void init() override;
        virtual void render(sf::RenderTarget& target) override;

    private:
        SpriteBatch _spriteBatch;
        sf::Vector2f _prevPosition = { 0,0 };
	};
}


