#pragma once

#include "Archive.h"
#include "Reflection.h"

namespace snapshot {

    class Snapshot
    {
    public:
        static void save(OutputArchive archive, const entt::registry& registry);
    };

    class SnapshotLoader
    {
    public:
        static void load(InputArchive archive, entt::registry& registry);
    };

} // namespace snapshot