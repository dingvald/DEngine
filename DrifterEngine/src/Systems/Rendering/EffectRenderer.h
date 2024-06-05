#pragma once
#include "Systems/System.h"
#include "Utility/SpriteBatch.h"

class TextureAtlas;

namespace drft::system
{
	class EffectRenderer : public System
	{
    private:
        virtual void init() override;
        virtual void render(sf::RenderTarget& target) override;

    private:
        const TextureAtlas* _textureAtlas = nullptr;
        std::unordered_map<int, SpriteBatch> _spriteLayers;
	};
}


