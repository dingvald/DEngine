#include "pch.h"
#include "StructureFactory.h"
#include "Spatial/Helpers.h"
#include "StaticStructure.h"
#include "ProceduralStructure.h"

static const std::string STATIC_DATA_PATH = ".\\data\\static\\";

bool drft::StructureFactory::loadStructures(const std::string& JSONfilename)
{
	using namespace rapidjson;

	std::string filepath = STATIC_DATA_PATH + "structures\\" + JSONfilename;

	std::ifstream ifs{ filepath };

	std::cout << "Loading " << JSONfilename << "..." << std::endl;

	if (!ifs.is_open())
	{
		std::cerr << "Could not open file " << filepath << std::endl;
		return false;
	}
	IStreamWrapper isw{ ifs };
	Document doc{};
	doc.ParseStream<kParseCommentsFlag>(isw);

	if (!doc.HasMember("Structure") || doc.HasParseError())
	{
		std::cout << JSONfilename << " could not be parsed." << std::endl;
		return false;
	}

	std::cout << "Parsing " << JSONfilename << "..." << std::endl;

	// Iterate each structure
	for (auto&& structure : doc["Structure"].GetObject())
	{
        std::string structureName = structure.name.GetString();
        std::string type = structure.value["type"].GetString();
        if (type == "static")
        {
            auto newStructure = std::make_unique<StaticStructure>(structureName);
            sf::Vector2i size;
            for (auto&& entity : structure.value["entity_positions"].GetObject())
            {
                std::string entityName = entity.name.GetString();
                std::vector<sf::Vector2i> positions;
                for (auto&& position : entity.value.GetArray())
                {
                    sf::Vector2i pos;
                    pos.x = position.GetArray()[0].GetInt();
                    pos.y = position.GetArray()[1].GetInt();
                    size.x = std::max(size.x, pos.x);
                    size.y = std::max(size.y, pos.y);
                    positions.push_back(pos);
                }
                newStructure->addEntity(entityName, std::move(positions));
            }
            newStructure->setMinimumBounds(size);
            newStructure->setMaximumBounds(size);
            if (structure.value.HasMember("can_rotate"))
            {
                newStructure->setCanRotate(structure.value["can_rotate"].GetBool());
            }
            if (structure.value.HasMember("can_mirror"))
            {
                newStructure->setCanMirror(structure.value["can_mirror"].GetBool());
            }

            _structures.insert({ structureName, std::move(newStructure) });
        }
        else if (type == "procedural")
        {
            throw std::exception("No implementation yet.");
            auto newStructure = std::make_unique<ProceduralStructure>(structureName);
            if (structure.value.HasMember("size"))
            {
                int x = structure.value["size"].GetArray()[0].GetInt();
                int y = structure.value["size"].GetArray()[1].GetInt();
                newStructure->setMaximumBounds({ x, y });
                newStructure->setMinimumBounds({ x, y });
            }
            else
            {
                if (!structure.value.HasMember("minimum_size") || !structure.value.HasMember("maximum_size"))
                {
                    throw std::exception("Structure needs to declare its size");
                }
                int min_x = structure.value["minimum_size"].GetArray()[0].GetInt();
                int min_y = structure.value["minimum_size"].GetArray()[1].GetInt();
                newStructure->setMinimumBounds({ min_x, min_y });
                int max_x = structure.value["maximum_size"].GetArray()[0].GetInt();
                int max_y = structure.value["maximum_size"].GetArray()[1].GetInt();
                newStructure->setMaximumBounds({ max_x, max_y });
            }
            _structures.insert({ structureName, std::move(newStructure) });
        }
	}
    return true;
}

void drft::StructureFactory::resolveAllSubstructures()
{
    std::unordered_set<std::string> fullyResolvedStructures;
    for (auto& [name, structure] : _structures)
    {
        resolve(*structure, fullyResolvedStructures);
    }
}

const drft::StructureBase& drft::StructureFactory::build(const std::string& structureName) const
{
    if (!_structures.contains(structureName)) throw std::exception("No such structure exists.");
    return *_structures.at(structureName);
}

void drft::StructureFactory::resolve(StructureBase& toResolve, std::unordered_set<std::string>& resolved)
{
    if (resolved.contains(toResolve.name())) return;

    int min_x = toResolve.getMinimumBounds().x;
    int min_y = toResolve.getMinimumBounds().y;
    int max_x = toResolve.getMaximumBounds().x;
    int max_y = toResolve.getMaximumBounds().y;

    for (auto& [name, relativePosition] : toResolve._substructures)
    {
        if (!_structures.contains(name)) throw std::exception("Substructure name does not exist.");
        if (!resolved.contains(name))
        {
            resolve(*_structures.at(name), resolved);
        }
        const auto& substructure = _structures.at(name);
        auto min = substructure->getMinimumBounds();
        auto max = substructure->getMaximumBounds();
        min_x = std::min(min.x, min_x + relativePosition.x);
        min_y = std::min(min.y, min_y + relativePosition.y);
        max_x = std::max(max.x, max_x + relativePosition.x);
        max_y = std::max(max.y, max_y + relativePosition.y);
    }

    toResolve.setMinimumBounds({ min_x, min_y });
    toResolve.setMaximumBounds({ max_x, max_y });
    resolved.insert(toResolve.name());
}
