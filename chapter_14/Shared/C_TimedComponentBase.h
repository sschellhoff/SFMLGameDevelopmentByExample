#pragma once
#include "C_Base.h"
#include <SFML/System/Time.hpp>

class C_TimedComponentBase : public C_Base{
public:
	C_TimedComponentBase(const Component& l_type)
		: C_Base(l_type), m_duration(sf::milliseconds(0)){}
	virtual ~C_TimedComponentBase(){}

	const sf::Time& GetTimer()const{ return m_duration; }
	void SetTimer(const sf::Time& l_time){ m_duration = l_time; }
	void AddToTimer(const sf::Time& l_time){ m_duration += l_time; }

	void Reset(){ m_duration = sf::milliseconds(0); }
protected:
	sf::Time m_duration;
};