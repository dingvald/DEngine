#pragma once
class XORshift
{
public:
    typedef std::uint32_t result_type;

    struct state_type {
        result_type x;
        result_type y;
        result_type z;
        result_type w;
    };

    XORshift(void);
    explicit XORshift(result_type r);
    explicit XORshift(const state_type& seed);

    void seed(void);
    void seed(result_type r);
    void seed(const state_type& state);

    result_type operator()(void);

    void discard(unsigned long long z);
    // Discard the next z random values.

    const state_type& state(void) const;
    void state(const state_type& state);

    static result_type min(void);
    static result_type max(void);

private:
    state_type _state;
};

