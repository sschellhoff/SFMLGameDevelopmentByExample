#include "State_Intro.h"
#include "StateManager.h"

State_Intro::State_Intro(StateManager* l_stateManager)
	: BaseState(l_stateManager){}

State_Intro::~State_Intro(){}

void State_Intro::OnCreate(){
	sf::Vector2u windowSize = m_stateMgr->GetContext()
		->m_wind->GetRenderWindow()->getSize();

	TextureManager* textureMgr = m_stateMgr->GetContext()->m_textureManager;
	textureMgr->RequireResource("Intro");
	m_introSprite.setTexture(*textureMgr->GetResource("Intro"));
	m_introSprite.setOrigin(textureMgr->GetResource("Intro")->getSize().x / 2.0f,
							textureMgr->GetResource("Intro")->getSize().y / 2.0f);

	m_introSprite.setPosition(windowSize.x / 2.0f, windowSize.y / 2.0f);

	m_text.setString(sf::String("Press SPACE to continue"));
	m_text.setCharacterSize(15);
	sf::FloatRect textRect = m_text.getLocalBounds();
	m_text.setOrigin(textRect.left + textRect.width / 2.0f,
		textRect.top + textRect.height / 2.0f);
	m_text.setPosition(m_introSprite.getPosition().x, 
		m_introSprite.getPosition().y + textureMgr->GetResource("Intro")->getSize().y / 1.5f);

	EventManager* evMgr = m_stateMgr->GetContext()->m_eventManager;
	evMgr->AddCallback(StateType::Intro, "Intro_Continue", &State_Intro::Continue,this);
	m_stateMgr->GetContext()->m_soundManager->PlayMusic("Electrix", 100.f, true);
}

void State_Intro::OnDestroy(){
	TextureManager* textureMgr = m_stateMgr->GetContext()->m_textureManager;
	textureMgr->ReleaseResource("Intro");

	EventManager* evMgr = m_stateMgr->GetContext()->m_eventManager;
	evMgr->RemoveCallback(StateType::Intro, "Intro_Continue");
}

void State_Intro::Draw(){
	sf::RenderWindow* window = m_stateMgr->GetContext()->m_wind->GetRenderWindow();

	window->draw(m_introSprite);
	window->draw(m_text);
}

void State_Intro::Continue(EventDetails* l_details){
	m_stateMgr->SwitchTo(StateType::MainMenu);
	m_stateMgr->Remove(StateType::Intro);
}

void State_Intro::Update(const sf::Time& l_time){}
void State_Intro::Activate(){}
void State_Intro::Deactivate(){}