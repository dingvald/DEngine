#pragma once
class TextureAtlas
{
public:
	bool load(const std::filesystem::path& directoryPath);
	const sf::Texture& getTexture() const;
	sf::Vector2f getUVCoords(entt::id_type textureId, sf::Vector2f localUV) const;
	sf::Sprite getSprite(entt::id_type textureId, sf::Vector2f uvSize, sf::Vector2f localUV) const;

private:
	sf::Texture _texture;
	entt::dense_map<entt::id_type, sf::IntRect> _subTextures;
};

