#pragma once
#include "Window.h"
#include "EventManager.h"
#include "TextureManager.h"
#include "System_Manager.h"
#include "Entity_Manager.h"
#include "DebugOverlay.h"

class Map;

struct SharedContext{
	SharedContext():
		m_wind(nullptr),
		m_eventManager(nullptr),
		m_textureManager(nullptr),
		m_systemManager(nullptr),
		m_entityManager(nullptr),
		m_gameMap(nullptr){}

	Window* m_wind;
	EventManager* m_eventManager;
	TextureManager* m_textureManager;
	SystemManager* m_systemManager;
	EntityManager* m_entityManager;
	Map* m_gameMap;
	DebugOverlay m_debugOverlay;
};