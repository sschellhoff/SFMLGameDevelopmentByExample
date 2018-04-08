#pragma once
#include "C_TimedComponentBase.h"
#include <SFML/Graphics/Rect.hpp>

class C_Attacker : public C_TimedComponentBase{
public:
	C_Attacker(): C_TimedComponentBase(Component::Attacker), 
		m_attacked(false), m_knockback(0.f), m_attackDuration(0){}
	void ReadIn(std::stringstream& l_stream){
		l_stream >> m_offset.x >> m_offset.y 
			>> m_attackArea.width >> m_attackArea.height 
			>> m_knockback >> m_attackDuration;
	}

	void SetAreaPosition(const sf::Vector2f& l_pos){
		m_attackArea.left = l_pos.x;
		m_attackArea.top = l_pos.y;
	}

	const sf::FloatRect& GetAreaOfAttack(){ return m_attackArea; }
	const sf::Vector2f& GetOffset(){ return m_offset; }
	bool HasAttacked(){ return m_attacked; }
	void SetAttacked(bool l_attacked){ m_attacked = l_attacked; }
	float GetKnockback(){ return m_knockback; }
	sf::Uint32 GetAttackDuration(){ return m_attackDuration; }
private:
	sf::FloatRect m_attackArea;
	sf::Vector2f m_offset;
	bool m_attacked;
	float m_knockback;
	sf::Uint32 m_attackDuration;
};