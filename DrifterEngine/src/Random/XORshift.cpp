#include "pch.h"
#include "XORshift.h"

static const XORshift::state_type s_default_seed = {
    123456789, 362436069, 521288629, 88675123
};

XORshift::XORshift(void)
    : _state(s_default_seed)
{}

XORshift::XORshift(const state_type& seed)
    : _state(seed)
{}

XORshift::XORshift(result_type r)
    : _state({ s_default_seed.x, s_default_seed.y, s_default_seed.z, r })
{}

void XORshift::seed(const state_type& seed) {
    state(seed);
}

void XORshift::seed(void) {
    state(s_default_seed);
}

void XORshift::seed(result_type r) {
    auto seed = s_default_seed;
    seed.w = r;
    state(seed);
}

void XORshift::discard(unsigned long long z) {
    while (z--)
        (*this)();
}

const XORshift::state_type& XORshift::state(void) const {
    return _state;
}

void XORshift::state(const state_type& state) {
    _state = state;
}

XORshift::result_type XORshift::min(void) {
    return std::numeric_limits<result_type>::min();
}

XORshift::result_type XORshift::max(void) {
    return std::numeric_limits<result_type>::max();
}

XORshift::result_type XORshift::operator()(void) {
    result_type t = _state.x ^ (_state.x << 15);
    _state.x = _state.y; _state.y = _state.z; _state.z = _state.w;
    return _state.w = _state.w ^ (_state.w >> 21) ^ (t ^ (t >> 4));
}

static bool operator==(
    const XORshift::state_type& lhs, const XORshift::state_type& rhs)
{
    return lhs.x == rhs.x
        && lhs.y == rhs.y
        && lhs.z == rhs.z
        && lhs.w == rhs.w;
}

static bool operator!=(
    const XORshift::state_type& lhs, const XORshift::state_type& rhs)
{
    return lhs.x != rhs.x
        || lhs.y != rhs.y
        || lhs.z != rhs.z
        || lhs.w != rhs.w;
}

bool operator==(const XORshift& lhs, const XORshift& rhs) {
    return lhs.state() == rhs.state();
}

bool operator!=(const XORshift& lhs, const XORshift& rhs) {
    return lhs.state() != rhs.state();
}