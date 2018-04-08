#pragma once
#include "Window.h"
#include "World.h"
#include "Snake.h"
#include "Textbox.h"

class Game{
public:
	Game();
	~Game();

	void HandleInput();
	void Update();
	void Render();

	sf::Time GetElapsed();
	void RestartClock();

	Window* GetWindow();
private:
	Window m_window;
	sf::Clock m_clock;
	float m_elapsed;

	World m_world;
	Snake m_snake;
	Textbox m_textbox;
};