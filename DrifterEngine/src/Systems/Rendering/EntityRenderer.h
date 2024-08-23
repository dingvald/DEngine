#pragma once
#include "Systems/System.h"
#include "Utility/ResourceHolder.h"
#include "Utility/SpriteBatch.h"

class TextureAtlas;

namespace drft::system
{
    struct CameraInfo;

    class EntityRenderer : public System
    {
    public:
        using System::System;

        virtual void init() override;
        virtual void render(sf::RenderTarget& target) override;

    private:
        void batchLitEntities(const CameraInfo& camera);
        void batchHadSeenEntities(const CameraInfo& camera);
        void batchEffectEntities(const CameraInfo& camera);

    private:
        const TextureAtlas* _textureAtlas = nullptr;
        std::unordered_map<int, SpriteBatch> _spriteLayers;
    };

} // namespace system


