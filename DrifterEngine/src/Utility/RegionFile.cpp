#include "pch.h"
#include "RegionFile.h"
#include <Spatial/ChunkPosition.h>
#include <Utility/StandardLogger.h>

drft::RegionFile::RegionFile(const std::filesystem::path& filepath)
	: _filepath(filepath)
{}

bool drft::RegionFile::open()
{
	if (!std::filesystem::exists(_filepath))
	{
		std::filesystem::create_directory(_filepath.parent_path());

		_filestream.open(_filepath, std::ios::binary | std::ios::trunc | std::ios::in | std::ios::out);
		_filestream.seekp(0);
		_filestream.write(_lookupTable.data(), LOOKUP_TABLE_BYTE_SIZE);
		_isLookupTableInitialized = true;
		
	}
	else
	{
		_filestream.open(_filepath, std::ios::binary | std::ios::ate | std::ios::in | std::ios::out);
		if (!_isLookupTableInitialized)
		{
			_filestream.seekg(0);
			_filestream.read(_lookupTable.data(), LOOKUP_TABLE_BYTE_SIZE);
			_isLookupTableInitialized = true;
		}	
	}

	if (!_filestream || !_filestream.is_open())
	{
		error_logger << "Error in RegionFile: " << _filepath << " could not be opened." << std::endl;
		return false;
	}
	return true;
}

void drft::RegionFile::close()
{
	_filestream.close();
}

void drft::RegionFile::writeChunk(ChunkPosition position, const CompressedChunk& compressed)
{
	if (!_filestream || !_filestream.is_open())
	{
		throw std::exception("Trying to write to unopened region file");
	}

	const size_t	totalSize	= compressed.size() + sizeof(uint64_t);
	const size_t	index		= calculateLookupIndex(position);
	uint16_t		offset		= getOffset(index);
	uint8_t			size		= getSize(index);

	if (offset == 0 && size == 0)
	{
		auto [newOffset, newSize] = createNewFileEntry(index, totalSize);
		offset = newOffset;
		size = newSize;
	}

	if (totalSize > static_cast<size_t>(size * SECTOR_SIZE))
	{
		// I just want to see how often this case happens... for now
		throw std::exception("Implement file re-write here");
	}

	const int write_position = LOOKUP_TABLE_BYTE_SIZE + (offset * SECTOR_SIZE);
	_filestream.seekp(write_position);
	uint64_t byte_size = compressed.size();
	_filestream.write(reinterpret_cast<const char*>(&byte_size), sizeof(uint64_t));
	_filestream.write(compressed.data(), compressed.size());

	// Add buffer space so chunk data isn't packed
	std::vector<char> buffer;
	buffer.resize(size * SECTOR_SIZE - totalSize, 0);
	_filestream.write(buffer.data(), buffer.size());
}

std::vector<char> drft::RegionFile::readChunk(ChunkPosition position)
{
	CompressedChunk result;
	if (!_filestream || !_filestream.is_open())
	{
		throw std::exception("Trying to read unopened region file");
	}

	uint64_t		byte_size		= 0u;
	const size_t	index			= calculateLookupIndex(position);
	uint16_t		offset			= getOffset(index);
	uint8_t			size			= getSize(index);

	if (offset == 0 && size == 0)
	{
		throw std::exception("Trying to load chunk from file, but chunk does not exist.");
	}

	const int read_position = LOOKUP_TABLE_BYTE_SIZE + (offset * SECTOR_SIZE);
	_filestream.seekg(read_position);
	_filestream.read(reinterpret_cast<char*>(&byte_size), sizeof(uint64_t));
	result.resize(byte_size);
	_filestream.read(result.data(), byte_size);

	return result;
}

uint16_t drft::RegionFile::getOffset(size_t index) const
{
	return (_lookupTable.at(index) << 8) | static_cast<uint8_t>(_lookupTable.at(index + 1));
}

uint8_t drft::RegionFile::getSize(size_t index) const
{
	return _lookupTable.at(index + 2);
}

void drft::RegionFile::setOffset(size_t index, uint16_t offset)
{
	std::streampos startPos = _filestream.tellp();

	char msb = offset >> 8;
	char lsb = offset & 0xFF;

	_filestream.seekp(index);
	_filestream.put(msb);
	_filestream.put(lsb);

	_lookupTable.at(index) = msb;
	_lookupTable.at(index + 1) = lsb;

	_filestream.seekp(startPos);
}

void drft::RegionFile::setSize(size_t index, uint8_t size)
{
	std::streampos startPos = _filestream.tellp();

	_filestream.seekp(index + 2);
	_filestream.put(static_cast<char>(size));

	_lookupTable.at(index + 2) = static_cast<char>(size);

	_filestream.seekp(startPos);
}

drft::RegionFile::NewFileEntry drft::RegionFile::createNewFileEntry(size_t index, size_t compressedSize)
{
	std::streampos startPos = _filestream.tellg();

	_filestream.seekg(0, std::ios::end);
	std::streampos end_pos = _filestream.tellg();

	uint16_t offset = static_cast<uint16_t>((static_cast<int>(end_pos) - LOOKUP_TABLE_BYTE_SIZE) / SECTOR_SIZE);
	uint8_t size	= static_cast<uint8_t>((compressedSize + sizeof(size_t)) / SECTOR_SIZE) + 1;

	setOffset(index, offset);
	setSize(index, size);

	_filestream.seekg(startPos);

	return { offset, size };
}

size_t drft::RegionFile::calculateLookupIndex(ChunkPosition position) const
{
	return LOOKUP_TABLE_ENTRY_BYTE_SIZE * ((position.x % REGION_DIMENSIONS) + (position.y % REGION_DIMENSIONS) * REGION_DIMENSIONS);
}
