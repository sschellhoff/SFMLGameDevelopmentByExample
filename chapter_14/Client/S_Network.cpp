#include "S_Network.h"
#include "System_Manager.h"
#include "S_Renderer.h"

void InterpolateSnapshot(const EntitySnapshot& l_s1, const sf::Int32& T1,
	const EntitySnapshot& l_s2, const sf::Int32& T2, EntitySnapshot& l_target, const sf::Int32& T_X)
{
	l_target.m_direction = l_s2.m_direction;
	l_target.m_state = l_s1.m_state;
	l_target.m_health = l_s2.m_health;
	l_target.m_name = l_s2.m_name;
	l_target.m_elevation = l_s1.m_elevation;

	l_target.m_position.x = Interpolate<float>(
		T1, T2, l_s1.m_position.x, l_s2.m_position.x, T_X);
	l_target.m_position.y = Interpolate<float>(
		T1, T2, l_s1.m_position.y, l_s2.m_position.y, T_X);

	l_target.m_velocity.x = Interpolate<float>(
		T1, T2, l_s1.m_velocity.x, l_s2.m_velocity.x, T_X);
	l_target.m_velocity.y = Interpolate<float>(
		T1, T2, l_s1.m_velocity.y, l_s2.m_velocity.y, T_X);

	l_target.m_acceleration.x = Interpolate<float>(
		T1, T2, l_s1.m_acceleration.x, l_s2.m_acceleration.x, T_X);
	l_target.m_acceleration.y = Interpolate<float>(
		T1, T2, l_s1.m_acceleration.y, l_s2.m_acceleration.y, T_X);
}

bool CompareSnapshots(const EntitySnapshot& l_s1, const EntitySnapshot& l_s2,
	bool l_position, bool l_physics, bool l_state)
{
	if (l_position && (l_s1.m_position != l_s2.m_position || 
		l_s1.m_elevation != l_s2.m_elevation))
	{ return false; }
	if (l_physics && (l_s1.m_velocity != l_s2.m_velocity ||
		l_s1.m_acceleration != l_s2.m_acceleration ||
		l_s1.m_direction != l_s2.m_direction))
	{ return false; }
	if (l_state && (l_s1.m_state != l_s2.m_state))
	{ return false; }
	return true;
}

S_Network::S_Network(SystemManager* l_systemMgr)
	: S_Base(System::Network, l_systemMgr), m_client(nullptr)
{
	Bitmask req;
	req.TurnOnBit((unsigned int)Component::Client);
	m_requiredComponents.push_back(req);

	m_systemManager->GetMessageHandler()->Subscribe(EntityMessage::Move, this);
	m_systemManager->GetMessageHandler()->Subscribe(EntityMessage::Attack, this);
	m_playerUpdateTimer = sf::milliseconds(0);
}

S_Network::~S_Network(){}

void S_Network::Update(float l_dT){
	if (!m_client){ return; }
	sf::Lock lock(m_client->GetMutex());
	m_playerUpdateTimer += sf::seconds(l_dT);
	if (m_playerUpdateTimer.asMilliseconds() >= PLAYER_UPDATE_INTERVAL){
		SendPlayerOutgoing();
		m_playerUpdateTimer = sf::milliseconds(0);
	}
	PerformInterpolation();
}

void S_Network::HandleEvent(const EntityId& l_entity,
	const EntityEvent& l_event){}

void S_Network::Notify(const Message& l_message){
	if (!HasEntity(l_message.m_receiver) || l_message.m_receiver != m_player){ return; }
	if (l_message.m_type == (MessageType)EntityMessage::Attack && m_outgoing.find(EntityMessage::Attack) != m_outgoing.end()){ return; }
	m_outgoing[(EntityMessage)l_message.m_type].emplace_back(l_message);
}

void S_Network::SetClient(Client* l_client){ m_client = l_client; }
void S_Network::SetPlayerID(const EntityId& l_entity){ m_player = l_entity; }
void S_Network::AddSnapshot(const EntityId& l_entity, 
	const sf::Int32& l_timestamp, EntitySnapshot& l_snapshot)
{
	sf::Lock lock(m_client->GetMutex());
	auto i = m_entitySnapshots.emplace(l_timestamp, SnapshotDetails());
	i.first->second.m_snapshots.emplace(l_entity, l_snapshot);
}

void S_Network::ApplyEntitySnapshot(const EntityId& l_entity, 
	const EntitySnapshot& l_snapshot, bool l_applyPhysics)
{
	ClientEntityManager* entities = (ClientEntityManager*)m_systemManager->GetEntityManager();
	C_Position* position = nullptr;
	C_Movable* movable = nullptr;
	S_Movement* movement_s = nullptr;
	S_State* state_s = nullptr;
	C_Health* health = nullptr;
	C_Name* name = nullptr;
	sf::Lock lock(m_client->GetMutex());
	if (position = entities->GetComponent<C_Position>(l_entity, Component::Position)){
		position->SetPosition(l_snapshot.m_position);
		position->SetElevation(l_snapshot.m_elevation);
	}
	if (l_applyPhysics){
		if (movable = entities->GetComponent<C_Movable>(l_entity, Component::Movable)){
			movable->SetVelocity(l_snapshot.m_velocity);
			movable->SetAcceleration(l_snapshot.m_acceleration);
		}
	}
	if (movement_s = m_systemManager->GetSystem<S_Movement>(System::Movement)){
		movement_s->SetDirection(l_entity, (Direction)l_snapshot.m_direction);
	}
	if (state_s = m_systemManager->GetSystem<S_State>(System::State)){
		state_s->ChangeState(l_entity, (EntityState)l_snapshot.m_state, true);
	}
	if (health = entities->GetComponent<C_Health>(l_entity, Component::Health)){
		health->SetHealth(l_snapshot.m_health);
	}
	if (name = entities->GetComponent<C_Name>(l_entity, Component::Name)){
		name->SetName(l_snapshot.m_name);
	}
}

void S_Network::SendPlayerOutgoing(){
	sf::Int32 p_x = 0, p_y = 0;
	sf::Int8 p_a = 0;

	for (auto &itr : m_outgoing){
		if (itr.first == EntityMessage::Move){
			sf::Int32 x = 0, y = 0;
			for (auto &message : itr.second){
				if (message.m_int == (int)Direction::Up){ --y; }
				else if (message.m_int == (int)Direction::Down){ ++y; }
				else if (message.m_int == (int)Direction::Left){ --x; }
				else if (message.m_int == (int)Direction::Right){ ++x; }
			}
			if (!x && !y){ continue; }
			p_x = x; p_y = y;
		} else if (itr.first == EntityMessage::Attack){ p_a = 1; }
	}

	sf::Packet packet;
	StampPacket(PacketType::PlayerUpdate, packet);
	packet << sf::Int8(EntityMessage::Move) << p_x << p_y << sf::Int8(Network::PlayerUpdateDelim);
	packet << sf::Int8(EntityMessage::Attack) << p_a << sf::Int8(Network::PlayerUpdateDelim);
	m_client->Send(packet);
	m_outgoing.clear();
}

void S_Network::ClearSnapshots(){ m_entitySnapshots.clear(); }

void S_Network::PerformInterpolation(){
	if (m_entitySnapshots.empty()){ return; }
	ClientEntityManager* entities = (ClientEntityManager*)m_systemManager->GetEntityManager();
	sf::Time t = m_client->GetTime();
	auto itr = ++m_entitySnapshots.begin();
	while (itr != m_entitySnapshots.end()){
		if (m_entitySnapshots.begin()->first <= t.asMilliseconds() - NET_RENDER_DELAY
			&& itr->first >= t.asMilliseconds() - NET_RENDER_DELAY)
		{
			auto Snapshot1 = m_entitySnapshots.begin();
			auto Snapshot2 = itr;
			bool SortDrawables = false;
			for (auto snap = Snapshot1->second.m_snapshots.begin();
				snap != Snapshot1->second.m_snapshots.end();)
			{
				if (!entities->HasEntity(snap->first)){
					if (entities->AddEntity(snap->second.m_type, snap->first) == (int)Network::NullID){
						std::cout << "Failed adding entity type: " << snap->second.m_type << std::endl;
						continue;
					}
					ApplyEntitySnapshot(snap->first, snap->second, true);
					++snap;
					continue;
				}
				auto snap2 = Snapshot2->second.m_snapshots.find(snap->first);
				if (snap2 == Snapshot2->second.m_snapshots.end()){
					// Entity that exists in first snapshot wasn't found in second.
					// Remove it, as it possibly de-spawned.
					sf::Lock lock(m_client->GetMutex());
					entities->RemoveEntity(snap->first);
					snap = Snapshot1->second.m_snapshots.erase(snap);
					continue;
				}

				EntitySnapshot i_snapshot;
				InterpolateSnapshot(snap->second, Snapshot1->first,
					snap2->second, Snapshot2->first,
					i_snapshot, t.asMilliseconds() - NET_RENDER_DELAY);
				ApplyEntitySnapshot(snap->first, i_snapshot, true);
				if (!CompareSnapshots(snap->second, snap2->second, true, false, false)){
					SortDrawables = true;
				}
				++snap;
			}
			if (SortDrawables){ m_systemManager->GetSystem<S_Renderer>(System::Renderer)->SortDrawables(); }
			return;
		}
		m_entitySnapshots.erase(m_entitySnapshots.begin());
		itr = ++m_entitySnapshots.begin();
	}
}