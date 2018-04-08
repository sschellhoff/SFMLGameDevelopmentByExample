#pragma once
#include <SFML/Network/Packet.hpp>
#include <SFML/System/Vector2.hpp>

struct EntitySnapshot{
	std::string m_type;
	sf::Vector2f m_position;
	sf::Int32 m_elevation;
	sf::Vector2f m_velocity;
	sf::Vector2f m_acceleration;
	sf::Uint8 m_direction;
	sf::Uint8 m_state;
	sf::Uint8 m_health;
	std::string m_name;
};

sf::Packet& operator <<(sf::Packet& l_packet, const EntitySnapshot& l_snapshot);
sf::Packet& operator >>(sf::Packet& l_packet, EntitySnapshot& l_snapshot);