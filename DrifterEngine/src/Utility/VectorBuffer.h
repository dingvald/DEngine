#pragma once
#include <streambuf>
#include <string>
#include <vector>

template <typename CharT = char, typename Traits = std::char_traits<CharT>>
class VectorBuffer : public std::basic_streambuf<CharT>
{
public:
    using streambuf = std::basic_streambuf<CharT, Traits>;
    using char_type = typename streambuf::char_type;
    using int_type = typename streambuf::int_type;
    using traits_type = typename streambuf::traits_type;
    using vector = std::vector<char_type>;
    using value_type = typename vector::value_type;
    using size_type = typename vector::size_type;

    VectorBuffer(size_type capacity = 0)
        : _vector()
    {
        reserve(capacity);
    }

    operator const std::vector<CharT>&() { return _vector; }

    constexpr void reserve(size_type capacity) { _vector.reserve(capacity); setp_from_vector(); }
    constexpr void reserve_additional(size_type additional_capacity) { reserve(size() + additional_capacity); }
    constexpr const value_type* data() const { return _vector.data(); }
    constexpr size_type size() const { return _vector.size(); }
    constexpr size_type capacity() const { return _vector.capacity(); }
    constexpr void clear() { _vector.clear(); }

    std::streamsize xsputn(const char_type* s, std::streamsize count) override {
        try {
            reserve_additional(count);
        }
        catch (const std::bad_alloc&) {
            return xsputn_slow(s, count);
        }
        return xsputn_fast(s, count);
    }

protected:
    constexpr value_type* pbase_from_vector() const { return const_cast<value_type*>(_vector.data()); }
    constexpr value_type* pptr_from_vector() const { return const_cast<value_type*>(_vector.data() + _vector.size()); }
    constexpr value_type* epptr_from_vector() const { return const_cast<value_type*>(_vector.data()) + _vector.capacity(); }
    constexpr void setp_from_vector() { streambuf::setp(pbase_from_vector(), epptr_from_vector()); streambuf::pbump(size()); }\

private:

    std::streamsize xsputn_fast(const char_type* s, std::streamsize count) {
        auto* old_pptr = pptr_from_vector();
        _vector.resize(_vector.size() + count);
        traits_type::copy(old_pptr, s, count);
        streambuf::pbump(count);

        return count;
    }
    std::streamsize xsputn_slow(const char_type* s, std::streamsize count) {
        std::streamsize written = 0;
        while (written < count) {
            try {
                _vector.emplace_back(s[written]);
            }
            catch (const std::bad_alloc&) {
                int_type c = this->overflow(traits_type::to_int_type(s[written]));
                if (traits_type::eq_int_type(c, traits_type::eof())) {
                    return written;
                }
            }
            setp_from_vector();
            written++;
        }
        return written;
    }

private:
    vector _vector;
};