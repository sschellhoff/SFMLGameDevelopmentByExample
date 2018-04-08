#include "Textbox.h"

Textbox::Textbox(){
	Setup(5,9,200,sf::Vector2f(0,0));
}

Textbox::Textbox(int l_visible, int l_charSize, 
int l_width, sf::Vector2f l_screenPos){
	Setup(l_visible, l_charSize, l_width, l_screenPos);
}

Textbox::~Textbox(){ Clear(); }

void Textbox::Setup(int l_visible, int l_charSize, 
int l_width, sf::Vector2f l_screenPos){
	m_numVisible = l_visible;

	sf::Vector2f l_offset(2.0f, 2.0f);
	
	m_font.loadFromFile("arial.ttf");
	m_content.setFont(m_font);
	m_content.setString("");
	m_content.setCharacterSize(l_charSize);
	m_content.setFillColor(sf::Color::White);
	m_content.setPosition(l_screenPos + l_offset);

	m_backdrop.setSize(sf::Vector2f(
		l_width, (l_visible * (l_charSize * 1.2f))));
	m_backdrop.setFillColor(sf::Color(90,90,90,90));
	m_backdrop.setPosition(l_screenPos);
}

void Textbox::Add(std::string l_message){
	m_messages.push_back(l_message);
	if (m_messages.size() < 6){ return; }
	m_messages.erase(m_messages.begin());
}

void Textbox::Clear(){ m_messages.clear(); }

void Textbox::Render(sf::RenderWindow& l_wind){
	std::string l_content;

	for(auto &itr : m_messages){
		l_content.append(itr+"\n");
	}

	if(l_content != ""){
		m_content.setString(l_content);
		l_wind.draw(m_backdrop);
		l_wind.draw(m_content);
	}
}