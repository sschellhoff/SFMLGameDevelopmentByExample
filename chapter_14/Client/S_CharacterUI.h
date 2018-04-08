#pragma once
#include "S_Base.h"
#include "Window.h"
#include "C_UI_Element.h"
#include "C_Health.h"
#include "C_Name.h"
#include "Client_System_Manager.h"

class S_CharacterUI : public S_Base{
public:
	S_CharacterUI(SystemManager* l_systemMgr);
	~S_CharacterUI();

	void Update(float l_dT);
	void HandleEvent(const EntityId& l_entity, const EntityEvent& l_event);
	void Notify(const Message& l_message);

	void Render(Window* l_wind);
private:
	sf::Sprite m_heartBar;
	sf::Text m_nickname;
	sf::RectangleShape m_nickbg;
	sf::Vector2u m_heartBarSize;
};