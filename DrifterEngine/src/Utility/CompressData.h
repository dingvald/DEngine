#pragma once

namespace drft::util
{
	std::vector<char> compressData(const std::vector<char>& inData);
	std::vector<char> decompressData(const std::vector<char>& inData);
}
