#include "S_Movement.h"
#include "System_Manager.h"
#include "Map.h"

S_Movement::S_Movement(SystemManager* l_systemMgr)
	: S_Base(System::Movement, l_systemMgr)
{
	Bitmask req;
	req.TurnOnBit((unsigned int)Component::Position);
	req.TurnOnBit((unsigned int)Component::Movable);
	m_requiredComponents.push_back(req);
	req.Clear();

	m_systemManager->GetMessageHandler()->Subscribe(EntityMessage::Is_Moving, this);

	m_gameMap = nullptr;
}

S_Movement::~S_Movement(){}

void S_Movement::Update(float l_dT){
	if (!m_gameMap){ return; }
	EntityManager* entities = m_systemManager->GetEntityManager();
	for (auto &entity : m_entities){
		C_Position* position = entities->GetComponent<C_Position>(entity, Component::Position);
		C_Movable* movable = entities->GetComponent<C_Movable>(entity, Component::Movable);
		MovementStep(l_dT, movable, position);
		position->MoveBy(movable->GetVelocity() * l_dT);
	}
}

void S_Movement::HandleEvent(const EntityId& l_entity,
	const EntityEvent& l_event)
{
	switch (l_event){
	case EntityEvent::Colliding_X: StopEntity(l_entity, Axis::x); break;
	case EntityEvent::Colliding_Y: StopEntity(l_entity, Axis::y); break;
	case EntityEvent::Moving_Left: SetDirection(l_entity, Direction::Left); break;
	case EntityEvent::Moving_Right: SetDirection(l_entity, Direction::Right); break;
	case EntityEvent::Moving_Up:
	{
		C_Movable* mov = m_systemManager->GetEntityManager()->GetComponent<C_Movable>(l_entity, Component::Movable);
		if (mov->GetVelocity().x == 0){ SetDirection(l_entity, Direction::Up); }
	}
		break;
	case EntityEvent::Moving_Down:
	{
		C_Movable* mov = m_systemManager->GetEntityManager()->GetComponent<C_Movable>(l_entity, Component::Movable);
		if (mov->GetVelocity().x == 0){ SetDirection(l_entity, Direction::Down); }
	}
		break;
	}
}

void S_Movement::Notify(const Message& l_message){
	EntityManager* eMgr = m_systemManager->GetEntityManager();
	EntityMessage m = (EntityMessage)l_message.m_type;
	switch (m){
	case EntityMessage::Is_Moving:
	{
		if (!HasEntity(l_message.m_receiver)){ return; }
		C_Movable* movable = eMgr->GetComponent<C_Movable>
			(l_message.m_receiver, Component::Movable);
		if (movable->GetVelocity() != sf::Vector2f(0.0f, 0.0f)){ return; }
		m_systemManager->AddEvent(l_message.m_receiver, (EventID)EntityEvent::Became_Idle);
	}
		break;
	}
}

const sf::Vector2f& S_Movement::GetTileFriction(unsigned int l_elevation,
	unsigned int l_x, unsigned int l_y)
{
	Tile* t = nullptr;
	while (!t && l_elevation >= 0){
		t = m_gameMap->GetTile(l_x, l_y, l_elevation);
		--l_elevation;
	}

	return(t ? t->m_properties->m_friction :
		m_gameMap->GetDefaultTile()->m_friction);
}

void S_Movement::MovementStep(float l_dT, C_Movable* l_movable, C_Position* l_position){
	sf::Vector2f f_coefficient = GetTileFriction(l_position->GetElevation(),
		floor(l_position->GetPosition().x / Sheet::Tile_Size),
		floor(l_position->GetPosition().y / Sheet::Tile_Size));

	sf::Vector2f friction(l_movable->GetSpeed().x * f_coefficient.x,
		l_movable->GetSpeed().y * f_coefficient.y);

	l_movable->AddVelocity(l_movable->GetAcceleration() * l_dT);
	l_movable->SetAcceleration(sf::Vector2f(0.0f, 0.0f));
	l_movable->ApplyFriction(friction * l_dT);

	float magnitude = sqrt(
		(l_movable->GetVelocity().x * l_movable->GetVelocity().x) +
		(l_movable->GetVelocity().y * l_movable->GetVelocity().y));

	if (magnitude <= l_movable->GetMaxVelocity()){ return; }
	float max_V = l_movable->GetMaxVelocity();
	l_movable->SetVelocity(sf::Vector2f(
		(l_movable->GetVelocity().x / magnitude) * max_V,
		(l_movable->GetVelocity().y / magnitude) * max_V));
}

void S_Movement::SetMap(Map* l_gameMap){ m_gameMap = l_gameMap; }

void S_Movement::StopEntity(const EntityId& l_entity,
	const Axis& l_axis)
{
	C_Movable* movable = m_systemManager->GetEntityManager()->GetComponent<C_Movable>(l_entity, Component::Movable);
	if (l_axis == Axis::x){
		movable->SetVelocity(sf::Vector2f(0.f, movable->GetVelocity().y));
	} else if (l_axis == Axis::y){
		movable->SetVelocity(sf::Vector2f(movable->GetVelocity().x, 0.f));
	}
}

void S_Movement::SetDirection(const EntityId& l_entity, const Direction& l_dir)
{
	C_Movable* movable = m_systemManager->GetEntityManager()->GetComponent<C_Movable>(l_entity, Component::Movable);
	movable->SetDirection(l_dir);

	Message msg((MessageType)EntityMessage::Direction_Changed);
	msg.m_receiver = l_entity;
	msg.m_int = (int)l_dir;
	m_systemManager->GetMessageHandler()->Dispatch(msg);
}