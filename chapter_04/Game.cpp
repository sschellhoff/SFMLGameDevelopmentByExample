#include "Game.h"

void Game::MoveSprite(EventDetails* l_details){
	sf::Vector2i mousepos = m_window.GetEventManager()->GetMousePos(m_window.GetRenderWindow());
	m_sprite.setPosition(mousepos.x, mousepos.y);
	std::cout << "Moving sprite to: " << mousepos.x << ":" << mousepos.y << std::endl;
}

Game::Game(): m_window("Chapter 4",sf::Vector2u(800,600)){
	Clock.restart();
	srand(time(nullptr));

	m_texture.loadFromFile("Mushroom.png");
	m_sprite.setTexture(m_texture);
	m_sprite.setOrigin(m_texture.getSize().x / 2, m_texture.getSize().y / 2);
	m_sprite.setPosition(0,0);

	m_window.GetEventManager()->AddCallback("Move",&Game::MoveSprite,this);
}

Game::~Game(){}

sf::Time Game::GetElapsed(){ return Clock.getElapsedTime(); }
void Game::RestartClock(){ Clock.restart(); }
Window* Game::GetWindow(){ return &m_window; }

void Game::Update(){
	m_window.Update();
}

void Game::Render(){
	m_window.BeginDraw();
	// Render here.
	m_window.GetRenderWindow()->draw(m_sprite);
	m_window.EndDraw();
}