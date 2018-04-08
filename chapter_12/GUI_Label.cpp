#include "GUI_Label.h"
#include <algorithm>
#include <iostream>
#include "Utilities.h"

GUI_Label::GUI_Label(const std::string& l_name, GUI_Interface* l_owner)
	: GUI_Element(l_name, GUI_ElementType::Label, l_owner){}
GUI_Label::~GUI_Label(){}

void GUI_Label::ReadIn(std::stringstream& l_stream){
	std::string content;
	Utils::ReadQuotedString(l_stream, content);
	m_visual.m_text.setString(content);
}

void GUI_Label::OnClick(const sf::Vector2f& l_mousePos){
	SetState(GUI_ElementState::Clicked);
}
void GUI_Label::OnRelease(){
	SetState(GUI_ElementState::Neutral);
}
void GUI_Label::OnHover(const sf::Vector2f& l_mousePos){
	SetState(GUI_ElementState::Focused);
}
void GUI_Label::OnLeave(){
	SetState(GUI_ElementState::Neutral);
}
void GUI_Label::Update(float l_dT){}
void GUI_Label::Draw(sf::RenderTarget* l_target){
	l_target->draw(m_visual.m_backgroundSolid);
	if (m_style[m_state].m_glyph != ""){ l_target->draw(m_visual.m_glyph); }
	l_target->draw(m_visual.m_text);
}