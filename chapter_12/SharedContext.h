#pragma once
#include "Window.h"
#include "EventManager.h"
#include "TextureManager.h"
#include "FontManager.h"
#include "AudioManager.h"
#include "SoundManager.h"
#include "GUI_Manager.h"
#include "System_Manager.h"
#include "Entity_Manager.h"
#include "DebugOverlay.h"

class Map;

struct SharedContext{
	SharedContext():
		m_wind(nullptr),
		m_eventManager(nullptr),
		m_textureManager(nullptr),
		m_fontManager(nullptr),
		m_audioManager(nullptr),
		m_soundManager(nullptr),
		m_systemManager(nullptr),
		m_entityManager(nullptr),
		m_gameMap(nullptr),
		m_guiManager(nullptr){}

	Window* m_wind;
	EventManager* m_eventManager;
	TextureManager* m_textureManager;
	FontManager* m_fontManager;
	AudioManager* m_audioManager;
	SoundManager* m_soundManager;
	SystemManager* m_systemManager;
	EntityManager* m_entityManager;
	Map* m_gameMap;
	GUI_Manager* m_guiManager;
	DebugOverlay m_debugOverlay;
};