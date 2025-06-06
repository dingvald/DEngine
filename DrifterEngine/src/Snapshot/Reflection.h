#pragma once
#pragma once

#include "Archive.h"

namespace snapshot 
{
    using namespace entt::literals;

    constexpr auto EMPLACE_INTO_REG_FN_NAME = "emplace"_hs;
    constexpr auto TAKE_COMPONENT_SNAPSHOT_FN_NAME = "take_component_snapshot"_hs;
    constexpr auto LOAD_COMPONENT_SNAPSHOT_FN_NAME = "load_component_snapshot"_hs;

    /**
     * Collection of functions to be reflected for components.
     * */
    namespace ReflectionFunctions 
    {
        template<typename T>
        void takeSnapshot(const entt::snapshot& snapshot, OutputArchive archive)
        {
            snapshot.component<T>(archive);
        }

        template<typename T>
        void loadSnapshot(const entt::snapshot_loader& snapshot, InputArchive archive)
        {
            snapshot.component<T>(archive);
        }

        template<typename T>
        void reflectComponentFunctions(entt::meta_factory<T>& meta)
        {
            meta.func<&entt::registry::emplace_or_replace<T>, entt::as_ref_t>(EMPLACE_INTO_REG_FN_NAME);
            meta.func<&takeSnapshot<T>>(TAKE_COMPONENT_SNAPSHOT_FN_NAME);
            meta.func<&loadSnapshot<T>>(LOAD_COMPONENT_SNAPSHOT_FN_NAME);
        }

        template<typename T, std::string_view const& Str>
        void assignName(entt::meta_factory<T>& meta)
        {
            using namespace entt::literals;
            meta.data<&Str>("meta_name"_hs);
        }

        template<typename T, std::string_view const& Str>
        void reflectWithName(entt::meta_factory<T>& meta)
        {
            meta.type(entt::hashed_string{ Str.data() });
            assignName<T, Str>(meta);
        }

    } // namespace ReflectionFunctions

    /**
     * Reflects serialization, emplace, removal, contains, get, get-type for passed
     * component type.
     * */
    template<typename T, std::string_view const& Str>
    entt::meta_factory<T> reflectComponent(entt::meta_ctx& ctx)
    {
        using namespace ReflectionFunctions;
        entt::meta_factory<T> meta = entt::meta<T>(ctx);

        reflectWithName<T, Str>(meta);
        reflectComponentFunctions<T>(meta);

        return meta;
    }

} // namespace snapshot