#pragma once
#include <AI/Utility/IUtilityBlackboard.h>

class Blackboard : public IUtilityBlackboard<entt::entity>
{
public:
	using Data = IUtilityBlackboard::Data;

	using DataList = std::vector<Data>;
	using DataName = entt::id_type;
	
	void set(DataName name, Data data);
	Data get(DataName name) const override;

	void pushToList(DataName name, Data data);
	const DataList& getList(DataName name) const override;

	// Clear all data from the blackboard
	void clear();
	// Clear an entry from the blackboard
	void clear(DataName name);

private:
	std::unordered_map<DataName, Data> _singleData;
	std::unordered_map<DataName, DataList> _listData;
};