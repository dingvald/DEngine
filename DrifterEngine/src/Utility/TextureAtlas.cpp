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
	LOG_MSG("Creating Texture Atlas from {}...", directoryPath.string());

	std::unordered_map<int, sf::Image> imageData;
	std::unordered_map<int, entt::hashed_string> imageNames;
	std::vector<PackingRect> rects;
	int currentId = 0;
	const unsigned int maxSizeInPixels = sf::Texture::getMaximumSize();
	unsigned int runningSizeInPixels = 0;

	LOG_MSG("Maximum texture size for GPU: {}", maxSizeInPixels);

	// Collect all images from directory + subdirectories
	for (const auto& entry : std::filesystem::recursive_directory_iterator(directoryPath))
	{
		if (entry.is_directory()) continue;

		const std::string& extension = entry.path().extension().string();
		if (!SupportedImageTypes.contains(extension))
		{
			LOG_WARNING("{} could not be added to the texture atlas.", entry.path().filename().string());
			LOG_WARNING("all files in {} must be the following types:", directoryPath.string());
			for (auto&& ext : SupportedImageTypes)
			{
				LOG_WARNING("{}", ext.data());
			}
			continue;
		}

		sf::Image subImage;
		if (subImage.loadFromFile(entry.path().string()))
		{
			const std::string imageName = entry.path().filename().replace_extension().string();
			LOG_MSG("Adding {}{}", imageName, extension);
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
		sf::Image image{ packer.getActualSize() };

		for (auto&& rect : rects)
		{
			if (!rect.was_packed) continue;

			const auto& subImage = imageData.at(rect.id);
			const auto& imageName = imageNames.at(rect.id);

			if (!image.copy(subImage, { static_cast<unsigned int>(rect.x), static_cast<unsigned int>(rect.y) }))
			{
				LOG_WARNING("image could not be copied");
				continue;
			}

			sf::IntRect intRect = { {rect.x, rect.y}, {rect.w, rect.h} };
			_subTextures.emplace(imageName.value(), std::move(intRect));
			runningSizeInPixels += rect.w * rect.h;
		}

		if (!_texture.loadFromImage(image))
		{
			LOG_WARNING("image could not be loaded into the texture atlas");
		}
	}
	else
	{
		LOG_ERROR("could not pack all textures into max texture size");
		return false;
	}

	LOG_MSG("Texture size used: {}", runningSizeInPixels);
	float ratio = static_cast<float>(runningSizeInPixels) / static_cast<float>(maxSizeInPixels);
	LOG_MSG("Percentage of atlas used: {:3.1f}%", ratio * 100.f);
	if (ratio > 1.0f)
	{
		LOG_ERROR("loaded textures exceed maximum size of atlas space");
	}
	else if (ratio > 0.8)
	{
		LOG_WARNING("loaded textures exceed 80% of atlas space");
	}
	
	LOG_MSG("SUCCESS: All supported images added to atlas")
	return true;
}

const sf::Texture& TextureAtlas::getTexture() const
{
	return _texture;
}

bool TextureAtlas::hasTexture(const std::string& name) const
{
	return (_subTextures.contains(entt::hashed_string{ name.c_str() }));
}

sf::IntRect TextureAtlas::getUV(entt::id_type textureId, sf::Vector2i uvSize, sf::Vector2i localUV) const
{
	sf::IntRect rect = _subTextures.at(textureId);
	sf::Vector2i uvCoords = { rect.position.x + (localUV.x * uvSize.x), rect.position.y + (localUV.y * uvSize.y) };
	return {uvCoords, uvSize};
}

sf::Sprite TextureAtlas::getSprite(entt::id_type textureId, sf::Vector2i uvSize, sf::Vector2i localUV) const
{
	auto globalUV = getUV(textureId, uvSize, localUV);
	return sf::Sprite(_texture, globalUV);
}

