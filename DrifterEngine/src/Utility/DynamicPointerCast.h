#pragma once
#include <memory>

template <typename To, typename From>
std::unique_ptr<To> dynamic_unique_cast(std::unique_ptr<From>&& p) {
    if (To* cast = dynamic_cast<To*>(p.get()))
    {
        std::unique_ptr<To> result(cast);
        p.release();
        return result;
    }
    return std::unique_ptr<To>(nullptr); // or throw std::bad_cast() if you prefer
}