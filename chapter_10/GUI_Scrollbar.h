#pragma once
#include "GUI_Element.h"

enum class SliderType{ Horizontal, Vertical };

class GUI_Scrollbar : public GUI_Element{
public:
	GUI_Scrollbar(const std::string& l_name, GUI_Interface* l_owner);
	~GUI_Scrollbar();

	void SetPosition(const sf::Vector2f& l_pos);
	
	void ReadIn(std::stringstream& l_stream);
	void OnClick(const sf::Vector2f& l_mousePos);
	void OnRelease();
	void OnHover(const sf::Vector2f& l_mousePos);
	void OnLeave();

	void ApplyStyle();
	void UpdateStyle(const GUI_ElementState& l_state, const GUI_Style& l_style);

	void Update(float l_dT);
	void Draw(sf::RenderTarget* l_target);
private:
	SliderType m_sliderType;
	sf::RectangleShape m_slider;
	sf::Vector2f m_moveMouseLast;
	int m_percentage;
};