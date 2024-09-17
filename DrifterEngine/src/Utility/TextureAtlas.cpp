#include "pch.h"
#include "TextureAtlas.h"
#include <Utility/RectPacker.h>
#include <Utility/StandardLogger.h>


const std::unordered_set<std::string_view> SupportedImageTypes =
{
	".png",
	".jpg",
	".bmp"
};

bool TextureAtlas::load(const std::filesystem::path& directoryPath)
{
	std::cout << "Creating Texture Atlas from " << directoryPath << "..." << std::endl;

	std::unordered_map<int, sf::Image> imageData;
	std::unordered_map<int, entt::hashed_string> imageNames;
	std::vector<PackingRect> rects;
	int currentId = 0;
	const int maxSizeInPixels = static_cast<int>(sf::Texture::getMaximumSize());

	// Collect all images from directory + subdirectories
	for (const auto& entry : std::filesystem::recursive_directory_iterator(directoryPath))
	{
		if (entry.is_directory()) continue;

		const std::string& extension = entry.path().extension().string();
		if (!SupportedImageTypes.contains(extension))
		{
			warning_logger << "WARNING: " << entry.path().filename() << " could not be added to the texture atlas." << std::endl;
			warning_logger << "All files in " << directoryPath << " must be the following types:" << std::endl;
			for (auto&& ext : SupportedImageTypes)
			{
				warning_logger << ext << std::endl;
			}
			continue;
		}

		sf::Image subImage;
		if (subImage.loadFromFile(entry.path().string()))
		{
			const std::string& imageName = entry.path().filename().replace_extension().string();
			std::cout << "Adding " << imageName << std::endl;
			PackingRect rect = {};
			rect.id = currentId;
			rect.w = subImage.getSize().x;
			rect.h = subImage.getSize().y;
			rects.emplace_back(std::move(rect));

			entt::hashed_string hashedName{ imageName.c_str()};
			imageNames.emplace(currentId, std::move(hashedName));
			imageData.emplace(currentId++, std::move(subImage));
		}
	}
	
	// Pack image rects
	RectPacker packer = { maxSizeInPixels, maxSizeInPixels };
	if (packer.pack(rects))
	{
		sf::Image image;
		image.create(packer.getActualSize().x, packer.getActualSize().y);
		for (auto&& rect : rects)
		{
			if (!rect.was_packed) continue;
			const auto& subImage = imageData.at(rect.id);
			const auto& imageName = imageNames.at(rect.id);

			image.copy(subImage, rect.x, rect.y);

			sf::IntRect intRect = { rect.x, rect.y, rect.w, rect.h };
			_subTextures.emplace(imageName.value(), std::move(intRect));
		}
		_texture.loadFromImage(image);
	}
	else
	{
		warning_logger << "Warning: Could not pack all textures into max texture size." << std::endl;
		return false;
	}

	std::cout << "SUCCESS: All supported images added to atlas." << std::endl;
	return true;
}

const sf::Texture& TextureAtlas::getTexture() const
{
	return _texture;
}

sf::IntRect TextureAtlas::getUV(entt::id_type textureId, sf::Vector2i uvSize, sf::Vector2i localUV) const
{
	sf::IntRect rect = _subTextures.at(textureId);
	sf::Vector2i uvCoords = { rect.left + (localUV.x * uvSize.x), rect.top + (localUV.y * uvSize.y) };
	return {uvCoords, uvSize};
}

sf::Sprite TextureAtlas::getSprite(entt::id_type textureId, sf::Vector2i uvSize, sf::Vector2i localUV) const
{
	sf::IntRect globalUV = getUV(textureId, uvSize, localUV);
	return sf::Sprite(_texture, globalUV);
}

