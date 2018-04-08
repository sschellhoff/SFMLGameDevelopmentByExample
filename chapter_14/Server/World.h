#pragma once
#include "Server.h"
#include "Server_Entity_Manager.h"
#include "Server_System_Manager.h"
#include "Map.h"
#include "NetSettings.h"

class World{
public:
	World();
	~World();

	void Update(const sf::Time& l_time);
	void HandlePacket(sf::IpAddress& l_ip, const PortNumber& l_port, 
		const PacketID& l_id, sf::Packet& l_packet, Server* l_server);
	void ClientLeave(const ClientID& l_client);
	void CommandLine();

	bool IsRunning();
private:
	sf::Time m_tpsTime;
	sf::Time m_serverTime;
	sf::Time m_snapshotTimer;
	sf::Thread m_commandThread;
	Server m_server;
	ServerSystemManager m_systems;
	ServerEntityManager m_entities;
	bool m_running;

	Map m_map;
	unsigned int m_tick;
	unsigned int m_tps;
};