#pragma once
#include "System.h"
#include "Utility/ResourceHolder.h"
#include "Utility/SpriteBatch.h"

namespace drft::system
{
    class EntityRenderer : public System
    {
    public:
        EntityRenderer()
        {
            phase = Phase::OnRender;
        }

        virtual void init() override;
        virtual void render(sf::RenderTarget& target) override;

    private:
        sf::Texture _sprites;
        std::map<int, SpriteBatch> _spriteLayers;
    };

} // namespace system


