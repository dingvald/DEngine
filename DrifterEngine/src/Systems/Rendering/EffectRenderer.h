#pragma once
#include "Systems/System.h"
#include "Utility/SpriteBatch.h"

namespace drft::system
{
	class EffectRenderer : public System
	{
    private:
        virtual void init() override;
        virtual void render(sf::RenderTarget& target) override;

    private:
        sf::Texture _sprites;
        std::unordered_map<int, SpriteBatch> _spriteLayers;
	};
}


