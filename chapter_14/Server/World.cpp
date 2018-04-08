#include "World.h"

World::World(): m_server(&World::HandlePacket, this),
	m_commandThread(&World::CommandLine, this), m_entities(nullptr),
	m_map(&m_entities), m_tick(0), m_tps(0), m_running(false)
{
	if (!m_server.Start()){ return; }
	m_running = true;
	m_systems.SetEntityManager(&m_entities);
	m_entities.SetSystemManager(&m_systems);
	m_map.LoadMap("media/Maps/map1.map");
	m_systems.GetSystem<S_Collision>(System::Collision)->SetMap(&m_map);
	m_systems.GetSystem<S_Movement>(System::Movement)->SetMap(&m_map);
	m_systems.GetSystem<S_Network>(System::Network)->RegisterServer(&m_server);
	m_server.BindTimeoutHandler(&World::ClientLeave, this);
	m_commandThread.launch();
}

World::~World(){ m_entities.SetSystemManager(nullptr); }

void World::Update(const sf::Time& l_time){
	if (!m_server.IsRunning()){ m_running = false; return; }
	m_serverTime += l_time;
	m_snapshotTimer += l_time;
	m_tpsTime += l_time;
	m_server.Update(l_time);
	m_server.GetMutex().lock();
	m_systems.Update(l_time.asSeconds());
	m_server.GetMutex().unlock();
	if (m_snapshotTimer.asMilliseconds() >= SNAPSHOT_INTERVAL){
		sf::Packet snapshot;
		m_systems.GetSystem<S_Network>(System::Network)->CreateSnapshot(snapshot);
		m_server.Broadcast(snapshot);
		m_snapshotTimer = sf::milliseconds(0);
	}
	if (m_tpsTime >= sf::milliseconds(1000)){
		m_tps = m_tick;
		m_tick = 0;
		m_tpsTime = sf::milliseconds(0);
	} else {
		++m_tick;
	}
}

void World::HandlePacket(sf::IpAddress& l_ip, const PortNumber& l_port,
	const PacketID& l_id, sf::Packet& l_packet, Server* l_server)
{
	ClientID id = l_server->GetClientID(l_ip, l_port);
	PacketType type = (PacketType)l_id;
	if (id >= 0){
		if (type == PacketType::Disconnect){
			ClientLeave(id);
			l_server->RemoveClient(l_ip, l_port);
		} else if (type == PacketType::Message){
			// ...
		} else if (type == PacketType::PlayerUpdate){
			m_systems.GetSystem<S_Network>(System::Network)->UpdatePlayer(l_packet, id);
		}
	} else {
		if (type != PacketType::Connect){ return; }
		std::string nickname;
		if (!(l_packet >> nickname)){ return; }
		ClientID cid = l_server->AddClient(l_ip, l_port);
		if (cid == -1){
			sf::Packet packet;
			StampPacket(PacketType::Disconnect, packet);
			l_server->Send(l_ip, l_port, packet);
			return;
		}
		sf::Lock lock(m_server.GetMutex());
		sf::Int32 eid = m_entities.AddEntity("Player");
		if (eid == -1){ return; }
		m_systems.GetSystem<S_Network>(System::Network)->RegisterClientID(eid, cid);
		C_Position* pos = m_entities.GetComponent<C_Position>(eid, Component::Position);
		pos->SetPosition(64.f, 64.f);
		m_entities.GetComponent<C_Name>(eid, Component::Name)->SetName(nickname);
		sf::Packet packet;
		StampPacket(PacketType::Connect, packet);
		packet << eid;
		packet << pos->GetPosition().x << pos->GetPosition().y;
		if (!l_server->Send(cid, packet)){
			std::cout << "Unable to respond to connect packet!" << std::endl;
			return;
		}
	}
}

void World::ClientLeave(const ClientID& l_client){
	sf::Lock lock(m_server.GetMutex());
	S_Network* network = m_systems.GetSystem<S_Network>(System::Network);
	m_entities.RemoveEntity(network->GetEntityID(l_client));
}

void World::CommandLine(){
	while (m_server.IsRunning()){
		std::string str;
		std::getline(std::cin, str);
		if (str == "terminate"){
			m_server.Stop();
			m_running = false;
			break;
		} else if (str == "disconnectall"){
			std::cout << "Disconnecting all clients..." << std::endl;
			m_server.DisconnectAll();
			sf::Lock lock(m_server.GetMutex());
			m_entities.Purge();
		} else if (str.find("tps") != std::string::npos){
			std::cout << "TPS: " << m_tps << std::endl;
		} else if (str.find("health") != std::string::npos){
			std::stringstream ss(str);
			std::string command;
			std::string eid;
			std::string health;
			if (!(ss >> command)){ continue; }
			if (!(ss >> eid)){ continue; }
			if (!(ss >> health)){ continue; }
			EntityId id = std::stoi(eid);
			Health healthValue = std::stoi(health);
			C_Health* h = m_entities.GetComponent<C_Health>(id, Component::Health);
			if (!h){ continue; }
			h->SetHealth(healthValue);
		} else if (str == "clients"){
			std::cout << m_server.GetClientCount() << " clients online:" << std::endl;
			std::cout << m_server.GetClientList() << std::endl;
		} else if (str == "entities"){
			std::cout << "Current entity count: " << m_entities.GetEntityCount() << std::endl;
		}
	}
}

bool World::IsRunning(){ return m_running; }