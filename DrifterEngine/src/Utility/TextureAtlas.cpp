#include "pch.h"
#include "TextureAtlas.h"


const std::unordered_set<std::string_view> SupportedImageTypes =
{
	".png",
	".jpg",
	".bmp"
};

bool TextureAtlas::createAtlas(const std::filesystem::path& directoryPath)
{
	std::cout << "Creating Texture Atlas from " << directoryPath << "..." << std::endl;
	sf::Image image;
	const unsigned int maxSizeInPixels = sf::Texture::getMaximumSize();
	for (const auto& filename : std::filesystem::directory_iterator(directoryPath))
	{
		sf::Image subImage;
		const auto extension = filename.path().extension();
		if (!SupportedImageTypes.contains(extension.string()))
		{
			std::cout << filename << " not a supported image type. Skipping." << std::endl;
			continue;
		}

		std::string imageName = filename.path().filename().string();
		if (subImage.loadFromFile(filename.path().string()))
		{
			if (!addSubImage(image, subImage, imageName, maxSizeInPixels))
			{

			}
		}
	}


	return false;
}

const sf::Texture& TextureAtlas::getTexture() const
{
	// TODO: insert return statement here
}

const sf::IntRect TextureAtlas::getSubTexture(const std::string& name) const
{
	return sf::IntRect();
}

bool TextureAtlas::addSubImage(const sf::Image& image, const sf::Image& subImage, const std::string& subImageName, unsigned int maxSizeInPixels)
{
	return false;
}

