#pragma once
#include "AI/GOAP/IGoal.h"

namespace drft::goap
{
    class ExploreGoal : public IGoal
    {
    public:
        ExploreGoal();
        virtual float utility(entt::const_handle agent) const override;
    };
}


