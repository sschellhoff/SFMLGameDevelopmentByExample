#pragma once
#include "BaseState.h"
#include "Map.h"
#include "EventManager.h"

class State_Game : public BaseState{
public:
	State_Game(StateManager* l_stateManager);
	~State_Game();

	void OnCreate();
	void OnDestroy();

	void Activate();
	void Deactivate();

	void Update(const sf::Time& l_time);
	void Draw();

	void MainMenu(EventDetails* l_details);
	void Pause(EventDetails* l_details);
	void ToggleOverlay(EventDetails* l_details);
private:
	Map* m_gameMap;
};