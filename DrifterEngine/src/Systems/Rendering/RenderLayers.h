#pragma once

namespace drft::system
{
    enum class RenderLayer : unsigned int
    {
        Tiles,
        Items,
        Props,
        Actors,
        Effects,
        Camera,
        Total
    };
}