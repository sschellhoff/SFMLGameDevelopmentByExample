#include "GUI_Scrollbar.h"
#include "GUI_Interface.h"
#include "GUI_Manager.h"
#include "SharedContext.h"

GUI_Scrollbar::GUI_Scrollbar(const std::string& l_name, GUI_Interface* l_owner)
	: GUI_Element(l_name, GUI_ElementType::Scrollbar, l_owner)
{
	m_isControl = true;
}
GUI_Scrollbar::~GUI_Scrollbar(){}

void GUI_Scrollbar::SetPosition(const sf::Vector2f& l_pos){
	GUI_Element::SetPosition(l_pos);
	if (m_sliderType == SliderType::Horizontal){ m_position.x = 0; }
	else { m_position.y = 0; }
}
void GUI_Scrollbar::ReadIn(std::stringstream& l_stream){
	std::string type;
	l_stream >> type;
	if (type == "Horizontal"){ m_sliderType = SliderType::Horizontal; }
	else { m_sliderType = SliderType::Vertical; }

	if (m_sliderType == SliderType::Horizontal){ m_slider.setPosition(0, GetPosition().y); }
	else { m_slider.setPosition(GetPosition().x, 0); }
}
void GUI_Scrollbar::OnClick(const sf::Vector2f& l_mousePos){
	if (!m_slider.getGlobalBounds().contains(l_mousePos - m_owner->GetPosition())){ return; }
	SetState(GUI_ElementState::Clicked);
	m_moveMouseLast = l_mousePos;
}
void GUI_Scrollbar::OnRelease(){
	SetState(GUI_ElementState::Neutral);
}
void GUI_Scrollbar::OnHover(const sf::Vector2f& l_mousePos){
	SetState(GUI_ElementState::Focused);
}
void GUI_Scrollbar::OnLeave(){
	SetState(GUI_ElementState::Neutral);
}
void GUI_Scrollbar::UpdateStyle(const GUI_ElementState& l_state,
	const GUI_Style& l_style)
{
	GUI_Element::UpdateStyle(l_state, l_style);
	if (m_sliderType == SliderType::Horizontal){ m_style[l_state].m_size.x = m_owner->GetSize().x; }
	else { m_style[l_state].m_size.y = m_owner->GetSize().y; }
}

void GUI_Scrollbar::ApplyStyle(){
	GUI_Element::ApplyStyle();
	m_slider.setFillColor(m_style[m_state].m_elementColor);
	bool horizontal = m_sliderType == SliderType::Horizontal;
	auto& bgSolid = m_visual.m_backgroundSolid;
	SetPosition((horizontal ?
		sf::Vector2f(0, m_owner->GetSize().y - bgSolid.getSize().y) :
		sf::Vector2f(m_owner->GetSize().x - bgSolid.getSize().x, 0)));
	bgSolid.setSize((horizontal ?
		sf::Vector2f(m_owner->GetSize().x, m_style[m_state].m_size.y) :
		sf::Vector2f(m_style[m_state].m_size.x, m_owner->GetSize().y)));
	m_slider.setPosition(
		(horizontal ? m_slider.getPosition().x : GetPosition().x),
		(horizontal ? GetPosition().y : m_slider.getPosition().y));
	float SizeFactor = (horizontal ?
		m_owner->GetContentSize().x / m_owner->GetSize().x :
		m_owner->GetContentSize().y / m_owner->GetSize().y);
	if (SizeFactor < 1.f){ SizeFactor = 1.f; }
	float SliderSize = (horizontal ? m_owner->GetSize().x : m_owner->GetSize().y) / SizeFactor;
	m_slider.setSize((horizontal ?
		sf::Vector2f(SliderSize, bgSolid.getSize().y) :
		sf::Vector2f(bgSolid.getSize().x, SliderSize)));
	bgSolid.setPosition(GetPosition());
}

void GUI_Scrollbar::Update(float l_dT){
	// Mouse-drag code.
	if (GetState() != GUI_ElementState::Clicked){ return; }
	SharedContext* context = m_owner->GetManager()->GetContext();
	sf::Vector2f mousePos = sf::Vector2f(context->m_eventManager->GetMousePos(
		context->m_wind->GetRenderWindow()));
	if (m_moveMouseLast == mousePos){ return; }
	sf::Vector2f difference = mousePos - m_moveMouseLast;
	m_moveMouseLast = mousePos;

	bool horizontal = m_sliderType == SliderType::Horizontal;
	m_slider.move((horizontal ? difference.x : 0), (horizontal ? 0 : difference.y));
	if (horizontal && m_slider.getPosition().x < 0){
		m_slider.setPosition(0, m_slider.getPosition().y);
	} else if (m_slider.getPosition().y < 0){
		m_slider.setPosition(m_slider.getPosition().x, 0);
	}
	if (horizontal && (m_slider.getPosition().x + m_slider.getSize().x > m_owner->GetSize().x)){
		m_slider.setPosition(m_owner->GetSize().x - m_slider.getSize().x, m_slider.getPosition().y);
	} else if (m_slider.getPosition().y + m_slider.getSize().y > m_owner->GetSize().y){
		m_slider.setPosition(m_slider.getPosition().x, m_owner->GetSize().y - m_slider.getSize().y);
	}
	float WorkArea = (horizontal ? m_owner->GetSize().x - m_slider.getSize().x
		: m_owner->GetSize().y - m_slider.getSize().y);
	int percentage = ((horizontal ?
		m_slider.getPosition().x : m_slider.getPosition().y) / WorkArea) * 100;
	if (horizontal){ m_owner->UpdateScrollHorizontal(percentage); }
	else { m_owner->UpdateScrollVertical(percentage); }
	SetRedraw(true);
}
void GUI_Scrollbar::Draw(sf::RenderTarget* l_target){
	l_target->draw(m_visual.m_backgroundSolid);
	l_target->draw(m_slider);
}