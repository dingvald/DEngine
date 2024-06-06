#pragma once
class TextureAtlas
{
public:
	bool load(const std::filesystem::path& directoryPath);
	const sf::Texture& getTexture() const;
	sf::IntRect getUV(entt::id_type textureId, sf::Vector2i uvSize, sf::Vector2i localUV) const;
	sf::Sprite getSprite(entt::id_type textureId, sf::Vector2i uvSize, sf::Vector2i localUV) const;

private:
	sf::Texture _texture;
	entt::dense_map<entt::id_type, sf::IntRect> _subTextures;
};

