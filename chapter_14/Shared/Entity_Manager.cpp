#include "Entity_Manager.h"
#include "System_Manager.h"

EntityManager::EntityManager(SystemManager* l_sysMgr): m_idCounter(0), m_systems(l_sysMgr){}
EntityManager::~EntityManager(){ Purge(); }

void EntityManager::SetSystemManager(SystemManager* l_sysMgr){
	m_systems = l_sysMgr;
}

int EntityManager::AddEntity(const Bitmask& l_mask, int l_id){
	unsigned int entity = (l_id != -1 ? l_id : m_idCounter);
	if (!m_entities.emplace(entity, EntityData()).second){ return -1; }
	if (l_id == -1){ ++m_idCounter; }
	for(int i = 0; i < N_COMPONENT_TYPES; ++i){
		if(l_mask.GetBit(i)){ AddComponent(entity,(Component)i); }
	}

	// Notifying the system manager of a modified entity.
	m_systems->EntityModified(entity,l_mask);
	m_systems->AddEvent(entity,(EventID)EntityEvent::Spawned);
	return entity;
}

int EntityManager::AddEntity(const std::string& l_entityFile, int l_id){
	int EntityId = -1;

	std::ifstream file;
	file.open(Utils::GetWorkingDirectory() + 
		"media/Entities/" + l_entityFile + ".entity");
	if (!file.is_open()){
		std::cout << "! Failed to load entity: " << l_entityFile << std::endl;
		return -1;
	}
	std::string line;
	while(std::getline(file,line)){
		if (line[0] == '|'){ continue; }
		std::stringstream keystream(line);
		std::string type;
		keystream >> type;
		if(type == "Name"){
				
		} else if(type == "Attributes"){
			if (EntityId != -1){ continue; }
			Bitset set = 0;
			Bitmask mask;
			keystream >> set;
			mask.SetMask(set);
			EntityId = AddEntity(mask);
			if (EntityId == -1){ return -1; }
		} else if(type == "Component"){
			if (EntityId == -1){ continue; }
			unsigned int c_id = 0;
			keystream >> c_id;
			C_Base* component = GetComponent<C_Base>(EntityId, (Component)c_id);
			if (!component){ continue; }
			keystream >> *component;
		} // Unknown entry.
	}
	file.close();
	m_entities.at(EntityId).m_type = l_entityFile;
	return EntityId;
}

bool EntityManager::RemoveEntity(const EntityId& l_id){
	auto itr = m_entities.find(l_id);
	if (itr == m_entities.end()){ return false; }
	Message msg((MessageType)EntityMessage::Removed_Entity);
	msg.m_receiver = l_id;
	msg.m_int = l_id;
	m_systems->GetMessageHandler()->Dispatch(msg);
	// Removing all components.
	while(itr->second.m_components.begin() != itr->second.m_components.end()){
		delete itr->second.m_components.back();
		itr->second.m_components.pop_back();
	}
	m_entities.erase(itr);
	m_systems->RemoveEntity(l_id);
	return true;
}

bool EntityManager::HasEntity(const EntityId& l_id){
	return (m_entities.find(l_id) != m_entities.end());
}

bool EntityManager::AddComponent(const EntityId& l_entity, const Component& l_component)
{
	auto itr = m_entities.find(l_entity);
	if (itr == m_entities.end()){ return false; }
	if (itr->second.m_bitmask.GetBit((unsigned int)l_component)){ return false; }
	// Component doesn't exist.
	auto itr2 = m_cFactory.find(l_component);
	if (itr2 == m_cFactory.end()){ return false; }
	// Component type does exist.
	C_Base* component = itr2->second();
	itr->second.m_components.emplace_back(component);
	itr->second.m_bitmask.TurnOnBit((unsigned int)l_component);
	// Notifying the system manager of a modified entity.
	m_systems->EntityModified(l_entity,itr->second.m_bitmask);
	return true;
}

bool EntityManager::RemoveComponent(const EntityId& l_entity, const Component& l_component)
{
	auto itr = m_entities.find(l_entity);
	if (itr == m_entities.end()){ return false; }
	// Found the entity.
	if (!itr->second.m_bitmask.GetBit((unsigned int)l_component)){ return false; }
	// Component exists.
	auto& container = itr->second.m_components;
	auto component = std::find_if(container.begin(), container.end(),
		[&l_component](C_Base* c){ return c->GetType() == l_component; });
	if (component == container.end()){ return false; }
	delete (*component);
	container.erase(component);
	itr->second.m_bitmask.ClearBit((unsigned int)l_component);

	m_systems->EntityModified(l_entity, itr->second.m_bitmask);
	return true;
}

bool EntityManager::HasComponent(const EntityId& l_entity, const Component& l_component)
{
	auto itr = m_entities.find(l_entity);
	if (itr == m_entities.end()){ return false; }
	return itr->second.m_bitmask.GetBit((unsigned int)l_component);
}

void EntityManager::Purge(){
	if (m_systems){ m_systems->PurgeEntities(); }
	for (auto& entity : m_entities){
		for (auto &component : entity.second.m_components){ delete component; }
		entity.second.m_components.clear();
		entity.second.m_bitmask.Clear();
	}
	m_entities.clear();
	m_idCounter = 0;
}

unsigned int EntityManager::GetEntityCount(){ return m_entities.size(); }