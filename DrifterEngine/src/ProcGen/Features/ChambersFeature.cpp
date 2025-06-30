#include "pch.h"
#include "ChambersFeature.h"
#include <Spatial/Grid2d.h>
#include <Spatial/Helpers.h>
#include <Random/Random.h>
#include <ProcGen/GenerationContext.h>

using namespace entt::literals;

void ChambersFeature::createFromJson(const rapidjson::Value& json)
{
    if (json.HasMember("wall_tags"))
    {
        for (auto&& tag : json["wall_tags"].GetArray())
        {
            _wallTags.emplace_back(entt::hashed_string{ tag.GetString() });
        }
    }
    if (json.HasMember("center_tags"))
    {
        for (auto&& tag : json["center_tags"].GetArray())
        {
            _centerTags.emplace_back(entt::hashed_string{ tag.GetString() });
        }
    }
    if (json.HasMember("floor_tags"))
    {
        for (auto&& tag : json["floor_tags"].GetArray())
        {
            _floorTags.emplace_back(entt::hashed_string{ tag.GetString() });
        }
    }
    if (json.HasMember("room_count"))
    {
        _roomCount.setMin(json["room_count"].GetArray()[0].GetInt());
        _roomCount.setMax(json["room_count"].GetArray()[1].GetInt());
    }
    if (json.HasMember("room_size"))
    {
        _roomSize.setMin(json["room_size"].GetArray()[0].GetInt());
        _roomSize.setMax(json["room_size"].GetArray()[1].GetInt());
    }
}

TaggedPositions ChambersFeature::generateTags(sf::Vector3i position, const GenerationContext& context) const
{
    TaggedPositions result;
    using Grid = drft::spatial::Grid2d<uint8_t>;
    drft::rng::Random random{ context.seed };

    // Define grid size based on max room size and count
    int gridSize = _roomSize.getMax() * 2 * _roomCount.getMax();
    Grid grid(gridSize, gridSize);
    grid.fill(0);

    struct Room { int x, y, w, h; };
    std::vector<Room> rooms;

    // BSP partitioning
    struct Node { int x, y, w, h, depth; };
    std::vector<Node> stack;
    int minRoom = _roomSize.getMin();
    int maxRoom = _roomSize.getMax();
    int minRooms = _roomCount.getMin();
    int maxRooms = _roomCount.getMax();
    stack.push_back({0, 0, gridSize, gridSize, 0});
    while (!stack.empty() && (int)rooms.size() < maxRooms) {
        Node node = stack.back(); stack.pop_back();
        // If node is too big, split
        bool split = (node.w > maxRoom || node.h > maxRoom) && (int)rooms.size() + (int)stack.size() < maxRooms;
        if (split) {
            bool horizontal = (node.w < node.h) ? true : (node.w > node.h) ? false : random.chance(0.5f);
            if (horizontal) {
                int splitY = random.intInRange(node.y + minRoom, node.y + node.h - minRoom);
                stack.push_back({node.x, node.y, node.w, splitY - node.y, node.depth + 1});
                stack.push_back({node.x, splitY, node.w, node.y + node.h - splitY, node.depth + 1});
            } else {
                int splitX = random.intInRange(node.x + minRoom, node.x + node.w - minRoom);
                stack.push_back({node.x, node.y, splitX - node.x, node.h, node.depth + 1});
                stack.push_back({splitX, node.y, node.x + node.w - splitX, node.h, node.depth + 1});
            }
        } else if (node.w >= minRoom && node.h >= minRoom && (int)rooms.size() < maxRooms) {
            // Place a room in this node
            int rw = random.intInRange(minRoom, std::min(node.w, maxRoom));
            int rh = random.intInRange(minRoom, std::min(node.h, maxRoom));
            int rx = node.x + (node.w - rw) / 2;
            int ry = node.y + (node.h - rh) / 2;
            rooms.push_back({rx, ry, rw, rh});
        }
    }
    // Draw rooms
    for (const auto& room : rooms) {
        for (int x = room.x; x < room.x + room.w; ++x) {
            for (int y = room.y; y < room.y + room.h; ++y) {
                grid.at(x, y) = 1; // floor
            }
        }
    }
    // Draw walls
    for (const auto& room : rooms) {
        for (int x = room.x - 1; x <= room.x + room.w; ++x) {
            for (int y = room.y - 1; y <= room.y + room.h; ++y) {
                if (x < 0 || y < 0 || x >= grid.width() || y >= grid.height()) continue;
                if (x >= room.x && x < room.x + room.w && y >= room.y && y < room.y + room.h) continue;
                grid.at(x, y) = 2; // wall
            }
        }
    }
    // Tagging
    int half = gridSize / 2;
    for (auto&& tag : _centerTags) {
        result[tag].push_back(position);
    }
    for (int x = 0; x < grid.width(); ++x) {
        for (int y = 0; y < grid.height(); ++y) {
            sf::Vector3i pos = position + sf::Vector3i{x - half, y - half, 0};
            if (grid.at(x, y) == 1 && !_floorTags.empty()) {
                for (auto&& tag : _floorTags) result[tag].push_back(pos);
            }
            if (grid.at(x, y) == 2 && !_wallTags.empty()) {
                for (auto&& tag : _wallTags) result[tag].push_back(pos);
            }
        }
    }
    return result;
}
