#include "pch.h"
#include "CompressData.h"
#include <zlib.h>
#include <Utility/StandardLogger.h>

const unsigned int BUFFER_CHUNK_SIZE = 1024u;

std::vector<char> drft::util::compressData(const std::vector<char>& inData)
{
    std::vector<char> result;

    z_stream zs = {};
    zs.zalloc = Z_NULL;
    zs.zfree = Z_NULL;
    zs.opaque = Z_NULL;

    int init_result = deflateInit(&zs, Z_DEFAULT_COMPRESSION);
    if (init_result != Z_OK)
    {
        throw std::runtime_error("Exception during zlib compression");
    }

    zs.next_in = (Bytef*)inData.data();
    zs.avail_in = inData.size();


    std::array<char, BUFFER_CHUNK_SIZE> buffer = {};
    int deflate_result;
    do {
        zs.next_out = reinterpret_cast<Bytef*>(&buffer);
        zs.avail_out = BUFFER_CHUNK_SIZE;
        deflate_result = deflate(&zs, Z_FINISH);
        size_t diff = zs.total_out - result.size();
        result.insert(result.end(), buffer.begin(), buffer.begin() + diff);

    } while (deflate_result == Z_OK);
    
    deflateEnd(&zs);

    if (deflate_result != Z_STREAM_END)
    {
        throw std::runtime_error("Exception during zlib compression");
    }

    return result;
}

std::vector<char> drft::util::decompressData(const std::vector<char>& inData)
{
    std::vector<char> result;

    z_stream zs = {};
    zs.zalloc = Z_NULL;
    zs.zfree = Z_NULL;
    zs.opaque = Z_NULL;
    zs.next_in = Z_NULL;
    zs.avail_in = 0;

    int init_result = inflateInit(&zs);
    if (init_result != Z_OK)
    {
        throw std::runtime_error("Exception during zlib compression: inflateInit failed");
    }

    zs.next_in = (Bytef*)inData.data();
    zs.avail_in = inData.size();

    std::array<char, BUFFER_CHUNK_SIZE> buffer = {};
    int inflate_result;
    do {
        zs.next_out = reinterpret_cast<Bytef*>(&buffer);
        zs.avail_out = BUFFER_CHUNK_SIZE;
        inflate_result = inflate(&zs, Z_NO_FLUSH);
        size_t diff = zs.total_out - result.size();
        result.insert(result.end(), buffer.begin(), buffer.begin() + diff);

    } while (inflate_result == Z_OK);
    
    inflateEnd(&zs);

    if (inflate_result != Z_STREAM_END)
    {
        throw std::runtime_error("Exception during zlib compression: Inflate result unexpected");
    }

    return result;
}
