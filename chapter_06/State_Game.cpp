#include "State_Game.h"
#include "StateManager.h"

State_Game::State_Game(StateManager* l_stateManager)
	: BaseState(l_stateManager){}

State_Game::~State_Game(){}

void State_Game::OnCreate(){
	m_texture.loadFromFile("Mushroom.png");
	m_sprite.setTexture(m_texture);
	m_sprite.setPosition(0,0);
	m_increment = sf::Vector2f(400.0f,400.0f);

	EventManager* evMgr = m_stateMgr->GetContext()->m_eventManager;
	evMgr->AddCallback(StateType::Game,"Key_Escape",&State_Game::MainMenu,this);
	evMgr->AddCallback(StateType::Game,"Key_P",&State_Game::Pause,this);
}

void State_Game::OnDestroy(){
	EventManager* evMgr = m_stateMgr->GetContext()->m_eventManager;
	evMgr->RemoveCallback(StateType::Game,"Key_Escape");
	evMgr->RemoveCallback(StateType::Game,"Key_P");
}

void State_Game::Update(const sf::Time& l_time){
	sf::Vector2u l_windSize = m_stateMgr->GetContext()->m_wind->GetWindowSize();
	sf::Vector2u l_textSize = m_texture.getSize();

	if((m_sprite.getPosition().x > l_windSize.x - l_textSize.x && m_increment.x > 0) ||
		(m_sprite.getPosition().x < 0 && m_increment.x < 0))
	{
			m_increment.x = -m_increment.x;
	}

	if((m_sprite.getPosition().y > l_windSize.y - l_textSize.y && m_increment.y > 0) ||
		(m_sprite.getPosition().y < 0 && m_increment.y < 0))
	{
			m_increment.y = -m_increment.y;
	}

	m_sprite.setPosition(m_sprite.getPosition().x + (m_increment.x * l_time.asSeconds()), 
		m_sprite.getPosition().y + (m_increment.y * l_time.asSeconds()));
}

void State_Game::Draw(){
	m_stateMgr->GetContext()->m_wind->GetRenderWindow()->draw(m_sprite);
}

void State_Game::MainMenu(EventDetails* l_details){
	m_stateMgr->SwitchTo(StateType::MainMenu); 
}

void State_Game::Pause(EventDetails* l_details){
	m_stateMgr->SwitchTo(StateType::Paused); 
}

void State_Game::Activate(){}
void State_Game::Deactivate(){}