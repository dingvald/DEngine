#include "pch.h"
#include "StructureFactory.h"
#include "Spatial/Helpers.h"

bool drft::StructureFactory::loadStructures(const std::string& JSONfilename)
{
    return false;
}

void drft::StructureFactory::initAllStructures()
{
    // creat a temp resolved cache?
    for (auto& [name, structure] : _structures)
    {
        int min_x = std::numeric_limits<int>::max();
        int min_y = std::numeric_limits<int>::max();
        int max_x = 0;
        int max_y = 0;

        // Resolve substructures
        // TODO: Needs to be recursive
        for (auto& [substructureName, relativePosition] : structure->_substructures)
        {
            auto& substructure = _structures.at(substructureName);
            auto min = substructure->getMinimumBounds();
            auto max = substructure->getMaximumBounds();
            min_x = std::min(min.x, min_x + relativePosition.x);
            min_y = std::min(min.y, min_y + relativePosition.y);
            max_x = std::max(max.x, max_x + relativePosition.x);
            max_y = std::max(max.y, max_y + relativePosition.y);
        }

        structure->setMinimumBounds({ min_x, min_y });
        structure->setMaximumBounds({ max_x, max_y });
    }

}

drft::StructureBase& drft::StructureFactory::build(const std::string& structureName) const
{
    if (!_structures.contains(structureName)) throw std::exception("No such structure exists.");
    return *_structures.at(structureName);
}
