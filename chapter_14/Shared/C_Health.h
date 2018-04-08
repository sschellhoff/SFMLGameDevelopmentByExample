#pragma once
#include "C_TimedComponentBase.h"

using Health = unsigned int;

class C_Health : public C_TimedComponentBase{
public:
	C_Health(): C_TimedComponentBase(Component::Health), 
		m_hurtDuration(0), m_deathDuration(0){}
	void ReadIn(std::stringstream& l_stream){
		l_stream >> m_maxHealth >> m_hurtDuration >> m_deathDuration;
		m_health = m_maxHealth;
	}
	Health GetHealth()const{ return m_health; }
	Health GetMaxHealth()const{ return m_maxHealth; }
	void SetHealth(const Health& l_health){ m_health = l_health; }
	void ResetHealth(){ m_health = m_maxHealth; }

	sf::Uint32 GetHurtDuration(){ return m_hurtDuration; }
	sf::Uint32 GetDeathDuration(){ return m_deathDuration; }
private:
	Health m_health;
	Health m_maxHealth;
	sf::Uint32 m_hurtDuration;
	sf::Uint32 m_deathDuration;
};