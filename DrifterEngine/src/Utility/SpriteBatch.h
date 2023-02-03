#pragma once

namespace drft
{

	class SpriteBatch : public sf::Drawable, public sf::Transformable
	{
	public:
		SpriteBatch();
		SpriteBatch(sf::Texture& texture);
		SpriteBatch& operator=(const SpriteBatch&) = default;
		SpriteBatch(SpriteBatch&&) = default;
		SpriteBatch& operator=(SpriteBatch&&) = default;

		void setTexture(const sf::Texture& texture);
		void addSprite(unsigned int spriteNum, sf::Color color, sf::Vector2f position);
		void clear();

	private:
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	private:
		sf::VertexArray _vertices;
		sf::Texture _spriteSheet;
	};

} // namespace drft


