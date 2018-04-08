#pragma once
#include <SFML/Graphics.hpp>
#include "Event_Queue.h"
#include "MessageHandler.h"
#include <unordered_map>
#include "S_Base.h"

using SystemContainer = std::unordered_map<System, S_Base*>;
using EntityEventContainer = std::unordered_map<EntityId, EventQueue>;

class EntityManager;

class SystemManager{
public:
	SystemManager();
	virtual ~SystemManager();

	void SetEntityManager(EntityManager* l_entityMgr);
	EntityManager* GetEntityManager();
	MessageHandler* GetMessageHandler();

	template<class T>
	T* GetSystem(const System& l_system){
		auto itr = m_systems.find(l_system);
		return(itr != m_systems.end() ? dynamic_cast<T*>(itr->second) : nullptr);
	}

	template<class T>
	bool AddSystem(const System& l_id){
		if (m_systems.find(l_id) != m_systems.end()){ return false; }
		m_systems[l_id] = new T(this);
		return true;
	}
	
	void AddEvent(const EntityId& l_entity, const EventID& l_event);

	void Update(float l_dT);
	void HandleEvents();

	void EntityModified(const EntityId& l_entity, const Bitmask& l_bits);
	void RemoveEntity(const EntityId& l_entity);
	
	void PurgeEntities();
	void PurgeSystems();
protected:
	SystemContainer m_systems;
	EntityManager* m_entityManager;
	EntityEventContainer m_events;
	MessageHandler m_messages;
};