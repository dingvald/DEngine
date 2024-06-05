#include "pch.h"
#include "SpriteBatch.h"
#include "SpriteIndexer.h"

drft::SpriteBatch::SpriteBatch()
	: _texture(nullptr)
{
	_vertices.setPrimitiveType(sf::Quads);
}

drft::SpriteBatch::SpriteBatch(sf::Texture& texture)
	: _texture(&texture)
{
	_vertices.setPrimitiveType(sf::Quads);
}

void drft::SpriteBatch::setTexture(const sf::Texture& texture)
{
	_texture = &texture;
}

void drft::SpriteBatch::addSprite(unsigned int spriteNum, sf::Color color, sf::Vector2f position)
{
	sf::Vertex topLeft, topRight, bottomLeft, bottomRight;

	auto rect = util::SpriteIndexer::get(static_cast<util::Sprite>(spriteNum), *_texture);
	int tu = spriteNum % (_texture->getSize().x / rect.width);
	int tv = spriteNum / (_texture->getSize().x / rect.width);

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

void drft::SpriteBatch::addSprite(sf::Vector2f uvSize, sf::Vector2f uv, sf::Color color, sf::Vector2f position)
{
	sf::Vertex topLeft, topRight, bottomLeft, bottomRight;

	topLeft.position = position;
	topRight.position = position + sf::Vector2f(uvSize.x, 0.0f);
	bottomLeft.position = position + sf::Vector2f(0.0f, uvSize.y);
	bottomRight.position = position + uvSize;

	topLeft.color = color;
	topRight.color = color;
	bottomLeft.color = color;
	bottomRight.color = color;

	topLeft.texCoords = sf::Vector2f(uv.x * uvSize.x, uv.y * uvSize.y);
	topRight.texCoords = sf::Vector2f((uv.x + 1) * uvSize.x, uv.y * uvSize.y);
	bottomLeft.texCoords = sf::Vector2f(uv.x * uvSize.x, (uv.y + 1) * uvSize.y);
	bottomRight.texCoords = sf::Vector2f((uv.x + 1) * uvSize.x, (uv.y + 1) * uvSize.y);

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
	states.texture = _texture;
	target.draw(_vertices, states);
}
