#pragma once
class Biome;

struct BiomeZone
{
	BiomeZone(const Biome* type)
		:_type(type)
	{}
	const Biome* getType() const { return _type; }
	void setID(unsigned int id) { _id = id; }
	unsigned int getID() const { return _id; }
	int size() const { return _zone.size(); }
	void put(sf::Vector2i position) { _zone.insert(position); }
	const std::unordered_set<sf::Vector2i>& getZone() const { return _zone; }
private:
	const Biome* _type;
	unsigned int _id = 0;
	std::unordered_set<sf::Vector2i> _zone;
};

