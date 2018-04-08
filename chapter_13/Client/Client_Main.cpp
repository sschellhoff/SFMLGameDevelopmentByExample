#include <iostream>
#include <SFML/Network.hpp>
#include "Client.h"

void HandlePacket(const PacketID& l_id, sf::Packet& l_packet, Client* l_client){
	if ((PacketType)l_id == PacketType::Message){
		std::string message;
		l_packet >> message;
		std::cout << message << std::endl;
	} else if ((PacketType)l_id == PacketType::Disconnect){
		l_client->Disconnect();
	}
}

void CommandProcess(Client* l_client){
	while (l_client->IsConnected()){
		std::string str;
		std::getline(std::cin, str);
		if (str != ""){
			if (str == "!quit"){
				l_client->Disconnect();
				break;
			}
			sf::Packet p;
			StampPacket(PacketType::Message, p);
			p << str;
			l_client->Send(p);
		}
	}
}

void main(int argc, char** argv){
	sf::IpAddress ip;
	PortNumber port;
	if (argc == 1){
		std::cout << "Enter Server IP: ";
		std::cin >> ip;
		std::cout << "Enter Server Port: ";
		std::cin >> port;
	} else if (argc == 3){
		ip = argv[1];
		port = atoi(argv[2]);
	} else {
		return;
	}

	Client client;
	client.SetServerInformation(ip, port);
	client.Setup(&HandlePacket);
	sf::Thread c(&CommandProcess, &client);
	if (client.Connect()){
		c.launch();
		sf::Clock clock;
		clock.restart();
		while (client.IsConnected()){
			client.Update(clock.restart());
		}
	} else {
		std::cout << "Failed to connect." << std::endl;
	}
	std::cout << "Quitting..." << std::endl;
	sf::sleep(sf::seconds(1.f));
}