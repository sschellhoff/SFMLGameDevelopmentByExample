#include "Client.h"

Client::Client():m_listenThread(&Client::Listen, this){}
Client::~Client(){ m_socket.unbind(); }

bool Client::Connect(){
	if (m_connected){ return false; }
	m_socket.bind(sf::Socket::AnyPort);
	std::cout << "Bound client to port: " << m_socket.getLocalPort() << std::endl;
	sf::Packet p;
	StampPacket(PacketType::Connect, p);
	p << m_playerName;
	if (m_socket.send(p, m_serverIp, m_serverPort) != sf::Socket::Done){ m_socket.unbind(); return false; }
	m_socket.setBlocking(false);
	p.clear();
	sf::IpAddress recvIP;
	PortNumber recvPORT;
	sf::Clock timer;
	timer.restart();

	std::cout << "Attempting to connect to: " << m_serverIp << ":" << m_serverPort << std::endl;
	while (timer.getElapsedTime().asMilliseconds() < CONNECT_TIMEOUT){
		sf::Socket::Status s = m_socket.receive(p, recvIP, recvPORT);
		if (s != sf::Socket::Done){ continue; }
		if (recvIP != m_serverIp){ continue; }
		PacketID id;
		if (!(p >> id)){ break; }
		if (id != PacketID(PacketType::Connect)){ continue; }
		m_packetHandler(id, p, this);
		m_connected = true;
		m_socket.setBlocking(true);
		m_lastHeartbeat = m_serverTime;
		m_listenThread.launch();
		return true;
	}
	std::cout << "Connection attempt failed! Server info: " << m_serverIp << ":" << m_serverPort << std::endl;
	m_socket.unbind();
	m_socket.setBlocking(true);
	return false;
}

bool Client::Disconnect(){
	if (!m_connected){ return false; }
	sf::Packet p;
	StampPacket(PacketType::Disconnect, p);
	sf::Socket::Status s = m_socket.send(p, m_serverIp, m_serverPort);
	m_connected = false;
	m_socket.unbind(); // Unbind to close the listening thread.
	if (s != sf::Socket::Done){ return false; }
	return true;
}

void Client::Listen(){
	sf::Packet packet;
	sf::IpAddress recvIP;
	PortNumber recvPORT;
	std::cout << "Beginning to listen..." << std::endl;
	while (m_connected){
		packet.clear();
		sf::Socket::Status status = m_socket.receive(packet, recvIP, recvPORT);
		if (status != sf::Socket::Done){
			if (m_connected){
				std::cout << "Failed receiving a packet from "
					<< recvIP << ":" << recvPORT << ". Status: " << status << std::endl;
				continue;
			} else {
				std::cout << "Socket unbound." << std::endl;
				break;
			}
		}
		if (recvIP != m_serverIp){
			// Ignore packets not sent from the server.
			std::cout << "Invalid packet received: invalid origin." << std::endl;
			continue;
		}
		PacketID id;
		if (!(packet >> id)){
			// Non-conventional packet.
			std::cout << "Invalid packet received: unable to extract id." << std::endl;
			continue;
		}
		PacketType type = (PacketType)id;
		if (type < PacketType::Disconnect || type >= PacketType::OutOfBounds){
			// Invalid packet type.
			std::cout << "Invalid packet received: id is out of bounds." << std::endl;
			continue;
		}

		if (type == PacketType::Heartbeat){
			sf::Packet p;
			StampPacket(PacketType::Heartbeat, p);
			if (m_socket.send(p, m_serverIp, m_serverPort) != sf::Socket::Done){
				std::cout << "Failed sending a heartbeat!" << std::endl;
			}
			sf::Int32 timestamp;
			packet >> timestamp;
			SetTime(sf::milliseconds(timestamp));
			m_lastHeartbeat = m_serverTime;
		} else if(m_packetHandler){
			m_packetHandler(id, packet, this); // Handle the packet.
		}
	}
	std::cout << "...Listening stopped." << std::endl;
}

bool Client::Send(sf::Packet& l_packet){
	if (!m_connected){ return false; }
	if (m_socket.send(l_packet, m_serverIp, m_serverPort) != sf::Socket::Done){ return false; }
	return true;
}

const sf::Time& Client::GetTime()const{
	return m_serverTime;
}

const sf::Time& Client::GetLastHeartbeat()const{
	return m_lastHeartbeat;
}

void Client::SetTime(const sf::Time& l_time){
	m_serverTime = l_time;
}

void Client::SetServerInformation(const sf::IpAddress& l_ip, const PortNumber& l_port){
	m_serverIp = l_ip;
	m_serverPort = l_port;
}

void Client::Update(const sf::Time& l_time){
	if (!m_connected){ return; }
	m_serverTime += l_time;
	if (m_serverTime.asMilliseconds() < 0){
		m_serverTime -= sf::milliseconds((sf::Int32)Network::HighestTimestamp);
		m_lastHeartbeat = m_serverTime;
		return;
	}
	if (m_serverTime.asMilliseconds() - m_lastHeartbeat.asMilliseconds() >= (sf::Int32)Network::ClientTimeout){
		// Timeout.
		std::cout << "Server connection timed out!" << std::endl;
		Disconnect();
	}
}

bool Client::IsConnected()const{ return m_connected; }
void Client::SetPlayerName(const std::string& l_name){ m_playerName = l_name; }

sf::Mutex& Client::GetMutex(){ return m_mutex; }

void Client::Setup(void(*l_handler)(const PacketID&, sf::Packet&, Client*)){
	m_packetHandler = std::bind(l_handler,
		std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
}

void Client::UnregisterPacketHandler(){
	m_packetHandler = nullptr;
}