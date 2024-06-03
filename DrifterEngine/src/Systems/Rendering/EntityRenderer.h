#pragma once
#include "Systems/System.h"
#include "Utility/ResourceHolder.h"
#include "Utility/SpriteBatch.h"

namespace drft::system
{
    class EntityRenderer : public System
    {
    private:
        virtual void init() override;
        virtual void render(sf::RenderTarget& target) override;

    private:
        sf::Texture _sprites;
        std::unordered_map<int, SpriteBatch> _spriteLayers;
    };

} // namespace system


