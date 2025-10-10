#pragma once
#include <memory>
#include <optional>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <JSON/ICreateFromJson.h>

struct SelectorContext
{
	unsigned int seed;
};

class IPositionSelector : public ICreateFromJson
{
public:
	using Ptr = std::unique_ptr<IPositionSelector>;
	virtual void createFromJson(const rapidjson::Value& json) = 0;
	virtual std::optional<sf::Vector2i> selectPosition(const sf::IntRect& area, const SelectorContext& context) const = 0;
};