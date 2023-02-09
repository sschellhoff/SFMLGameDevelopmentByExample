#include "State_Game.h"
#include "StateManager.h"

State_Game::State_Game(StateManager* l_stateManager)
	: BaseState(l_stateManager){}

State_Game::~State_Game(){}

void State_Game::OnCreate(){
	EventManager* evMgr = m_stateMgr->
		GetContext()->m_eventManager;

	//evMgr->AddCallback(StateType::Game, "Key_Escape", &State_Game::MainMenu, this);
	evMgr->AddCallback(StateType::Game, "Key_Escape", &State_Game::Pause, this);
	evMgr->AddCallback(StateType::Game, "Key_O", &State_Game::ToggleOverlay, this);

	sf::Vector2u size = m_stateMgr->GetContext()->m_wind->GetWindowSize();
	m_view.setSize(size.x,size.y);
	m_view.setCenter(size.x/2,size.y/2);
	m_view.zoom(0.6f);
	m_stateMgr->GetContext()->m_wind->GetRenderWindow()->setView(m_view);

	m_gameMap = new Map(m_stateMgr->GetContext(), this);

	int level = m_stateMgr->GetContext()->m_mapNumber;

	std::cout<<"Will load map "<<level<<std::endl;

	// i know this is not ideal 
	if(level==1)
		m_gameMap->LoadMap("media/Maps/map1.map");
	else if(level==2)
		m_gameMap->LoadMap("media/Maps/map2.map");

	
	music.openFromFile("Resources/MusicTracks/" + m_gameMap->GetMusicName());
	music.setVolume(50.0f);
	music.setLoop(true);
	
	
}

void State_Game::OnDestroy(){
	EventManager* evMgr = m_stateMgr->
		GetContext()->m_eventManager;
	evMgr->RemoveCallback(StateType::Game, "Key_Escape");
	//evMgr->RemoveCallback(StateType::Game, "Key_P");
	evMgr->RemoveCallback(StateType::Game, "Key_O");
	
	delete m_gameMap;
	m_gameMap = nullptr;
}

void State_Game::Update(const sf::Time& l_time){
	SharedContext* context = m_stateMgr->GetContext();
	EntityBase* player = context->m_entityManager->Find("Player");
	if(!player){
		std::cout << "Respawning player..." << std::endl;
		context->m_entityManager->Add(EntityType::Player,"Player");
		player = context->m_entityManager->Find("Player");

		// set currenthp
		context->m_characterCurrentHealth = ((Character*) player)->GetHitpoints();

		std::cout<< "SC health:" <<context->m_characterCurrentHealth<<std::endl;

		std::cout<<m_gameMap->GetPlayerStart().x << " " << m_gameMap->GetPlayerStart().y << std::endl;

		player->SetPosition(m_gameMap->GetPlayerStart());
	} else {
		m_view.setCenter(player->GetPosition());
		context->m_wind->GetRenderWindow()->setView(m_view);
	}

	sf::FloatRect viewSpace = context->m_wind->GetViewSpace();
	if(viewSpace.left <= 0){
		m_view.setCenter(viewSpace.width / 2,m_view.getCenter().y);
		context->m_wind->GetRenderWindow()->setView(m_view);
	} else if (viewSpace.left + viewSpace.width > (m_gameMap->GetMapSize().x + 1) * Sheet::Tile_Size){
		m_view.setCenter(((m_gameMap->GetMapSize().x + 1) * Sheet::Tile_Size) - (viewSpace.width / 2), m_view.getCenter().y);
		context->m_wind->GetRenderWindow()->setView(m_view);
	}

	m_gameMap->Update(l_time.asSeconds());
	m_stateMgr->GetContext()->m_entityManager->Update(l_time.asSeconds());
}

void State_Game::Draw(){
	m_gameMap->Draw();
	m_stateMgr->GetContext()->m_entityManager->Draw();
}

// void State_Game::MainMenu(EventDetails* l_details){
// 	m_stateMgr->SwitchTo(StateType::MainMenu);
// }

void State_Game::Pause(EventDetails* l_details){
	m_stateMgr->SwitchTo(StateType::Paused);
}

void State_Game::Activate(){
	music.play();
}
void State_Game::Deactivate(){
	music.pause();
}
// Test/debug methods.
void State_Game::ToggleOverlay(EventDetails* l_details){
	m_stateMgr->GetContext()->m_debugOverlay.SetDebug(!m_stateMgr->GetContext()->m_debugOverlay.Debug());
}