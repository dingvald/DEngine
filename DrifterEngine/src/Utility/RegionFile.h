#pragma once

const int SECTOR_SIZE = 8184;
const int REGION_DIMENSIONS = 32;
const int LOOKUP_TABLE_NUM_ENTRIES = REGION_DIMENSIONS * REGION_DIMENSIONS;
const int LOOKUP_TABLE_ENTRY_BYTE_SIZE = 3; // 2 Bytes: offset in sectors - 1 Byte: size in sectors
const int LOOKUP_TABLE_BYTE_SIZE = LOOKUP_TABLE_NUM_ENTRIES * LOOKUP_TABLE_ENTRY_BYTE_SIZE;

namespace drft
{

struct ChunkPosition;
using CompressedChunk = std::vector<char>;


class RegionFile
{
public:
	RegionFile(const std::filesystem::path& filepath);

	bool open();
	void close();

	void writeChunk(ChunkPosition position, const CompressedChunk& compressed);
	CompressedChunk readChunk(ChunkPosition position);

private:
	uint16_t getOffset(size_t index) const;
	uint8_t getSize(size_t index) const;
	void setOffset(size_t index, uint16_t offset);
	void setSize(size_t index, uint8_t size);

	struct NewFileEntry
	{
		uint16_t offset;
		uint8_t size;
	};
	NewFileEntry createNewFileEntry(size_t index, size_t compressedSize);
	size_t calculateLookupIndex(ChunkPosition position) const;

private:
	using LookupTable = std::array<char, LOOKUP_TABLE_BYTE_SIZE>;
	LookupTable _lookupTable = { 0 };
	bool _isLookupTableInitialized = false;
	std::filesystem::path _filepath;
	std::fstream _filestream;
};

}