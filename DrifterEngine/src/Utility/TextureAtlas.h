#pragma once
class TextureAtlas
{
public:
	bool createAtlas(const std::filesystem::path& directoryPath);
	const sf::Texture& getTexture() const;
	const sf::IntRect getSubTexture(const std::string& name) const;

private:
	bool addSubImage(const sf::Image& image, const sf::Image& subImage, const std::string& subImageName, unsigned int maxSizeInPixels);

private:
	sf::Texture _texture;
	entt::dense_map<std::string, sf::IntRect> _subTextures;
};

