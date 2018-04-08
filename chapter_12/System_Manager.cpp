#include "System_Manager.h"
#include "Entity_Manager.h"

SystemManager::SystemManager(): m_entityManager(nullptr){
	m_systems[System::State] = new S_State(this);
	m_systems[System::Control] = new S_Control(this);
	m_systems[System::Movement] = new S_Movement(this);
	m_systems[System::Collision] = new S_Collision(this);
	m_systems[System::SheetAnimation] = new S_SheetAnimation(this);
	m_systems[System::Sound] = new S_Sound(this);
	m_systems[System::Renderer] = new S_Renderer(this);
}

SystemManager::~SystemManager(){
	PurgeSystems();
}

void SystemManager::SetEntityManager(EntityManager* l_entityMgr){
	if(!m_entityManager){ m_entityManager = l_entityMgr; }
}

EntityManager* SystemManager::GetEntityManager(){
	return m_entityManager;
}

MessageHandler* SystemManager::GetMessageHandler(){
	return &m_messages;
}

void SystemManager::AddEvent(const EntityId& l_entity, 
	const EventID& l_event)
{
	m_events[l_entity].AddEvent(l_event);
}

void SystemManager::Update(float l_dT){
	for(auto &itr : m_systems){
		itr.second->Update(l_dT);
	}
	HandleEvents();
}

void SystemManager::HandleEvents(){
	for(auto &event : m_events){
		EventID id = 0;
		while(event.second.ProcessEvents(id)){
			for(auto &system : m_systems)
			{
				if(system.second->HasEntity(event.first)){
					system.second->HandleEvent(event.first,(EntityEvent)id);
				}
			}
		}
	}
}

void SystemManager::Draw(Window* l_wind, unsigned int l_elevation)
{
	auto itr = m_systems.find(System::Renderer);
	if (itr == m_systems.end()){ return; }
	S_Renderer* system = (S_Renderer*)itr->second;
	system->Render(l_wind, l_elevation);
}

void SystemManager::EntityModified(const EntityId& l_entity, const Bitmask& l_bits)
{
	for(auto &s_itr : m_systems){
		S_Base* system = s_itr.second;
		if(system->FitsRequirements(l_bits)){
			if(!system->HasEntity(l_entity)){
				system->AddEntity(l_entity);
			}
		} else {
			if(system->HasEntity(l_entity)){
				system->RemoveEntity(l_entity);
			}
		}
	}
}

void SystemManager::RemoveEntity(const EntityId& l_entity){
	for(auto &system : m_systems){
		system.second->RemoveEntity(l_entity);
	}
}

void SystemManager::PurgeEntities(){
	for(auto &system : m_systems){
		system.second->Purge();
	}
}

void SystemManager::PurgeSystems(){
	for (auto &system : m_systems){
		delete system.second;
	}
	m_systems.clear();
}