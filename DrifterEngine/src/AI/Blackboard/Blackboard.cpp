#include "pch.h"
#include "Blackboard.h"

static const Blackboard::DataList EmptyDataList = {};

void Blackboard::set(DataName name, Data data)
{
    _singleData.emplace(name, std::move(data));
}

Blackboard::Data Blackboard::get(DataName name) const
{
    if (!_singleData.contains(name))
    {
        return std::monostate{};
    }
    return _singleData.at(name);
}

void Blackboard::pushToList(DataName name, Data data)
{
    DataList& list = _listData[name];
    list.push_back(data);
}

const Blackboard::DataList& Blackboard::getList(DataName name) const
{
    if (!_listData.contains(name))
    {
        return EmptyDataList;
    }
    return _listData.at(name);
}

void Blackboard::clear()
{
    _singleData.clear();
    _listData.clear();
}

void Blackboard::clear(DataName name)
{
    _singleData.erase(name);
    _listData.erase(name);
}
