#pragma once
#include "Window.h"
#include "EventManager.h"
#include "StateManager.h"
#include "TextureManager.h"
#include <iostream>

class Game{
public:
	Game();
	~Game();

	void Update();
	void Render();
	void LateUpdate();

	sf::Time GetElapsed();

	Window* GetWindow();
private:
	void RestartClock();
	sf::Clock m_clock;
	sf::Time m_elapsed;
	SharedContext m_context;
	Window m_window;
	Client m_client;
	TextureManager m_textureManager;
	FontManager m_fontManager;
	AudioManager m_audioManager;
	SoundManager m_soundManager;
	ClientSystemManager m_systemManager;
	ClientEntityManager m_entityManager;
	GUI_Manager m_guiManager;
	StateManager m_stateManager;
	DebugOverlay m_debugOverlay;
};