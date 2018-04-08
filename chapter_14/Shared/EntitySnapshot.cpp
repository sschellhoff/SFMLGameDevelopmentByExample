#include "EntitySnapshot.h"

sf::Packet& operator <<(sf::Packet& l_packet, const EntitySnapshot& l_snapshot){
	return l_packet << l_snapshot.m_type << l_snapshot.m_name << l_snapshot.m_position.x << l_snapshot.m_position.y << l_snapshot.m_elevation
		<< l_snapshot.m_velocity.x << l_snapshot.m_velocity.y << l_snapshot.m_acceleration.x
		<< l_snapshot.m_acceleration.y << l_snapshot.m_direction << l_snapshot.m_state << l_snapshot.m_health;
}

sf::Packet& operator >>(sf::Packet& l_packet, EntitySnapshot& l_snapshot){
	return l_packet >> l_snapshot.m_type >> l_snapshot.m_name >> l_snapshot.m_position.x >> l_snapshot.m_position.y >> l_snapshot.m_elevation
		>> l_snapshot.m_velocity.x >> l_snapshot.m_velocity.y >> l_snapshot.m_acceleration.x
		>> l_snapshot.m_acceleration.y >> l_snapshot.m_direction >> l_snapshot.m_state >> l_snapshot.m_health;
}