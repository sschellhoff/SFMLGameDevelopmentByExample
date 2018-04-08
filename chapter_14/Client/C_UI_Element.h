#pragma once
#include "C_Base.h"
#include <SFML/System/Vector2.hpp>

class C_UI_Element : public C_Base{
public:
	C_UI_Element() : C_Base(Component::UI_Element), m_showHealth(false), m_showName(false){}
	void ReadIn(std::stringstream& l_stream){
		l_stream >> m_offset.x >> m_offset.y;
	}

	const sf::Vector2f& GetOffset(){ return m_offset; }
	void SetOffset(const sf::Vector2f& l_offset){ m_offset = l_offset; }

	void SetShowHealth(bool l_show){ m_showHealth = l_show; }
	void SetShowName(bool l_show){ m_showName = l_show; }
	bool ShowHealth(){ return m_showHealth; }
	bool ShowName(){ return m_showName; }
private:
	sf::Vector2f m_offset;
	bool m_showHealth;
	bool m_showName;
};