#include "Server.h"

Server::Server(void(*l_handler)(sf::IpAddress&, const PortNumber&, const PacketID&, sf::Packet&, Server*))
	: m_listenThread(&Server::Listen, this), m_running(false)
{
	// Bind a packet handler function.
	m_packetHandler = std::bind(l_handler, 
		std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
		std::placeholders::_4, std::placeholders::_5);
}

Server::~Server(){ Stop(); }

void Server::BindTimeoutHandler(void(*l_handler)(const ClientID&)){
	m_timeoutHandler = std::bind(l_handler, std::placeholders::_1);
}

bool Server::Send(const ClientID& l_id, sf::Packet& l_packet){
	sf::Lock lock(m_mutex);
	auto itr = m_clients.find(l_id);
	if (itr == m_clients.end()){ return false; }
	if (m_outgoing.send(l_packet, itr->second.m_clientIP, itr->second.m_clientPORT) 
		!= sf::Socket::Done)
	{
		std::cout << "Error sending a packet..." << std::endl;
		return false;
	}
	m_totalSent += l_packet.getDataSize();
	return true;
}

bool Server::Send(sf::IpAddress& l_ip, const PortNumber& l_port, sf::Packet& l_packet){
	if (m_outgoing.send(l_packet, l_ip, l_port) != sf::Socket::Done){ return false; }
	m_totalSent += l_packet.getDataSize();
	return true;
}

void Server::Broadcast(sf::Packet& l_packet, const ClientID& l_ignore){
	sf::Lock lock(m_mutex);
	for (auto &client : m_clients){
		if (client.first == l_ignore){ continue; }
		if (m_outgoing.send(l_packet, client.second.m_clientIP, client.second.m_clientPORT)
			!= sf::Socket::Done)
		{
			std::cout << "Error broadcasting a packet to client: " 
				<< client.first << std::endl;
			continue;
		}
		m_totalSent += l_packet.getDataSize();
	}
}

void Server::Listen(){
	sf::IpAddress ip;
	PortNumber port;
	sf::Packet packet;
	std::cout << "Beginning to listen..." << std::endl;
	while (m_running){
		packet.clear();
		sf::Socket::Status status = m_incoming.receive(packet, ip, port);
		if (status != sf::Socket::Done){
			if (m_running){
				std::cout << "Error receiving a packet from: "
					<< ip << ":" << port << ". Code: " << status << std::endl;
				continue;
			} else {
				std::cout << "Socket unbound." << std::endl;
				break;
			}
		}

		m_totalReceived += packet.getDataSize();

		PacketID id;
		if (!(packet >> id)){
			std::cout << "Invalid packet received: unable to extract id." << std::endl;
			continue;
		} // Non-conventional packet.
		PacketType type = (PacketType)id;
		if (id < (PacketID)PacketType::Disconnect || id >= (PacketID)PacketType::OutOfBounds){
			std::cout << "Invalid packet received: id is out of bounds." << std::endl;
			continue;
		} // Invalid packet type.

		if (type == PacketType::Heartbeat){
			bool ClientFound = false;
			sf::Lock lock(m_mutex);
			for (auto &client : m_clients){
				auto& info = client.second;
				if (info.m_clientIP != ip || info.m_clientPORT != port){ continue; }
				ClientFound = true;
				if (!info.m_heartbeatWaiting){ std::cout << "Invalid heartbeat packet received!" << std::endl; break; }
				info.m_latency = m_serverTime.asMilliseconds() - info.m_heartbeatSent.asMilliseconds();
				info.m_lastHeartbeat = m_serverTime;
				info.m_heartbeatWaiting = false;
				info.m_heartbeatRetry = 0;
				break;
			}
			if (!ClientFound){ std::cout << "Heartbeat from unknown client received..." << std::endl; }
		} else if (m_packetHandler){
			m_packetHandler(ip, port, id, packet, this); // Handle the packet.
		}
	}
	std::cout << "...Listening stopped." << std::endl;
}

void Server::Update(const sf::Time& l_time){
	m_serverTime += l_time;
	if (m_serverTime.asMilliseconds() < 0){ 
		m_serverTime -= sf::milliseconds((sf::Int32)Network::HighestTimestamp);
		sf::Lock lock(m_mutex);
		for (auto &client : m_clients){
			client.second.m_lastHeartbeat = 
				sf::milliseconds(std::abs(client.second.m_lastHeartbeat.asMilliseconds() - (sf::Int32)Network::HighestTimestamp));
		}
	}

	sf::Lock lock(m_mutex);
	for (auto itr = m_clients.begin(); itr != m_clients.end();){
		sf::Int32 elapsed = m_serverTime.asMilliseconds() - itr->second.m_lastHeartbeat.asMilliseconds();
		if (elapsed >= HEARTBEAT_INTERVAL){
			if (elapsed >= (sf::Int32)Network::ClientTimeout || itr->second.m_heartbeatRetry > HEARTBEAT_RETRIES){
				// Remove client.
				std::cout << "Client " << itr->first << " has timed out." << std::endl;
				if (m_timeoutHandler){ m_timeoutHandler(itr->first); }
				itr = m_clients.erase(itr);
				continue;
			}
			if (!itr->second.m_heartbeatWaiting || (elapsed >= HEARTBEAT_INTERVAL * (itr->second.m_heartbeatRetry + 1))){
				// Heartbeat
				if (itr->second.m_heartbeatRetry >= 3){
					std::cout << "Re-try(" << itr->second.m_heartbeatRetry
						<< ") heartbeat for client " << itr->first << std::endl;
				}
				sf::Packet Heartbeat;
				StampPacket(PacketType::Heartbeat, Heartbeat);
				Heartbeat << m_serverTime.asMilliseconds();
				Send(itr->first, Heartbeat);
				if (itr->second.m_heartbeatRetry == 0){
					itr->second.m_heartbeatSent = m_serverTime;
				}
				itr->second.m_heartbeatWaiting = true;
				++itr->second.m_heartbeatRetry;

				m_totalSent += Heartbeat.getDataSize();
			}
		}
		++itr;
	}
}

ClientID Server::AddClient(const sf::IpAddress& l_ip, const PortNumber& l_port){
	sf::Lock lock(m_mutex);
	for (auto &itr : m_clients){
		if (itr.second.m_clientIP == l_ip && itr.second.m_clientPORT == l_port){
			return ClientID(Network::NullID);
		}
	}
	ClientID id = m_lastID;
	ClientInfo info(l_ip, l_port, m_serverTime);
	m_clients.emplace(id, info);
	++m_lastID;
	return id;
}

ClientID Server::GetClientID(const sf::IpAddress& l_ip, const PortNumber& l_port){
	sf::Lock lock(m_mutex);
	for (auto itr = m_clients.begin(); itr != m_clients.end(); ++itr){
		if (itr->second.m_clientIP == l_ip && itr->second.m_clientPORT == l_port){ return itr->first; }
	}
	return (ClientID)Network::NullID;
}

bool Server::HasClient(const ClientID& l_id){
	return (m_clients.find(l_id) != m_clients.end());
}

bool Server::HasClient(const sf::IpAddress& l_ip, const PortNumber& l_port){
	return(GetClientID(l_ip, l_port) >= 0);
}

bool Server::GetClientInfo(const ClientID& l_id, ClientInfo& l_info){
	sf::Lock lock(m_mutex);
	for (auto itr = m_clients.begin(); itr != m_clients.end(); ++itr){
		if (itr->first == l_id){
			l_info = itr->second;
			return true;
		}
	}
	return false;
}

bool Server::RemoveClient(const ClientID& l_id){
	sf::Lock lock(m_mutex);
	auto itr = m_clients.find(l_id);
	if (itr == m_clients.end()){ return false; }
	sf::Packet p;
	StampPacket(PacketType::Disconnect, p);
	Send(l_id, p);
	m_clients.erase(itr);
	return true;
}

bool Server::RemoveClient(const sf::IpAddress& l_ip, const PortNumber& l_port){
	sf::Lock lock(m_mutex);
	for (auto itr = m_clients.begin(); itr != m_clients.end(); ++itr)
	{
		if (itr->second.m_clientIP == l_ip && itr->second.m_clientPORT == l_port){
			sf::Packet p;
			StampPacket(PacketType::Disconnect, p);
			Send(itr->first, p);
			m_clients.erase(itr);
			return true;
		}
	}
	return false;
}

void Server::DisconnectAll(){
	if (!m_running){ return; }
	sf::Packet p;
	StampPacket(PacketType::Disconnect, p);
	Broadcast(p);
	sf::Lock lock(m_mutex);
	m_clients.clear();
}

bool Server::Start(){
	if (m_running){ return false; }
	if (m_incoming.bind((unsigned short)Network::ServerPort) != sf::Socket::Done){ return false; }
	m_outgoing.bind(sf::Socket::AnyPort);
	Setup();
	std::cout << "Incoming port: " <<
		m_incoming.getLocalPort() << ". Outgoing port: " 
		<< m_outgoing.getLocalPort() << std::endl;
	m_listenThread.launch();
	m_running = true;
	return true;
}

bool Server::Stop(){
	if (!m_running){ return false; }
	DisconnectAll();
	m_running = false;
	m_incoming.unbind();
	return true;
}

bool Server::IsRunning(){ return m_running; }

unsigned int Server::GetClientCount(){ return m_clients.size(); }
std::string Server::GetClientList(){
	std::string list;
	std::string delimiter = "--------------------------------------";
	list = delimiter;
	list += '\n';
	list += "ID";
	list += '\t';
	list += "Client IP:PORT";
	list += '\t'; list += '\t';
	list += "Ping";
	list += '\n';
	list += delimiter;
	list += '\n';
	for (auto &client : m_clients){
		list += std::to_string(client.first); 
		list += '\t';
		list += client.second.m_clientIP.toString() + ":" + std::to_string(client.second.m_clientPORT);
		list += '\t'; list += '\t';
		list += std::to_string(client.second.m_latency) + "ms.";
		list += '\n';
	}
	list += delimiter;
	list += '\n';
	list += "Total data sent: " + std::to_string(m_totalSent / 1000) + "kB. Total data received: " + std::to_string(m_totalReceived / 1000) + "kB";
	return list;
}

sf::Mutex& Server::GetMutex(){ return m_mutex; }

void Server::Setup(){
	m_lastID = 0;
	m_running = false;
	m_totalSent = 0;
	m_totalReceived = 0;
}