#pragma once
class TextureAtlas
{
public:
	bool createAtlas(const std::filesystem::path& directoryPath);
	const sf::Texture& getTexture() const;
	sf::IntRect getSubTexture(const entt::hashed_string& name) const;

private:
	sf::Texture _texture;
	entt::dense_map<unsigned int, sf::IntRect> _subTextures;
};

