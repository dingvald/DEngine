#pragma once
#include "Systems/System.h"

namespace drft::system
{
    enum class Relationship;

    class CollisionSystem : public System
    {
    public:
        using System::System;

    private:
        void init() override;
        void updateEnd(const float dt) override;

        void onCollisionAdded(entt::registry& registry, entt::entity entity) const;
        Relationship determineTargetRelationship(entt::const_handle sourceEntity, const std::vector<entt::entity>& entities) const;
    };
}


