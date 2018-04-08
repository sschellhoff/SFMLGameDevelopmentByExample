#include "Window.h"

Window::Window(){ 
	Setup("Window", sf::Vector2u(640,480));
}

Window::Window(const std::string& title, const sf::Vector2u& size){ 
	Setup(title,size);
}

Window::~Window(){ m_window.close(); }

void Window::Setup(const std::string& title, const sf::Vector2u& size){
	m_windowTitle = title;
	m_windowSize = size;
	m_isFullscreen = false;
	m_isDone = false;
	m_isFocused = true;

	m_eventManager.AddCallback(StateType(0),"Fullscreen_toggle", &Window::ToggleFullscreen, this);
	m_eventManager.AddCallback(StateType(0),"Window_close",&Window::Close,this);

	Create();
}

void Window::Create(){
	sf::Uint32 style = sf::Style::Default;
	if(m_isFullscreen){ style = sf::Style::Fullscreen; }

	m_window.create(sf::VideoMode(m_windowSize.x,m_windowSize.y,32),m_windowTitle,style);
}

void Window::BeginDraw(){ m_window.clear(sf::Color::Black); }
void Window::EndDraw(){ m_window.display(); }

bool Window::IsDone(){ return m_isDone; }
bool Window::IsFullscreen(){ return m_isFullscreen; }
bool Window::IsFocused(){ return m_isFocused; }

sf::RenderWindow* Window::GetRenderWindow(){ return &m_window; }
EventManager* Window::GetEventManager(){ return &m_eventManager; }
sf::Vector2u Window::GetWindowSize(){ return m_windowSize; }
sf::FloatRect Window::GetViewSpace(){
	sf::Vector2f viewCenter(m_window.getView().getCenter());
	sf::Vector2f viewSize(m_window.getView().getSize());
	sf::Vector2f viewSizeHalf(viewSize.x / 2, viewSize.y / 2);
	sf::FloatRect viewSpace(viewCenter - viewSizeHalf, viewSize);
	return viewSpace;
}

void Window::ToggleFullscreen(EventDetails* l_details){
	m_isFullscreen = !m_isFullscreen;
	m_window.close();
	Create();
}

void Window::Close(EventDetails* l_details){ m_isDone = true; }

void Window::Update(){
	sf::Event event;

	while(m_window.pollEvent(event)){
		if (event.type == sf::Event::LostFocus){ m_isFocused = false; m_eventManager.SetFocus(false); }
		else if (event.type == sf::Event::GainedFocus){ m_isFocused = true; m_eventManager.SetFocus(true); }
		m_eventManager.HandleEvent(event);
	}

	m_eventManager.Update();
}