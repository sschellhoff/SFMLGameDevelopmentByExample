#include "S_Timers.h"
#include "System_Manager.h"

S_Timers::S_Timers(SystemManager* l_systemMgr)
	: S_Base(System::Timers, l_systemMgr)
{
	Bitmask req;
	req.TurnOnBit((unsigned int)Component::State);
	req.TurnOnBit((unsigned int)Component::Attacker);
	m_requiredComponents.push_back(req);
	req.ClearBit((unsigned int)Component::Attacker);
	req.TurnOnBit((unsigned int)Component::Health);
	m_requiredComponents.push_back(req);
}

S_Timers::~S_Timers(){}

void S_Timers::Update(float l_dT){
	EntityManager* entities = m_systemManager->GetEntityManager();
	for (auto &entity : m_entities){
		EntityState state = entities->GetComponent<C_State>(entity, Component::State)->GetState();
		if (state == EntityState::Attacking){
			C_Attacker* attack = entities->GetComponent<C_Attacker>(entity, Component::Attacker);
			attack->AddToTimer(sf::seconds(l_dT));
			if (attack->GetTimer().asMilliseconds() < attack->GetAttackDuration()){ continue; }
			attack->Reset();
			attack->SetAttacked(false);
		} else if (state == EntityState::Hurt || state == EntityState::Dying){
			C_Health* health = entities->GetComponent<C_Health>(entity, Component::Health);
			health->AddToTimer(sf::seconds(l_dT));
			if ((state == EntityState::Hurt && health->GetTimer().asMilliseconds() < health->GetHurtDuration()) ||
				(state == EntityState::Dying && health->GetTimer().asMilliseconds() < health->GetDeathDuration())){
				continue;
			}

			health->Reset();
			if (state == EntityState::Dying){
				Message msg((MessageType)EntityMessage::Respawn);
				msg.m_receiver = entity;
				m_systemManager->GetMessageHandler()->Dispatch(msg);
				health->ResetHealth();
			}
		} else { continue; }
		m_systemManager->GetSystem<S_State>(System::State)->ChangeState(entity, EntityState::Idle, true);
	}
}

void S_Timers::HandleEvent(const EntityId& l_entity, const EntityEvent& l_event){}
void S_Timers::Notify(const Message& l_message){}