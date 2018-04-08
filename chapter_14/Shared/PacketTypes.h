#pragma once
#include <SFML/Network/Packet.hpp>
using PacketID = sf::Int8;
enum class PacketType{
	Disconnect = -1, Connect, Heartbeat, Snapshot, PlayerUpdate, Message, Hurt, OutOfBounds
};

void StampPacket(const PacketType& l_type, sf::Packet& l_packet);