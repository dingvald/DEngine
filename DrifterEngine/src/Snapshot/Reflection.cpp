#include <pch.h>
#include "Reflection.h"

namespace snapshot {

    std::string_view Reflection::name() const
    {
        using namespace entt::literals;
        return _type.data("meta_name"_hs)
            .get(entt::meta_handle{})
            .cast<std::string_view>();
    }

    Reflection::Reflection(entt::meta_type in_type)
        : _type(in_type)
    {}
    Reflection::Reflection(entt::hashed_string name)
        : _type(entt::resolve(name))
    {}
    Reflection::Reflection(std::string const& name)
        : Reflection(entt::hashed_string{ name.c_str() })
    {}
    Reflection::Reflection(std::string_view name)
        : Reflection(entt::hashed_string{ name.data() })
    {}
    Reflection::Reflection(entt::id_type type_id)
        : _type(entt::resolve(type_id))
    {}
    Reflection::Reflection(entt::type_info type_info)
        : _type(entt::resolve(type_info))
    {}

    entt::meta_any ComponentReflection::get(entt::const_handle h) const
    {
        try {
            auto func = _reflection.type().func(GET_CONST_COMPONENT_FN_NAME);
            auto res = func.invoke({}, h);

            if (!res || !std::as_const(res).data()) {
                throw std::runtime_error("Failed to get instance of reflected component");
            }
            return res;
        }
        catch (std::runtime_error& e) {
            throw std::runtime_error("Failed to get instance of reflected component");
        }
    }

    entt::meta_any ComponentReflection::get(entt::handle h) const
    {
        try {
            auto res =
                _reflection.type().invoke(GET_COMPONENT_FN_NAME, entt::meta_handle{}, h);
            if (!res) {
                throw std::runtime_error("Failed to get instance of reflected component");
            }
            return res;
        }
        catch (std::runtime_error& e) {
            throw std::runtime_error("Failed to get instance of reflected component");
        }
    }

    bool ComponentReflection::isSerializable() const
    {
        using namespace entt::literals;
        auto prop = _reflection.type().prop("serialize"_hs);

        if (prop) return true;
        return false;
    }

    bool ComponentReflection::contains(entt::const_handle h) const
    {
        return _reflection.type()
            .invoke(CONTAINS_COMPONENT_FN_NAME, entt::meta_handle{}, h)
            .cast<bool>();
    }

    void ComponentReflection::remove(entt::handle h) const
    {
        _reflection.type().invoke(REMOVE_COMPONENT_FN_NAME, entt::meta_handle{}, h);
    }

    void ComponentReflection::emplace(entt::handle h, entt::meta_handle comp) const
    {
        if (!comp->invoke(EMPLACE_COMPONENT_FN_NAME, h, comp->data())) {
            throw std::runtime_error("Failed to emplace component_any");
        }
    }

    void ComponentReflection::emplace(entt::handle h) const
    {
        auto comp = _reflection.type().construct();
        if (!comp) {
            throw std::runtime_error(
                "Failed to construct reflected component for emplacement");
        }

        emplace(h, comp);
    }

    ComponentReflection::ComponentReflection(Reflection in_reflection)
        : _reflection(in_reflection)
    {}

    void Handle::doSave(OutputArchive archive) const
    {
        auto data = std::as_const(any)->data();
        if (data == nullptr) {
            throw std::runtime_error("Trying to save nullptr any");
        }
        auto type = any->type();
        auto func = type.func(SAVE_FN_NAME);
        auto res = func.invoke({}, data, archive);

        if (!res) {
            throw std::runtime_error("Failed to save any");
        }
    }

    Handle::Handle(entt::meta_any const& any)
        : any(any)
    {}

    void Any::doLoad(InputArchive archive)
    {
        if (!any.invoke(LOAD_FN_NAME, any.data(), archive)) {
            throw std::runtime_error("Failed to load any");
        }
    }

    Any::Any(entt::meta_any any)
        : any(any)
    {}

} 