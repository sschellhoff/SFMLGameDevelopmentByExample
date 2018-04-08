#include "S_Base.h"
#include "System_Manager.h"

S_Base::S_Base(const System& l_id, SystemManager* l_systemMgr)
	: m_id(l_id), m_systemManager(l_systemMgr){}

S_Base::~S_Base(){ Purge(); }

bool S_Base::AddEntity(const EntityId& l_entity){
	if (HasEntity(l_entity)){ return false; }
	m_entities.emplace_back(l_entity);
	return true;
}

bool S_Base::HasEntity(const EntityId& l_entity){
	return std::find(m_entities.begin(),
		m_entities.end(), l_entity) != m_entities.end();
}

bool S_Base::RemoveEntity(const EntityId& l_entity){
	auto entity = std::find_if(m_entities.begin(), m_entities.end(),
		[&l_entity](EntityId& id){ return id == l_entity; });
	if (entity == m_entities.end()){ return false; }
	m_entities.erase(entity);
	return true;
}

System S_Base::GetId(){ return m_id; }

bool S_Base::FitsRequirements(const Bitmask& l_bits){
	return std::find_if(m_requiredComponents.begin(), m_requiredComponents.end(),
		[&l_bits](Bitmask& b){ return b.Matches(l_bits, b.GetMask()); }) != m_requiredComponents.end();
}

void S_Base::Purge(){ m_entities.clear(); }