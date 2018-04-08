#pragma once
#include "BaseState.h"
#include "EventManager.h"

class State_MainMenu : public BaseState{
public:
	State_MainMenu(StateManager* l_stateManager);
	~State_MainMenu();

	void OnCreate();
	void OnDestroy();

	void Activate();
	void Deactivate();

	void Update(const sf::Time& l_time);
	void Draw();

	void Play(EventDetails* l_details);
	void Disconnect(EventDetails* l_details);
	void Quit(EventDetails* l_details);
private:

};