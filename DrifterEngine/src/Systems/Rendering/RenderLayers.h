#pragma once

namespace drft::system
{
    class RenderLayer
    {
    public:
        enum Layer : unsigned int
        {
            Tiles,
            Items,
            Props,
            Actors,
            EffectsBack,
            EffectsFront,
            Camera,
            Total
        };
    };
}