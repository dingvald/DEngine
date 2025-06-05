#pragma once

#include "Archive.h"
#include "Reflection.h"

namespace snapshot {

    namespace detail {

        struct SerializeHandleEntity
        {
            entt::entity e;
            std::vector<Handle> components;

        private:
            friend class cereal::access;
            template<typename Archive>
            void save(Archive& archive) const
            {
                auto sz_e = static_cast<size_t>(e);

                archive(cereal::make_nvp("e", sz_e));
                archive(CEREAL_NVP(components));
            }
            template<typename Archive>
            void load(Archive& archive)
            {
                throw std::runtime_error("Don't load via SerializeHandleEntity");
            }
        };

        struct SerializeEntity
        {
            entt::entity e;
            std::vector<Any> components;

        private:
            friend class cereal::access;
            template<typename Archive>
            void save(Archive& archive) const
            {
                throw std::runtime_error("Don't save via SerializeEntity");
            }
            template<typename Archive>
            void load(Archive& archive)
            {
                auto sz_e = 0UL;
                archive(cereal::make_nvp("e", sz_e));
                e = static_cast<entt::entity>(sz_e);

                archive(CEREAL_NVP(components));
            }
        };

    } // namespace detail

    class Snapshot
    {
    public:
        static void save(OutputArchive, entt::const_handle);
        static void save(OutputArchive, std::vector<entt::entity>&, entt::registry const&);
        static void save(OutputArchive, entt::registry const&);

    private:
        static void saveHandle(OutputArchive&, entt::const_handle);
    };

    class SnapshotLoader
    {
    public:
        static void load(InputArchive, entt::handle);
        static void load(InputArchive, entt::registry&);

    private:
        static void loadHandle(InputArchive, entt::registry&);
        static void loadHandle(InputArchive, entt::handle);
    };

} // namespace snapshot