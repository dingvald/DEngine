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
	sf::Vertex tl, tr, bl, br;

	auto rect = util::SpriteIndexer::get(static_cast<util::Sprite>(spriteNum), _spriteSheet);
	int tu = spriteNum % (_spriteSheet.getSize().x / rect.width);
	int tv = spriteNum / (_spriteSheet.getSize().x / rect.width);

	tl.position = position;
	tr.position = position + sf::Vector2f(16.0f, 0.0f);
	bl.position = position + sf::Vector2f(0.0f, 16.0f);
	br.position = position + sf::Vector2f(16.0f, 16.0f);

	tl.color = color;
	tr.color = color;
	bl.color = color;
	br.color = color;

	tl.texCoords = sf::Vector2f(tu * rect.width, tv * rect.height);
	tr.texCoords = sf::Vector2f((tu + 1) * rect.width, tv * rect.height);
	bl.texCoords = sf::Vector2f(tu * rect.width, (tv + 1) * rect.height);
	br.texCoords = sf::Vector2f((tu + 1) * rect.width, (tv + 1) * rect.height);

	_vertices.append(tl);
	_vertices.append(tr);
	_vertices.append(br);
	_vertices.append(bl);
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
