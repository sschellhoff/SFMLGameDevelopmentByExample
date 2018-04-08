#include <iostream>
#include <SFML/Network.hpp>
#include "Server.h"

void Handler(sf::IpAddress& l_ip, const PortNumber& l_port, const PacketID& l_id, sf::Packet& l_packet, Server* l_server){
	ClientID id = l_server->GetClientID(l_ip, l_port);
	if (id >= 0){
		if ((PacketType)l_id == PacketType::Disconnect){
			l_server->RemoveClient(l_ip, l_port);
			sf::Packet p;
			StampPacket(PacketType::Message, p);
			std::string message;
			message = "Client left! " + l_ip.toString() + ":" + std::to_string(l_port);
			p << message;
			l_server->Broadcast(p, id);
		} else if ((PacketType)l_id == PacketType::Message){
			std::string receivedMessage;
			l_packet >> receivedMessage;
			std::string message = l_ip.toString() + ":" + std::to_string(l_port) + " :" + receivedMessage;
			sf::Packet p;
			StampPacket(PacketType::Message, p);
			p << message;
			l_server->Broadcast(p, id);
		}
	} else {
		if ((PacketType)l_id == PacketType::Connect){
			ClientID id = l_server->AddClient(l_ip, l_port);
			sf::Packet packet;
			StampPacket(PacketType::Connect, packet);
			l_server->Send(id, packet);
		}
	}
}

void CommandProcess(Server* l_server){
	while (l_server->IsRunning()){
		std::string str;
		std::getline(std::cin, str);
		if (str == "!quit"){
			l_server->Stop();
			break;
		} else if (str == "dc"){
			l_server->DisconnectAll();
			std::cout << "DC..." << std::endl;
		} else if (str == "list"){
			std::cout << l_server->GetClientCount() << " clients online:" << std::endl;
			std::cout << l_server->GetClientList() << std::endl;
		}
	}
}

int main(){
	Server server(Handler);

	if (server.Start()){
		sf::Thread c(&CommandProcess, &server);
		c.launch();

		sf::Clock clock;
		clock.restart();
		while (server.IsRunning()){
			server.Update(clock.restart());
		}
		std::cout << "Stopping server..." << std::endl;
	}

	system("PAUSE");
	return 0;
}