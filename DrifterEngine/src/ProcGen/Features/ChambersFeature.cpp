#include "pch.h"
#include "ChambersFeature.h"
#include <Utility/StandardLogger.h>
#include <Spatial/Grid2d.h>
#include <Spatial/Helpers.h>
#include <Random/Random.h>
#include <ProcGen/GenerationContext.h>

enum TileType : uint8_t {
    Tile_Empty = 0,
    Tile_Floor = 1,
    Tile_Wall = 2,
    Tile_Hallway = 3
};

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
    if (json.HasMember("hallway_tags"))
    {
        for (auto&& tag : json["hallway_tags"].GetArray())
        {
            _hallwayTags.emplace_back(entt::hashed_string{ tag.GetString() });
        }
    }
    if (json.HasMember("door_tags"))
    {
        for (auto&& tag : json["door_tags"].GetArray())
        {
            _doorTags.emplace_back(entt::hashed_string{ tag.GetString() });
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
    if (json.HasMember("entrances"))
    {
        _entrances.setMin(json["entrances"].GetArray()[0].GetInt());
        _entrances.setMax(json["entrances"].GetArray()[1].GetInt());
    }
    if (json.HasMember("max_area") && json["max_area"].IsArray() && json["max_area"].Size() == 2)
    {
        _maxArea.x = json["max_area"][0].GetInt();
        _maxArea.y = json["max_area"][1].GetInt();
    }
}

TaggedPositions ChambersFeature::generateTags(sf::Vector3i position, const GenerationContext& context) const
{
    TaggedPositions result;
    using Grid = drft::spatial::Grid2d<uint8_t>;
    drft::rng::Random random{ context.seed };

    int minRoom = _roomSize.getMin();
    int maxRoom = _roomSize.getMax();
    int minRooms = _roomCount.getMin();
    int maxRooms = _roomCount.getMax();

    // Use max_area as bounding box
    int maxWidth = _maxArea.x > 0 ? _maxArea.x : maxRoom * 2 * maxRooms + 2;
    int maxHeight = _maxArea.y > 0 ? _maxArea.y : maxRoom * 2 * maxRooms + 2;

    // Clamp room size if needed
    if (maxRoom > maxWidth || maxRoom > maxHeight) {
        LOG_WARNING("ChambersFeature: room_size (%d) is greater than max_area (%d, %d). Clamping room_size.", maxRoom, maxWidth, maxHeight);
        maxRoom = std::min(maxWidth, maxHeight);
    }
    // Adjust room count to fit bounding box area
    int areaPerRoom = maxRoom * maxRoom;
    int allowedRooms = maxRooms;
    int maxArea = maxWidth * maxHeight;
    if (areaPerRoom * allowedRooms > maxArea) {
        allowedRooms = std::max(1, maxArea / areaPerRoom);
    }
    // Use maxWidth and maxHeight for grid size
    int gridSizeX = maxWidth;
    int gridSizeY = maxHeight;
    Grid grid(gridSizeX, gridSizeY);
    grid.fill(Tile_Empty);

    struct Room { int x, y, w, h; int cx() const { return x + w / 2; } int cy() const { return y + h / 2; } };

    // Generate candidate rooms, but only accept non-overlapping ones
    std::vector<Room> candidateRooms;
    struct Node { int x, y, w, h, depth; };
    std::vector<Node> stack;
    stack.push_back({0, 0, gridSizeX, gridSizeY, 0});
    while (!stack.empty() && (int)candidateRooms.size() < allowedRooms * 2) { // allow more candidates for filtering
        Node node = stack.back(); stack.pop_back();
        bool split = (node.w > maxRoom || node.h > maxRoom) && (int)candidateRooms.size() + (int)stack.size() < allowedRooms * 2;
        if (split) {
            bool horizontal = (node.w < node.h) ? true : (node.w > node.h) ? false : random.chance(0.5f);
            if (horizontal && node.h > 2 * minRoom) {
                int splitY = random.intInRange(node.y + minRoom, node.y + node.h - minRoom);
                stack.push_back({node.x, node.y, node.w, splitY - node.y, node.depth + 1});
                stack.push_back({node.x, splitY, node.w, node.y + node.h - splitY, node.depth + 1});
            } else if (!horizontal && node.w > 2 * minRoom) {
                int splitX = random.intInRange(node.x + minRoom, node.x + node.w - minRoom);
                stack.push_back({node.x, node.y, splitX - node.x, node.h, node.depth + 1});
                stack.push_back({splitX, node.y, node.x + node.w - splitX, node.h, node.depth + 1});
            } else {
                int rw = random.intInRange(minRoom, std::min(node.w, maxRoom));
                int rh = random.intInRange(minRoom, std::min(node.h, maxRoom));
                // Clamp so room + walls fit inside grid
                rw = std::min(rw, gridSizeX - 2);
                rh = std::min(rh, gridSizeY - 2);
                int rx = node.x + (node.w - rw) / 2;
                int ry = node.y + (node.h - rh) / 2;
                // Clamp so walls fit
                rx = std::max(1, std::min(rx, gridSizeX - rw - 1));
                ry = std::max(1, std::min(ry, gridSizeY - rh - 1));
                candidateRooms.push_back({rx, ry, rw, rh});
            }
        } else if (node.w >= minRoom && node.h >= minRoom && (int)candidateRooms.size() < allowedRooms * 2) {
            int rw = random.intInRange(minRoom, std::min(node.w, maxRoom));
            int rh = random.intInRange(minRoom, std::min(node.h, maxRoom));
            rw = std::min(rw, gridSizeX - 2);
            rh = std::min(rh, gridSizeY - 2);
            int rx = node.x + (node.w - rw) / 2;
            int ry = node.y + (node.h - rh) / 2;
            rx = std::max(1, std::min(rx, gridSizeX - rw - 1));
            ry = std::max(1, std::min(ry, gridSizeY - rh - 1));
            candidateRooms.push_back({rx, ry, rw, rh});
        }
    }
    // Filter out overlapping rooms
    std::vector<Room> rooms;
    for (const auto& room : candidateRooms) {
        bool overlaps = false;
        for (const auto& other : rooms) {
            if (!(room.x + room.w <= other.x || other.x + other.w <= room.x ||
                  room.y + room.h <= other.y || other.y + other.h <= room.y)) {
                overlaps = true;
                break;
            }
        }
        if (!overlaps) {
            rooms.push_back(room);
            if ((int)rooms.size() >= allowedRooms) break;
        }
    }

    // Draw rooms
    for (const auto& room : rooms) {
        for (int x = room.x; x < room.x + room.w; ++x) {
            for (int y = room.y; y < room.y + room.h; ++y) {
                grid.at(x, y) = Tile_Floor; // floor
            }
        }
    }
    // Connect rooms with short hallways using MST-like approach
    std::vector<std::pair<int, int>> hallwayPoints;
    if (!rooms.empty()) {
        std::vector<bool> connected(rooms.size(), false);
        connected[0] = true;
        size_t connectedCount = 1;
        while (connectedCount < rooms.size()) {
            int bestA = -1, bestB = -1, bestDist = 999999;
            for (size_t a = 0; a < rooms.size(); ++a) {
                if (!connected[a]) continue;
                for (size_t b = 0; b < rooms.size(); ++b) {
                    if (connected[b]) continue;
                    int dx = rooms[a].cx() - rooms[b].cx();
                    int dy = rooms[a].cy() - rooms[b].cy();
                    int dist = dx*dx + dy*dy;
                    if (dist < bestDist) {
                        bestDist = dist;
                        bestA = (int)a;
                        bestB = (int)b;
                    }
                }
            }
            if (bestA != -1 && bestB != -1) {
                // L-shaped hallway
                int x1 = rooms[bestA].cx(), y1 = rooms[bestA].cy();
                int x2 = rooms[bestB].cx(), y2 = rooms[bestB].cy();
                for (int x = std::min(x1, x2); x <= std::max(x1, x2); ++x) { grid.at(x, y1) = Tile_Hallway; hallwayPoints.emplace_back(x, y1); }
                for (int y = std::min(y1, y2); y <= std::max(y1, y2); ++y) { grid.at(x2, y) = Tile_Hallway; hallwayPoints.emplace_back(x2, y); }
                connected[bestB] = true;
                ++connectedCount;
            } else {
                break;
            }
        }
    }
    // Enclose hallways with walls (cardinal + diagonal directions)
    const int dx[8] = {1, -1, 0, 0, 1, 1, -1, -1};
    const int dy[8] = {0, 0, 1, -1, 1, -1, 1, -1};
    for (const auto& pt : hallwayPoints) {
        int x = pt.first, y = pt.second;
        for (int d = 0; d < 8; ++d) {
            int nx = x + dx[d], ny = y + dy[d];
            if (nx < 0 || ny < 0 || nx >= grid.width() || ny >= grid.height()) continue;
            if (grid.at(nx, ny) == Tile_Empty) grid.at(nx, ny) = Tile_Wall; // wall
        }
    }
    // Draw walls
    for (const auto& room : rooms) {
        for (int x = room.x - 1; x <= room.x + room.w; ++x) {
            for (int y = room.y - 1; y <= room.y + room.h; ++y) {
                if (x < 0 || y < 0 || x >= grid.width() || y >= grid.height()) continue;
                if (x >= room.x && x < room.x + room.w && y >= room.y && y < room.y + room.h) continue;
                if (grid.at(x, y) == Tile_Empty) grid.at(x, y) = Tile_Wall; // wall
            }
        }
    }
    // --- Place door_tags at hallway tiles with two opposing walls and a perpendicular room tile ---
    std::vector<sf::Vector3i> doorPositions;
    for (int x = 1; x < grid.width() - 1; ++x) {
        for (int y = 1; y < grid.height() - 1; ++y) {
            if (grid.at(x, y) != Tile_Hallway) continue;
            // Check for N/S walls
            if (grid.at(x, y-1) == Tile_Wall && grid.at(x, y+1) == Tile_Wall) {
                // Perpendicular: E/W
                bool eastRoom = grid.at(x+1, y+1) == Tile_Floor || grid.at(x+1, y-1) == Tile_Floor;
                bool westRoom = grid.at(x-1, y+1) == Tile_Floor || grid.at(x-1, y-1) == Tile_Floor;
                bool eastValid = eastRoom && (grid.at(x-1, y) == Tile_Floor || grid.at(x-1, y) == Tile_Hallway);
                bool westValid = westRoom && (grid.at(x+1, y) == Tile_Floor || grid.at(x+1, y) == Tile_Hallway);
                if (eastValid || westValid) {
                    int halfX = (gridSizeX - 1) / 2;
                    int halfY = (gridSizeY - 1) / 2;
                    sf::Vector3i pos = position + sf::Vector3i{x - halfX, y - halfY, 0};
                    doorPositions.push_back(pos);
                }
            }
            // Check for E/W walls
            if (grid.at(x-1, y) == Tile_Wall && grid.at(x+1, y) == Tile_Wall) {
                // Perpendicular: N/S
                bool northRoom = grid.at(x+1, y-1) == Tile_Floor || grid.at(x-1, y-1) == Tile_Floor;
                bool southRoom = grid.at(x+1, y+1) == Tile_Floor || grid.at(x-1, y+1) == Tile_Floor;
                bool northValid = northRoom && (grid.at(x, y+1) == Tile_Floor || grid.at(x, y+1) == Tile_Hallway);
                bool southValid = southRoom && (grid.at(x, y-1) == Tile_Floor || grid.at(x, y-1) == Tile_Hallway);
                if (northValid || southValid) {
                    int halfX = (gridSizeX - 1) / 2;
                    int halfY = (gridSizeY - 1) / 2;
                    sf::Vector3i pos = position + sf::Vector3i{x - halfX, y - halfY, 0};
                    doorPositions.push_back(pos);
                }
            }
        }
    }

    // --- Find boundary rooms and place entrances ---
    std::vector<std::pair<Room, std::vector<sf::Vector3i>>> boundaryEntrances; // room, possible entrance positions
    for (const auto& room : rooms) {
        std::vector<sf::Vector3i> possibleEntrances;
        // Check each wall of the room for boundary
        for (int x = room.x; x < room.x + room.w; ++x) {
            if (room.y - 1 == 0) // top
                possibleEntrances.push_back(position + sf::Vector3i{x - ((gridSizeX - 1) / 2), room.y - 1 - ((gridSizeY - 1) / 2), 0});
            if (room.y + room.h == gridSizeY - 1) // bottom
                possibleEntrances.push_back(position + sf::Vector3i{x - ((gridSizeX - 1) / 2), room.y + room.h - ((gridSizeY - 1) / 2), 0});
        }
        for (int y = room.y; y < room.y + room.h; ++y) {
            if (room.x - 1 == 0) // left
                possibleEntrances.push_back(position + sf::Vector3i{room.x - 1 - ((gridSizeX - 1) / 2), y - ((gridSizeY - 1) / 2), 0});
            if (room.x + room.w == gridSizeX - 1) // right
                possibleEntrances.push_back(position + sf::Vector3i{room.x + room.w - ((gridSizeX - 1) / 2), y - ((gridSizeY - 1) / 2), 0});
        }
        if (!possibleEntrances.empty())
            boundaryEntrances.emplace_back(room, possibleEntrances);
    }
    // Randomly select up to N entrances (without std::shuffle)
    int entranceCount = _entrances.isInfinite() ? 1 : random.intInRange(_entrances);
    std::vector<sf::Vector3i> entrancePositions;
    std::vector<size_t> availableIndices(boundaryEntrances.size());
    std::iota(availableIndices.begin(), availableIndices.end(), 0);
    for (int i = 0; i < entranceCount && !availableIndices.empty(); ++i) {
        int pick = random.intInRange(0, (int)availableIndices.size() - 1);
        size_t idx = availableIndices[pick];
        availableIndices.erase(availableIndices.begin() + pick);
        const auto& entry = boundaryEntrances[idx];
        if (!entry.second.empty()) {
            entrancePositions.push_back(entry.second[random.intInRange(0, (int)entry.second.size() - 1)]);
        }
    }
    // --- Tagging (add after other tags) ---
    for (const auto& pos : doorPositions) {
        if (!_doorTags.empty()) for (auto&& tag : _doorTags) result[tag].push_back(pos);
    }
    for (const auto& pos : entrancePositions) {
        if (!_doorTags.empty()) for (auto&& tag : _doorTags) result[tag].push_back(pos);
    }
    // Tagging
    int halfX = (gridSizeX - 1) / 2;
    int halfY = (gridSizeY - 1) / 2;
    for (auto&& tag : _centerTags) {
        result[tag].push_back(position);
    }
    for (int x = 0; x < grid.width(); ++x) {
        for (int y = 0; y < grid.height(); ++y) {
            sf::Vector3i pos = position + sf::Vector3i{x - halfX, y - halfY, 0};
            if (grid.at(x, y) == Tile_Floor && !_floorTags.empty()) {
                for (auto&& tag : _floorTags) result[tag].push_back(pos);
            }
            if (grid.at(x, y) == Tile_Wall && !_wallTags.empty()) {
                for (auto&& tag : _wallTags) result[tag].push_back(pos);
            }
            if (grid.at(x, y) == Tile_Hallway && !_hallwayTags.empty()) {
                for (auto&& tag : _hallwayTags) result[tag].push_back(pos);
            }
        }
    }
    return result;
}
