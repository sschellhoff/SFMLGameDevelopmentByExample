#pragma once
#include <vector>
#include <SFML/Graphics.hpp>

class DebugOverlay{
public:
	DebugOverlay(){
		m_debug = false;
	}

	~DebugOverlay(){
		while(m_drawables.begin() != m_drawables.end()){
			delete m_drawables.back();
			m_drawables.pop_back();
		}
	}

	void Add(sf::Drawable* l_drawable){
		m_drawables.push_back(l_drawable);
	}

	void Draw(sf::RenderWindow* l_wind){
		while(m_drawables.begin() != m_drawables.end()){
			l_wind->draw(*m_drawables.back());
			delete m_drawables.back();
			m_drawables.pop_back();
		}
	}

	bool Debug(){ return m_debug; }
	void SetDebug(const bool& l_val){ m_debug = l_val; }
private:
	std::vector<sf::Drawable*> m_drawables;
	bool m_debug;
};