#pragma once
#include "C_Base.h"
#include "Bitmask.h"
#include <unordered_map>
#include <vector>
#include <functional>
#include <fstream>
#include "Utilities.h"

using EntityId = int;
using ComponentContainer = std::vector<C_Base*>;

struct EntityData{
	Bitmask m_bitmask;
	std::string m_type;
	ComponentContainer m_components;
};

using EntityContainer = std::unordered_map<EntityId,EntityData>;
using ComponentFactory = std::unordered_map<Component, std::function<C_Base*(void)>>;

class SystemManager;

class EntityManager{
public:
	EntityManager(SystemManager* l_sysMgr);
	virtual ~EntityManager();

	void SetSystemManager(SystemManager* l_sysMgr);

	int AddEntity(const Bitmask& l_mask, int l_id = -1);
	virtual int AddEntity(const std::string& l_entityFile, int l_id = -1);
	bool RemoveEntity(const EntityId& l_id);

	bool HasEntity(const EntityId& l_id);

	bool AddComponent(const EntityId& l_entity, const Component& l_component);

	template<class T>
	T* GetComponent(const EntityId& l_entity, const Component& l_component)
	{
		auto itr = m_entities.find(l_entity);
		if (itr == m_entities.end()){ return nullptr; }
		// Found the entity.
		if (!itr->second.m_bitmask.GetBit((unsigned int)l_component)){ return nullptr; }
		// Component exists.
		auto& container = itr->second.m_components;
		auto component = std::find_if(container.begin(), container.end(),
			[&l_component](C_Base* c){ return c->GetType() == l_component; });
		return (component != container.end() ? dynamic_cast<T*>(*component) : nullptr);
	}

	bool RemoveComponent(const EntityId& l_entity, const Component& l_component);
	bool HasComponent(const EntityId& l_entity, const Component& l_component);

	void Purge();

	unsigned int GetEntityCount();
protected:
	template<class T>
	void AddComponentType(const Component& l_id){
		m_cFactory[l_id] = []() -> C_Base* { return new T(); };
	}

	// Data members
	unsigned int m_idCounter;
	EntityContainer m_entities;
	ComponentFactory m_cFactory;

	SystemManager* m_systems;
};