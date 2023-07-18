#include "pch.h"
#include "MachineFactory.h"

bool drft::MachineFactory::loadPrototypes(const std::string& JSONfilename)
{
    _machines.emplace("NULL", Machine());
    // Parse JSON and make derived machines
    using namespace rapidjson;
    std::string filepath = ".\\data\\static\\machines\\" + JSONfilename;

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

    if (!doc.HasMember("Machines") || doc.HasParseError())
    {
        std::cout << JSONfilename << " could not be parsed." << std::endl;
        return false;
    }

    std::cout << "Parsing " << JSONfilename << "..." << std::endl;

    for (auto& machine : doc["Machines"].GetObject())
    {
        _machines.emplace(machine.name.GetString(), Machine());
        sf::Vector2i bounds;
        const auto machineObject = machine.value.GetObject();
        if (machineObject.HasMember("Prefabs"))
        {
            for (auto& prefab : machineObject["Prefabs"].GetArray())
            {

            }
        }
        if (machineObject.HasMember("Entities"))
        {
            for (auto& entity : machineObject["Entities"].GetArray())
            {
                auto entityObject = entity.GetObject();
                auto& entityPositionPair = *entityObject.MemberBegin();
                auto entityName = entityPositionPair.name.GetString();
                auto xPos = entityPositionPair.value.GetArray()[0].GetInt();
                auto yPos = entityPositionPair.value.GetArray()[1].GetInt();
                bounds.x = std::max(bounds.x, xPos);
                bounds.y = std::max(bounds.y, yPos);
                _machines.at(machine.name.GetString()).addEntity(entityName, { xPos, yPos });
            }
        }
        _machines.at(machine.name.GetString()).setBounds(bounds);
    }
}

const drft::Machine& drft::MachineFactory::build(const std::string& name) const
{
    if (!_machines.contains(name)) return _machines.at("NULL");
    return _machines.at(name);
}
