#include "State_YesNoMenu.h"
#include "StateManager.h"

State_YesNoMenu::State_YesNoMenu(StateManager* l_stateManager)
	: BaseState(l_stateManager){}

State_YesNoMenu::~State_YesNoMenu(){}

void State_YesNoMenu::OnCreate(){

	sf::Vector2u windowSize = m_stateMgr->GetContext()
		->m_wind->GetRenderWindow()->getSize();

	// Set up background.
	m_rect.setSize(sf::Vector2f(windowSize));
	m_rect.setPosition(0,0);
	m_rect.setFillColor(sf::Color(20,9,23,255));

	// Set up title.
	m_font.loadFromFile(Utils::GetResourceDirectory() + "media/Fonts/FORCED_SQUARE.ttf");
	m_text.setFont(m_font);
	m_text.setString(sf::String("Are you sure?"));
	m_text.setCharacterSize(30);

	sf::FloatRect textRect = m_text.getLocalBounds();
	m_text.setOrigin(textRect.left + textRect.width / 2.0f,
		textRect.top + textRect.height / 2.0f);

	m_text.setPosition(windowSize.x / 2.0f, windowSize.y / 2.3f);

	// Set up buttons.
	m_buttonSize = sf::Vector2f(300.0f,32.0f);
	m_buttonPos = sf::Vector2f(windowSize.x / 2.0f, windowSize.y / 2.0f);
	m_buttonPadding = 4; // 4px.

	std::string str[2];
	str[0] = "Yes";
	str[1] = "No";

	for(int i = 0; i < 2; ++i){
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
	evMgr->AddCallback(StateType::YesNoMenu, "Mouse_Left", &State_YesNoMenu::MouseClick, this);
}

void State_YesNoMenu::OnDestroy(){
	EventManager* evMgr = m_stateMgr->
		GetContext()->m_eventManager;
	evMgr->RemoveCallback(StateType::YesNoMenu, "Mouse_Left");
}

void State_YesNoMenu::Activate(){}

void State_YesNoMenu::MouseClick(EventDetails* l_details){
	SharedContext* context = m_stateMgr->GetContext();
	sf::Vector2i mousePos = l_details->m_mouse;

	float halfX = m_buttonSize.x / 2.0f;
	float halfY = m_buttonSize.y / 2.0f;
	for(int i = 0; i < 2; ++i){
		if(mousePos.x>=m_rects[i].getPosition().x - halfX &&
			mousePos.x<=m_rects[i].getPosition().x + halfX &&
			mousePos.y>=m_rects[i].getPosition().y - halfY &&
			mousePos.y<=m_rects[i].getPosition().y + halfY)
		{
			if(i == 0){
                m_stateMgr->SwitchTo(StateType::MainMenu);
			} else if(i == 1){
				m_stateMgr->SwitchTo(StateType::Paused);
			}
		}
	}
}

void State_YesNoMenu::Draw(){
	sf::RenderWindow* window = m_stateMgr->
		GetContext()->m_wind->GetRenderWindow();
	window->draw(m_rect);
	window->draw(m_text);
	for(int i = 0; i < 2; ++i){
		window->draw(m_rects[i]);
		window->draw(m_labels[i]);
	}
}

void State_YesNoMenu::Update(const sf::Time& l_time){}
void State_YesNoMenu::Deactivate(){}