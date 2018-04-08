#include "S_Combat.h"
#include "System_Manager.h"

S_Combat::S_Combat(SystemManager* l_systemMgr)
	: S_Base(System::Combat, l_systemMgr)
{
	Bitmask req;
	req.TurnOnBit((unsigned int)Component::Position);
	req.TurnOnBit((unsigned int)Component::Movable);
	req.TurnOnBit((unsigned int)Component::State);
	req.TurnOnBit((unsigned int)Component::Health);
	m_requiredComponents.push_back(req);
	req.ClearBit((unsigned int)Component::Health);
	req.TurnOnBit((unsigned int)Component::Attacker);
	m_requiredComponents.push_back(req);

	m_systemManager->GetMessageHandler()->Subscribe(EntityMessage::Being_Attacked, this);
}

S_Combat::~S_Combat(){}

void S_Combat::Update(float l_dT){
	EntityManager* entities = m_systemManager->GetEntityManager();
	for (auto &entity : m_entities){
		C_Attacker* attack = entities->GetComponent<C_Attacker>(entity, Component::Attacker);
		if (!attack){ continue; }
		sf::Vector2f offset = attack->GetOffset();
		sf::FloatRect AoA = attack->GetAreaOfAttack();
		Direction dir = entities->GetComponent<C_Movable>(entity, Component::Movable)->GetDirection();
		sf::Vector2f position = entities->GetComponent<C_Position>(entity, Component::Position)->GetPosition();
		if (dir == Direction::Left){ offset.x -= AoA.width / 2; }
		else if (dir == Direction::Right){ offset.x += AoA.width / 2; }
		else if (dir == Direction::Up){ offset.y -= AoA.height / 2; }
		else if (dir == Direction::Down){ offset.y += AoA.height / 2; }
		position -= sf::Vector2f(AoA.width / 2, AoA.height / 2);
		attack->SetAreaPosition(position + offset);
	}
}

void S_Combat::HandleEvent(const EntityId& l_entity, const EntityEvent& l_event){}

void S_Combat::Notify(const Message& l_message){
	if (!HasEntity(l_message.m_receiver) || !HasEntity(l_message.m_sender)){ return; }
	EntityManager* entities = m_systemManager->GetEntityManager();
	EntityMessage m = (EntityMessage)l_message.m_type;
	switch (m){
	case EntityMessage::Being_Attacked:
		C_Health* victim = entities->GetComponent<C_Health>(l_message.m_receiver, Component::Health);
		C_Attacker* attacker = entities->GetComponent<C_Attacker>(l_message.m_sender, Component::Attacker);
		if (!victim || !attacker){ return; }
		S_State* StateSystem = m_systemManager->GetSystem<S_State>(System::State);
		if (StateSystem->GetState(l_message.m_sender) != EntityState::Attacking){ return; }
		if (attacker->HasAttacked()){ return; }
		// Begin attacking.
		victim->SetHealth((victim->GetHealth() > 1 ? victim->GetHealth() - 1 : 0));
		attacker->SetAttacked(true);
		if (!victim->GetHealth()){
			StateSystem->ChangeState(l_message.m_receiver, EntityState::Dying, true);
		} else {
			Message msg((MessageType)EntityMessage::Hurt);
			msg.m_receiver = l_message.m_receiver;
			m_systemManager->GetMessageHandler()->Dispatch(msg);
		}

		// Knockback.
		Direction attackerDirection = entities->GetComponent<C_Movable>(l_message.m_sender, Component::Movable)->GetDirection();
		float Knockback = attacker->GetKnockback();
		sf::Vector2f KnockbackVelocity;
		if (attackerDirection == Direction::Left || attackerDirection == Direction::Up){ Knockback = -Knockback; }
		if (attackerDirection == Direction::Left || attackerDirection == Direction::Right){ KnockbackVelocity.x = Knockback; }
		else{ KnockbackVelocity.y = Knockback; }
		entities->GetComponent<C_Movable>(l_message.m_receiver, Component::Movable)->SetVelocity(KnockbackVelocity);
		break;
	}
}