#pragma once
#include "Window.h"
#include "EventManager.h"
#include <iostream>

class Game{
public:
	Game();
	~Game();

	void Update();
	void Render();

	sf::Time GetElapsed();
	void RestartClock();

	void MoveSprite(EventDetails* l_details);

	Window* GetWindow();
private:
	Window m_window;
	sf::Clock Clock;

	sf::Texture m_texture;
	sf::Sprite m_sprite;
};