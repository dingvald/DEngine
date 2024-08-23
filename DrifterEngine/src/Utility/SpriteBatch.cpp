#include "pch.h"
#include "SpriteBatch.h"

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

void drft::SpriteBatch::addSprite(sf::IntRect uv, sf::Color color, sf::Vector2f position)
{
	sf::Vertex topLeft, topRight, bottomLeft, bottomRight;

	topLeft.position = position;
	topRight.position = position + sf::Vector2f(uv.width, 0.0f);
	bottomLeft.position = position + sf::Vector2f(0.0f, uv.height);
	bottomRight.position = position + sf::Vector2f(uv.width, uv.height);

	topLeft.color = color;
	topRight.color = color;
	bottomLeft.color = color;
	bottomRight.color = color;

	topLeft.texCoords = sf::Vector2f(uv.left, uv.top);
	topRight.texCoords = sf::Vector2f(uv.left + uv.width, uv.top);
	bottomLeft.texCoords = sf::Vector2f(uv.left, uv.top + uv.height);
	bottomRight.texCoords = sf::Vector2f(uv.left + uv.width, uv.top + uv.height);

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
