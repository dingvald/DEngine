#pragma once
#include "Systems/System.h"
#include "Utility/ResourceHolder.h"
#include "Utility/SpriteBatch.h"

class TextureAtlas;

namespace drft::system
{
    class EntityRenderer : public System
    {
    public:
        using System::System;

        virtual void init() override;
        virtual void render(sf::RenderTarget& target) override;

    private:
        const TextureAtlas* _textureAtlas = nullptr;
        std::unordered_map<int, SpriteBatch> _spriteLayers;
    };

} // namespace system


