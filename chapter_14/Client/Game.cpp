#include "Game.h"
Game::Game() : m_window("Chapter 14", sf::Vector2u(640, 480)),
	m_entityManager(&m_systemManager, &m_textureManager), m_stateManager(&m_context),
	m_systemManager(&m_textureManager, &m_fontManager), m_guiManager(m_window.GetEventManager(), &m_context),
	m_soundManager(&m_audioManager)
{
	m_clock.restart();
	srand(time(nullptr));

	m_systemManager.SetEntityManager(&m_entityManager);

	m_client.SetServerInformation("localhost", (PortNumber)Network::ServerPort);
	m_context.m_wind = &m_window;
	m_context.m_eventManager = m_window.GetEventManager();
	m_context.m_textureManager = &m_textureManager;
	m_context.m_fontManager = &m_fontManager;
	m_context.m_audioManager = &m_audioManager;
	m_context.m_soundManager = &m_soundManager;
	m_context.m_systemManager = &m_systemManager;
	m_context.m_entityManager = &m_entityManager;
	m_context.m_guiManager = &m_guiManager;
	m_context.m_client = &m_client;
	m_systemManager.GetSystem<S_Sound>(System::Sound)->SetUp(&m_audioManager, &m_soundManager);

	// Debug:
	m_systemManager.m_debugOverlay = &m_debugOverlay;
	m_fontManager.RequireResource("Main");
	m_stateManager.SwitchTo(StateType::Intro);
}

Game::~Game(){ 
	m_fontManager.ReleaseResource("Main");
}

sf::Time Game::GetElapsed(){ return m_clock.getElapsedTime(); }
void Game::RestartClock(){ m_elapsed = m_clock.restart(); }
Window* Game::GetWindow(){ return &m_window; }

void Game::Update(){
	m_window.Update();
	m_stateManager.Update(m_elapsed);
	m_client.Update(m_elapsed);
	m_guiManager.Update(m_elapsed.asSeconds());
	m_soundManager.Update(m_elapsed.asSeconds());

	GUI_Event guiEvent;
	while (m_guiManager.PollEvent(guiEvent)){
		m_window.GetEventManager()->HandleEvent(guiEvent);
	}
}

void Game::Render(){
	m_window.BeginDraw();
	// Render here.
	m_stateManager.Draw();

	sf::View CurrentView = m_window.GetRenderWindow()->getView();
	m_window.GetRenderWindow()->setView(m_window.GetRenderWindow()->getDefaultView());
	m_guiManager.Render(m_window.GetRenderWindow());
	m_window.GetRenderWindow()->setView(CurrentView);

	// Debug.
	if(m_context.m_debugOverlay.Debug()){
		m_context.m_debugOverlay.Draw(m_window.GetRenderWindow());
	}
	// End debug.

	m_window.EndDraw();
}

void Game::LateUpdate(){
	m_stateManager.ProcessRequests();
	RestartClock();
}