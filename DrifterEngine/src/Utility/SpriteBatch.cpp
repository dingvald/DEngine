#include "pch.h"
#include "SpriteBatch.h"
#include "SpriteIndexer.h"

drft::SpriteBatch::SpriteBatch()
{
	_vertices.setPrimitiveType(sf::Quads);
}

drft::SpriteBatch::SpriteBatch(sf::Texture& texture)
{
	_spriteSheet = texture;
	_vertices.setPrimitiveType(sf::Quads);
}

void drft::SpriteBatch::setTexture(const sf::Texture& texture)
{
	_spriteSheet = texture;
}

void drft::SpriteBatch::addSprite(unsigned int spriteNum, sf::Color color, sf::Vector2f position)
{
	sf::Vertex topLeft, topRight, bottomLeft, bottomRight;

	auto rect = util::SpriteIndexer::get(static_cast<util::Sprite>(spriteNum), _spriteSheet);
	int tu = spriteNum % (_spriteSheet.getSize().x / rect.width);
	int tv = spriteNum / (_spriteSheet.getSize().x / rect.width);

	topLeft.position		= position;
	topRight.position		= position + sf::Vector2f(16.0f, 0.0f);
	bottomLeft.position		= position + sf::Vector2f(0.0f, 16.0f);
	bottomRight.position	= position + sf::Vector2f(16.0f, 16.0f);

	topLeft.color			= color;
	topRight.color			= color;
	bottomLeft.color		= color;
	bottomRight.color		= color;

	topLeft.texCoords		= sf::Vector2f(tu * rect.width, tv * rect.height);
	topRight.texCoords		= sf::Vector2f((tu + 1) * rect.width, tv * rect.height);
	bottomLeft.texCoords	= sf::Vector2f(tu * rect.width, (tv + 1) * rect.height);
	bottomRight.texCoords	= sf::Vector2f((tu + 1) * rect.width, (tv + 1) * rect.height);

	_vertices.append(topLeft);
	_vertices.append(topRight);
	_vertices.append(bottomRight);
	_vertices.append(bottomLeft);
}

void drft::SpriteBatch::clear()
{
	_vertices.clear();
}

void drft::SpriteBatch::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform();
	states.texture = &_spriteSheet;
	target.draw(_vertices, states);
}
