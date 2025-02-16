#pragma once

class HashedString
{
public:
	constexpr HashedString(const char* str)
		: _friendlyString(str)
		, _hashedString(entt::hashed_string(str))
	{}

	constexpr entt::id_type getHash() const
	{
		return _hashedString;
	}
	constexpr std::string_view getFriendly() const
	{
		return _friendlyString;
	}

	constexpr bool operator==(const HashedString& other)
	{
		return _hashedString == other._hashedString;
	}

	constexpr operator std::string_view() const
	{
		return _friendlyString;
	}
	constexpr operator entt::id_type() const
	{
		return _hashedString;
	}

private:
	std::string_view _friendlyString;
	entt::id_type _hashedString;
};

