#include "pch.h"
#include "SpriteBatch.h"

#pragma optimize("", off)

drft::SpriteBatch::SpriteBatch()
	: _texture(nullptr)
{
	_vertices.setPrimitiveType(sf::PrimitiveType::Triangles);
}

drft::SpriteBatch::SpriteBatch(sf::Texture& texture)
	: _texture(&texture)
{
	_vertices.setPrimitiveType(sf::PrimitiveType::Triangles);
}

void drft::SpriteBatch::setTexture(const sf::Texture& texture)
{
	_texture = &texture;
}

void drft::SpriteBatch::addSprite(sf::IntRect uv, sf::Color color, sf::Vector2f position)
{
	sf::Vertex topLeft, topRight, bottomLeft, bottomRight;

	topLeft.position = position;
	topRight.position = position + sf::Vector2f(uv.size.x, 0.0f);
	bottomLeft.position = position + sf::Vector2f(0.0f, uv.size.y);
	bottomRight.position = position + sf::Vector2f(uv.size);

	topLeft.color = color;
	topRight.color = color;
	bottomLeft.color = color;
	bottomRight.color = color;

	topLeft.texCoords = sf::Vector2f(uv.position);
	topRight.texCoords = sf::Vector2f(uv.position.x + uv.size.x, uv.position.y);
	bottomLeft.texCoords = sf::Vector2f(uv.position.x, uv.position.y + uv.size.y);
	bottomRight.texCoords = sf::Vector2f(uv.position + uv.size);

	// Triangle 1
	_vertices.append(topLeft);
	_vertices.append(topRight);
	_vertices.append(bottomRight);

	// Triangle 2
	_vertices.append(topLeft);
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
