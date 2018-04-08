#include "State_Game.h"
#include "StateManager.h"

State_Game::State_Game(StateManager* l_stateManager)
	: BaseState(l_stateManager){}

State_Game::~State_Game(){}

void State_Game::OnCreate(){
	EventManager* evMgr = m_stateMgr->GetContext()->m_eventManager;

	evMgr->AddCallback(StateType::Game, "Key_Escape", &State_Game::MainMenu, this);
	evMgr->AddCallback(StateType::Game, "Key_O", &State_Game::ToggleOverlay, this);
	evMgr->AddCallback(StateType::Game, "Player_MoveLeft", &State_Game::PlayerMove, this);
	evMgr->AddCallback(StateType::Game, "Player_MoveRight", &State_Game::PlayerMove, this);
	evMgr->AddCallback(StateType::Game, "Player_MoveUp", &State_Game::PlayerMove, this);
	evMgr->AddCallback(StateType::Game, "Player_MoveDown", &State_Game::PlayerMove, this);

	sf::Vector2u size = m_stateMgr->GetContext()->m_wind->GetWindowSize();
	m_view.setSize(size.x, size.y);
	m_view.setCenter(size.x / 2, size.y / 2);
	m_view.zoom(0.6f);
	m_stateMgr->GetContext()->m_wind->GetRenderWindow()->setView(m_view);

	m_gameMap = new Map(m_stateMgr->GetContext());
	m_gameMap->LoadMap("media/Maps/map1.map");

	EntityManager* entities = m_stateMgr->GetContext()->m_entityManager;
	m_stateMgr->GetContext()->m_systemManager->GetSystem<S_Collision>(System::Collision)->SetMap(m_gameMap);
	m_stateMgr->GetContext()->m_systemManager->GetSystem<S_Movement>(System::Movement)->SetMap(m_gameMap);
	m_player = m_gameMap->GetPlayerId();

	m_stateMgr->GetContext()->m_soundManager->PlayMusic("TownTheme", 50.f, true);
}

void State_Game::OnDestroy(){
	m_stateMgr->GetContext()->m_guiManager->RemoveInterface(StateType::Game, "Test");
	EventManager* evMgr = m_stateMgr->GetContext()->m_eventManager;
	evMgr->RemoveCallback(StateType::Game, "Key_Escape");
	evMgr->RemoveCallback(StateType::Game, "Key_O");
	evMgr->RemoveCallback(StateType::Game, "Player_MoveLeft");
	evMgr->RemoveCallback(StateType::Game, "Player_MoveRight");
	evMgr->RemoveCallback(StateType::Game, "Player_MoveUp");
	evMgr->RemoveCallback(StateType::Game, "Player_MoveDown");
	
	delete m_gameMap;
}

void State_Game::Update(const sf::Time& l_time){
	SharedContext* context = m_stateMgr->GetContext();
	UpdateCamera();
	m_gameMap->Update(l_time.asSeconds());
	context->m_systemManager->Update(l_time.asSeconds());
}

void State_Game::UpdateCamera(){
	if (m_player == -1){ return; }
	SharedContext* context = m_stateMgr->GetContext();
	C_Position* pos = m_stateMgr->GetContext()->m_entityManager->
		GetComponent<C_Position>(m_player, Component::Position);

	m_view.setCenter(pos->GetPosition());
	context->m_wind->GetRenderWindow()->setView(m_view);

	sf::FloatRect viewSpace = context->m_wind->GetViewSpace();
	if (viewSpace.left <= 0){
		m_view.setCenter(viewSpace.width / 2, m_view.getCenter().y);
		context->m_wind->GetRenderWindow()->setView(m_view);
	} else if (viewSpace.left + viewSpace.width > (m_gameMap->GetMapSize().x) * Sheet::Tile_Size){
		m_view.setCenter(((m_gameMap->GetMapSize().x) * Sheet::Tile_Size) - (viewSpace.width / 2), m_view.getCenter().y);
		context->m_wind->GetRenderWindow()->setView(m_view);
	}

	if (viewSpace.top <= 0){
		m_view.setCenter(m_view.getCenter().x, viewSpace.height / 2);
		context->m_wind->GetRenderWindow()->setView(m_view);
	} else if (viewSpace.top + viewSpace.height > (m_gameMap->GetMapSize().y) * Sheet::Tile_Size){
		m_view.setCenter(m_view.getCenter().x, ((m_gameMap->GetMapSize().y) * Sheet::Tile_Size) - (viewSpace.height / 2));
		context->m_wind->GetRenderWindow()->setView(m_view);
	}
}

void State_Game::Draw(){
	for (unsigned int i = 0; i < Sheet::Num_Layers; ++i){
		m_gameMap->Draw(i);
		m_stateMgr->GetContext()->m_systemManager->Draw(
			m_stateMgr->GetContext()->m_wind, i);
	}
}

void State_Game::MainMenu(EventDetails* l_details){
	m_stateMgr->SwitchTo(StateType::MainMenu);
}

void State_Game::Pause(EventDetails* l_details){
	m_stateMgr->SwitchTo(StateType::Paused);
}

void State_Game::Activate(){}
void State_Game::Deactivate(){}

void State_Game::PlayerMove(EventDetails* l_details){
	Message msg((MessageType)EntityMessage::Move);
	if (l_details->m_name == "Player_MoveLeft"){
		msg.m_int = (int)Direction::Left;
	} else if (l_details->m_name == "Player_MoveRight"){
		msg.m_int = (int)Direction::Right;
	} else if (l_details->m_name == "Player_MoveUp"){
		msg.m_int = (int)Direction::Up;
	} else if (l_details->m_name == "Player_MoveDown"){
		msg.m_int = (int)Direction::Down;
	}
	msg.m_receiver = m_player;
	m_stateMgr->GetContext()->m_systemManager->GetMessageHandler()->Dispatch(msg);
}

void State_Game::ToggleOverlay(EventDetails* l_details){
	m_stateMgr->GetContext()->m_debugOverlay.SetDebug(!m_stateMgr->GetContext()->m_debugOverlay.Debug());
}