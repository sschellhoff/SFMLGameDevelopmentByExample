#include "S_Network.h"
#include "System_Manager.h"

S_Network::S_Network(SystemManager* l_systemMgr)
	: S_Base(System::Network, l_systemMgr)
{
	Bitmask req;
	req.TurnOnBit((unsigned int)Component::Client);
	m_requiredComponents.push_back(req);

	MessageHandler* messageHandler = 
		m_systemManager->GetMessageHandler();
	messageHandler->Subscribe(EntityMessage::Removed_Entity, this);
	messageHandler->Subscribe(EntityMessage::Hurt, this);
	messageHandler->Subscribe(EntityMessage::Respawn, this);
}

S_Network::~S_Network(){}

void S_Network::Update(float l_dT){
	EntityManager* entities = m_systemManager->GetEntityManager();
	for (auto &entity : m_entities){
		auto& player = m_playerInput[entity];
		if (player.m_movedX || player.m_movedY){
			if (player.m_movedX){
				Message msg((MessageType)EntityMessage::Move);
				msg.m_receiver = entity;
				if (player.m_movedX > 0){ msg.m_int = (int)Direction::Right; }
				else { msg.m_int = (int)Direction::Left; }
				m_systemManager->GetMessageHandler()->Dispatch(msg);
			}

			if (player.m_movedY){
				Message msg((MessageType)EntityMessage::Move);
				msg.m_receiver = entity;
				if (player.m_movedY > 0){ msg.m_int = (int)Direction::Down; }
				else { msg.m_int = (int)Direction::Up; }
				m_systemManager->GetMessageHandler()->Dispatch(msg);
			}
		}
		if (player.m_attacking){
			Message msg((MessageType)EntityMessage::Attack);
			msg.m_receiver = entity;
			m_systemManager->GetMessageHandler()->Dispatch(msg);
		}
	}
}

void S_Network::HandleEvent(const EntityId& l_entity, const EntityEvent& l_event){}

void S_Network::Notify(const Message& l_message){
	if (!HasEntity(l_message.m_receiver)){ return; }
	EntityMessage m = EntityMessage(l_message.m_type);
	if (m == EntityMessage::Removed_Entity){ m_playerInput.erase(l_message.m_receiver); return; }
	if (m == EntityMessage::Hurt){
		sf::Packet packet;
		StampPacket(PacketType::Hurt, packet);
		packet << l_message.m_receiver;
		m_server->Broadcast(packet);
		return;
	}
	if (m == EntityMessage::Respawn){
		C_Position* position = m_systemManager->GetEntityManager()->GetComponent<C_Position>(l_message.m_receiver, Component::Position);
		if (!position){ return; }
		position->SetPosition(64.f, 64.f);
		position->SetElevation(1);
	}
}

bool S_Network::RegisterClientID(const EntityId& l_entity, const ClientID& l_client){
	if (!HasEntity(l_entity)){ return false; }
	m_systemManager->GetEntityManager()->GetComponent<C_Client>
		(l_entity, Component::Client)->SetClientID(l_client);
	return true;
}

void S_Network::RegisterServer(Server* l_server){ m_server = l_server; }

ClientID S_Network::GetClientID(const EntityId& l_entity){
	if (!HasEntity(l_entity)){ return (ClientID)Network::NullID; }
	return m_systemManager->GetEntityManager()->GetComponent<C_Client>(l_entity, Component::Client)->GetClientID();
}

EntityId S_Network::GetEntityID(const ClientID& l_client){
	EntityManager* e = m_systemManager->GetEntityManager();
	auto entity = std::find_if(m_entities.begin(), m_entities.end(),
		[&e, &l_client](EntityId& id){
			return e->GetComponent<C_Client>(id, Component::Client)->GetClientID() == l_client;
	});
	return(entity != m_entities.end() ? *entity : (EntityId)Network::NullID);
}

void S_Network::CreateSnapshot(sf::Packet& l_packet){
	sf::Lock lock(m_server->GetMutex());
	ServerEntityManager* e = (ServerEntityManager*)m_systemManager->GetEntityManager();
	StampPacket(PacketType::Snapshot, l_packet);
	l_packet << sf::Int32(e->GetEntityCount());
	if (e->GetEntityCount()){
		e->DumpEntityInfo(l_packet);
	}
}

void S_Network::UpdatePlayer(sf::Packet& l_packet, const ClientID& l_cid){
	sf::Lock lock(m_server->GetMutex());
	EntityId eid = GetEntityID(l_cid);
	if (eid == -1){ return; }
	if (!HasEntity(eid)){ return; }
	sf::Int8 entity_message;
	m_playerInput[eid].m_attacking = false;
	while (l_packet >> entity_message){
		switch (entity_message){
		case sf::Int8(EntityMessage::Move):
		{
			sf::Int32 x = 0, y = 0;
			l_packet >> x >> y;
			m_playerInput[eid].m_movedX = x;
			m_playerInput[eid].m_movedY = y;
			break;
		}
		case sf::Int8(EntityMessage::Attack):
		{
			sf::Int8 attackState;
			l_packet >> attackState;
			if (attackState){ m_playerInput[eid].m_attacking = true; }
			break;
		}
		}
		sf::Int8 delim = 0;
		if (!(l_packet >> delim) || delim != (sf::Int8)Network::PlayerUpdateDelim){
			std::cout << "Faulty update!" << std::endl; 
			break;
		}
	}
}