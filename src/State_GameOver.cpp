#include "State_GameOver.h"
#include "StateManager.h"

State_GameOver::State_GameOver(StateManager* l_stateManager)
	: BaseState(l_stateManager){}

State_GameOver::~State_GameOver(){}

void State_GameOver::OnCreate(){

	m_bufferFailSound.loadFromFile(Utils::GetResourceDirectory() + "SoundEffects/failfare.wav");
	m_FailSound.setBuffer(m_bufferFailSound);
	m_FailSound.setVolume(50.0f);

	sf::Vector2u windowSize = m_stateMgr->GetContext()
		->m_wind->GetRenderWindow()->getSize();

	// Set up background.
	m_rect.setSize(sf::Vector2f(windowSize));
	m_rect.setPosition(0,0);
	m_rect.setFillColor(sf::Color(20,9,23,255));

	// Set up Logo sprite. (will be another image)
	TextureManager* textureMgr = m_stateMgr->GetContext()->m_textureManager;
	textureMgr->RequireResource("GameOver");
	m_logo.setTexture(*textureMgr->GetResource("GameOver"));
	m_logo.setOrigin(textureMgr->GetResource("GameOver")->getSize().x / 2.0f,
					textureMgr->GetResource("GameOver")->getSize().y / 2.0f);

	m_logo.setScale(0.7f, 0.7f);
	m_logo.setPosition(windowSize.x / 2.0f, windowSize.y / 3.0f);

	// Set up title.
	m_font.loadFromFile(Utils::GetResourceDirectory() + "media/Fonts/FORCED_SQUARE.ttf");
	m_text.setFont(m_font);
	m_text.setString(sf::String("You Died!"));
	m_text.setCharacterSize(30);

	sf::FloatRect textRect = m_text.getLocalBounds();
	m_text.setOrigin(textRect.left + textRect.width / 2.0f,
		textRect.top + textRect.height / 2.0f);

	m_text.setPosition(windowSize.x / 2.0f, windowSize.y / 1.44f);

	// Set up buttons.
	m_buttonSize = sf::Vector2f(300.0f,32.0f);
	m_buttonPos = sf::Vector2f(windowSize.x / 2.0f, windowSize.y / 1.3f);
	m_buttonPadding = 4; // 4px.

	std::string str[3];
	str[0] = "Restart";
	str[1] = "Go map selection";
	str[2] = "Go to main menu";

	for(int i = 0; i < 3; ++i){
		sf::Vector2f buttonPosition(
			m_buttonPos.x,m_buttonPos.y + 
			(i * (m_buttonSize.y + m_buttonPadding)));
		m_rects[i].setSize(m_buttonSize);
		m_rects[i].setFillColor(sf::Color(58,28,63,255));

		m_rects[i].setOrigin(
			m_buttonSize.x / 2.0f, m_buttonSize.y / 2.0f);
		m_rects[i].setPosition(buttonPosition);

		m_labels[i].setFont(m_font);
		m_labels[i].setString(sf::String(str[i]));
		m_labels[i].setCharacterSize(20);

		sf::FloatRect rect = m_labels[i].getLocalBounds();
		m_labels[i].setOrigin(
			rect.left + rect.width / 2.0f,
			rect.top + rect.height / 2.0f);

		m_labels[i].setPosition(buttonPosition);
	}

	EventManager* evMgr = m_stateMgr->
		GetContext()->m_eventManager;
	evMgr->AddCallback(StateType::GameOver, "Mouse_Left", &State_GameOver::MouseClick, this);
}


void State_GameOver::MouseClick(EventDetails* l_details){
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
				m_stateMgr->SwitchTo(StateType::ChooseMap);
			} else if(i == 2){
				m_stateMgr->SwitchTo(StateType::MainMenu);
			}
		}
	}
}

void State_GameOver::Activate(){
	m_stateMgr->Remove(StateType::Game); 	// Destroy 'Game' state in case the user chooses 'restart'
	m_FailSound.play(); 					// Play a sound effect.
}

void State_GameOver::Draw(){
	sf::RenderWindow* window = m_stateMgr->
		GetContext()->m_wind->GetRenderWindow();

	window->draw(m_rect);
	window->draw(m_text);
	window->draw(m_logo);
	for(int i = 0; i < 3; ++i){
		window->draw(m_rects[i]);
		window->draw(m_labels[i]);
	}
}

void State_GameOver::OnDestroy(){
	EventManager* evMgr = m_stateMgr->
		GetContext()->m_eventManager;
	evMgr->RemoveCallback(StateType::MainMenu, "Mouse_Left");

	m_stateMgr->GetContext()->m_textureManager->ReleaseResource("GameOver");
}

void State_GameOver::Deactivate(){}

void State_GameOver::Update(const sf::Time& l_time){}
