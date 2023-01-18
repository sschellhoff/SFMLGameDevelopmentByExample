#include "State_Paused.h"
#include "StateManager.h"

State_Paused::State_Paused(StateManager* l_stateManager)
	: BaseState(l_stateManager){}

State_Paused::~State_Paused(){}

void State_Paused::OnCreate(){
	SetTransparent(true); // Set our transparency flag.
	m_font.loadFromFile(Utils::GetResourceDirectory() + "media/Fonts/arial.ttf");
	m_text.setFont(m_font);
	m_text.setString(sf::String("PAUSE MENU:"));
	m_text.setCharacterSize(18);

	sf::Vector2u windowSize = m_stateMgr->
		GetContext()->m_wind->GetRenderWindow()->getSize();

	m_rect.setSize(sf::Vector2f(windowSize));
	m_rect.setPosition(0,0);
	m_rect.setFillColor(sf::Color(0,0,0,150));

	sf::FloatRect textRect = m_text.getLocalBounds();
	m_text.setOrigin(textRect.left + textRect.width / 2.0f,
		textRect.top + textRect.height / 2.0f);

	m_text.setPosition(400,100);

	m_buttonSize = sf::Vector2f(300.0f,32.0f);
	m_buttonPos = sf::Vector2f(400,200);
	m_buttonPadding = 4; // 4px.

	std::string str[3];
	str[0] = "Resume";
	str[1] = "Help";
	str[2] = "Exit to MainMenu";

	for(int i = 0; i < 3; ++i){
		sf::Vector2f buttonPosition(
			m_buttonPos.x,m_buttonPos.y + 
			(i * (m_buttonSize.y + m_buttonPadding)));
		m_rects[i].setSize(m_buttonSize);
		m_rects[i].setFillColor(sf::Color::Red);

		m_rects[i].setOrigin(
			m_buttonSize.x / 2.0f, m_buttonSize.y / 2.0f);
		m_rects[i].setPosition(buttonPosition);

		m_labels[i].setFont(m_font);
		m_labels[i].setString(sf::String(str[i]));
		m_labels[i].setCharacterSize(12);

		sf::FloatRect rect = m_labels[i].getLocalBounds();
		m_labels[i].setOrigin(
			rect.left + rect.width / 2.0f,
			rect.top + rect.height / 2.0f);

		m_labels[i].setPosition(buttonPosition);
	}

	EventManager* evMgr = m_stateMgr->
		GetContext()->m_eventManager;
	evMgr->AddCallback(StateType::Paused,"Key_P", &State_Paused::Unpause,this);
	evMgr->AddCallback(StateType::Paused, "Mouse_Left", &State_Paused::MouseClick,this);
}

void State_Paused::OnDestroy(){
	EventManager* evMgr = m_stateMgr->
		GetContext()->m_eventManager;
	evMgr->RemoveCallback(StateType::Paused, "Key_P");
}

void State_Paused::MouseClick(EventDetails* l_details){
	SharedContext* context = m_stateMgr->GetContext();
	sf::Vector2i mousePos = l_details->m_mouse;

	float halfX = m_buttonSize.x / 2.0f;
	float halfY = m_buttonSize.y / 2.0f;
	for(int i = 0; i < 3; ++i){
		if(mousePos.x>=m_rects[i].getPosition().x - halfX &&
			mousePos.x<=m_rects[i].getPosition().x + halfX &&
			mousePos.y>=m_rects[i].getPosition().y - halfY &&
			mousePos.y<=m_rects[i].getPosition().y + halfY)
		{
			if(i == 0){
				m_stateMgr->SwitchTo(StateType::Game);
			} else if(i == 1){
				
			} else if(i == 2){
				//m_stateMgr->GetContext()->m_wind->Close();
				//std::cout<<"option 3"<<std::endl;
				m_stateMgr->SwitchTo(StateType::MainMenu);
			}
		}
	}
}

void State_Paused::Draw(){
	sf::RenderWindow* window = m_stateMgr->
		GetContext()->m_wind->GetRenderWindow();
	window->draw(m_text);
	window->draw(m_rect);
	for(int i = 0; i < 3; ++i){
		window->draw(m_rects[i]);
		window->draw(m_labels[i]);
	}
}

void State_Paused::Unpause(EventDetails* l_details){
	m_stateMgr->SwitchTo(StateType::Game);
}

void State_Paused::Activate(){}
void State_Paused::Deactivate(){}
void State_Paused::Update(const sf::Time& l_time){}