#include "pch.h"
#include "CppUnitTest.h"
#include <vector>
#include <Utility/CompressData.h>
#include <Utility/ChunkSerializer.h>
#include <EnTT/entt.h>

#include <Components/ComponentMetaBinder.h>
#include <Components/ActorComponent.h>
#include <Components/DescriptionComponent.h>
#include <Components/AttackerComponent.h>
#include <Utility/RegionFile.h>


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

const std::filesystem::path TEST_PATH = std::filesystem::current_path() / "test_files";

namespace UnitTests
{
	TEST_CLASS(UnitTests)
	{
	private:
		struct TestRegistry
		{
			entt::registry registry;
			entt::entity entity1;
			entt::entity entity2;
		};

		TestRegistry createDummyRegistry()
		{
			entt::registry result;
			auto entity1 = result.create();
			ActorComponent actor1 = { .ap = 100, .moveSpeed = 420.f, .actSpeed = 69.f };
			result.emplace<ActorComponent>(entity1, actor1);
			DescriptionComponent description1 = { .name = "Test Boii" };
			result.emplace<DescriptionComponent>(entity1, description1);
			auto entity2 = result.create();
			ActorComponent actor2 = { .ap = 25, .moveSpeed = 26, .actSpeed = 27 };
			result.emplace<ActorComponent>(entity2, actor2);
			AttackerComponent attack = { .baseDamage = 7 };
			result.emplace<AttackerComponent>(entity2, attack);
			return {std::move(result), entity1, entity2};
		}

	public:
		TEST_CLASS_INITIALIZE(Init)
		{
			std::filesystem::remove_all(TEST_PATH);
			std::filesystem::create_directory(TEST_PATH);
		}

		TEST_METHOD(UncompressedData_CompressThenUncompress_NoDataLoss)
		{
			std::string string_data = {
				"This is my data. My data is my friend - my only friend. Without me my data is useless; without my data I am useless."
			};

			std::vector<char> vector_data{ string_data.begin(), string_data.end() };
			const size_t expected_size = vector_data.size();
			auto compressed = drft::util::compressData(vector_data);
			auto uncompressed = drft::util::decompressData(compressed);
			const size_t actual_size = uncompressed.size();
			std::string rebuit_string{ uncompressed.begin(), uncompressed.end() };

			Assert::AreEqual(expected_size, actual_size);
			Assert::AreEqual(string_data, rebuit_string);
		}

		TEST_METHOD(LargeUncompressedData_CompressThenUncompress_NoDataLoss)
		{
			const size_t data_size = 32768;
			std::vector<char> vector_data;
			vector_data.reserve(data_size);
			char current = 0;
			for (int i = 0; i < data_size; i++)
			{
				vector_data.push_back(current++);
			}

			auto compressed = drft::util::compressData(vector_data);
			auto uncompressed = drft::util::decompressData(compressed);
			const size_t actual_size = uncompressed.size();

			Assert::AreEqual(data_size, actual_size);
			for (int i = 0; i < data_size; i++)
			{
				Assert::AreEqual(vector_data[i], uncompressed[i]);
			}
		}

		TEST_METHOD(Registry_SerializeAndCompress_NoDataLossOnDecompress)
		{
			ComponentMetaBinder::bindComponents();
			auto setup = createDummyRegistry();

			auto compressed = drft::ChunkSerializer::serializeAndCompressRegistry(setup.registry);
			entt::registry registry2;
			drft::ChunkSerializer::decompressAndDeserializeRegistry(compressed, registry2);

			Assert::AreEqual(setup.registry.size(), registry2.size());
			ActorComponent* actor = registry2.try_get<ActorComponent>(setup.entity1);
			Assert::IsNotNull(actor);
			Assert::AreEqual(100, actor->ap);
			Assert::AreEqual(420.f, actor->moveSpeed);
			Assert::AreEqual(69.f, actor->actSpeed);
			DescriptionComponent* desc = registry2.try_get<DescriptionComponent>(setup.entity1);
			Assert::IsNotNull(desc);
			Assert::AreEqual(std::string{ "Test Boii" }, desc->name);
		}

		TEST_METHOD(Registry_WriteChunkToFile_ReadChunkAsExpected)
		{
			ComponentMetaBinder::bindComponents();
			auto setup = createDummyRegistry();
			
			const std::filesystem::path test_path = TEST_PATH / "test_chunk.dat";
			drft::RegionFile regionFile{ test_path };
			const drft::ChunkPosition chunk = { 0,0,0 };

			auto start_compressed = drft::ChunkSerializer::serializeAndCompressRegistry(setup.registry);
			Assert::IsTrue(regionFile.open());
			regionFile.writeChunk(chunk, start_compressed);
			regionFile.close();
			Assert::IsTrue(regionFile.open());
			auto end_compressed = regionFile.readChunk(chunk);
			regionFile.close();

			Assert::AreEqual(start_compressed.size(), end_compressed.size());
			for (int i = 0; i < start_compressed.size(); i++)
			{
				Assert::AreEqual(start_compressed[i], end_compressed[i]);
			}
		}

		TEST_METHOD(Registry_WriteManyChunksToFile_ReadChunkAsExpected)
		{
			ComponentMetaBinder::bindComponents();
			auto setup = createDummyRegistry();
			const std::filesystem::path test_path = TEST_PATH / "test_chunk2.dat";
			drft::RegionFile regionFile{ test_path };
			std::vector<drft::ChunkPosition> chunks;
			for (int x = 0; x < REGION_DIMENSIONS; x++)
			{
				for (int y = 0; y < REGION_DIMENSIONS; y++)
				{
					chunks.push_back(drft::ChunkPosition{ x, y, 0 });
				}
			}

			auto start_compressed = drft::ChunkSerializer::serializeAndCompressRegistry(setup.registry);
			Assert::IsTrue(regionFile.open());
			for (auto&& chunk : chunks)
			{
				regionFile.writeChunk(chunk, start_compressed);
			}
			regionFile.close();

			Assert::IsTrue(regionFile.open());
			for (auto&& chunk : chunks)
			{
				auto end_compressed = regionFile.readChunk(chunk);
				Assert::AreEqual(start_compressed.size(), end_compressed.size());
				for (int i = 0; i < start_compressed.size(); i++)
				{
					Assert::AreEqual(start_compressed[i], end_compressed[i]);
				}
			}
			regionFile.close();
		}
	};
}
