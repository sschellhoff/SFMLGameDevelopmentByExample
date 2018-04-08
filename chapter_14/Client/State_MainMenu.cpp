#include "State_MainMenu.h"
#include "StateManager.h"

State_MainMenu::State_MainMenu(StateManager* l_stateManager)
	: BaseState(l_stateManager){}

State_MainMenu::~State_MainMenu(){}

void State_MainMenu::OnCreate(){
	SetTransparent(true); // Transparent for rendering.
	SetTranscendent(true); // Transcendent for updating.
	GUI_Manager* gui = m_stateMgr->GetContext()->m_guiManager;
	gui->LoadInterface(StateType::MainMenu, "MainMenu.interface", "MainMenu");
	Window* wnd = m_stateMgr->GetContext()->m_wind;
	gui->GetInterface(StateType::MainMenu, "MainMenu")->SetPosition(sf::Vector2f(wnd->GetWindowSize().x / 3.2f, wnd->GetWindowSize().y / 3.5f));
	EventManager* eMgr = m_stateMgr->GetContext()->m_eventManager;
	eMgr->AddCallback(StateType::MainMenu, "MainMenu_Play", &State_MainMenu::Play, this);
	eMgr->AddCallback(StateType::MainMenu, "MainMenu_Disconnect", &State_MainMenu::Disconnect, this);
	eMgr->AddCallback(StateType::MainMenu, "MainMenu_Quit", &State_MainMenu::Quit, this);
}

void State_MainMenu::OnDestroy(){
	m_stateMgr->GetContext()->m_guiManager->RemoveInterface(StateType::MainMenu, "MainMenu");
	EventManager* eMgr = m_stateMgr->GetContext()->m_eventManager;
	GUI_Manager* gui = m_stateMgr->GetContext()->m_guiManager;
	gui->RemoveInterface(StateType::MainMenu, "MainMenu");
	eMgr->RemoveCallback(StateType::MainMenu, "MainMenu_Play");
	eMgr->RemoveCallback(StateType::MainMenu, "MainMenu_Disconnect");
	eMgr->RemoveCallback(StateType::MainMenu, "MainMenu_Quit");
}

void State_MainMenu::Activate(){
	GUI_Interface* menu = m_stateMgr->GetContext()->m_guiManager->GetInterface(StateType::MainMenu, "MainMenu");
	if (m_stateMgr->HasState(StateType::Game)){
		// Resume
		menu->GetElement("Play")->SetText("Resume");
		menu->GetElement("Disconnect")->SetActive(true);
		menu->GetElement("IP")->SetActive(false);
		menu->GetElement("PORT")->SetActive(false);
		menu->GetElement("IpLabel")->SetActive(false);
		menu->GetElement("PortLabel")->SetActive(false);
		menu->GetElement("NameLabel")->SetActive(false);
		menu->GetElement("Nickname")->SetActive(false);
	} else {
		// Play
		menu->GetElement("Play")->SetText("CONNECT");
		menu->GetElement("Disconnect")->SetActive(false);
		menu->GetElement("IP")->SetActive(true);
		menu->GetElement("PORT")->SetActive(true);
		menu->GetElement("IpLabel")->SetActive(true);
		menu->GetElement("PortLabel")->SetActive(true);
		menu->GetElement("NameLabel")->SetActive(true);
		menu->GetElement("Nickname")->SetActive(true);
	}
}

void State_MainMenu::Play(EventDetails* l_details){
	if (!m_stateMgr->HasState(StateType::Game)){
		GUI_Interface* menu = m_stateMgr->GetContext()->m_guiManager->GetInterface(StateType::MainMenu, "MainMenu");
		std::string ip = menu->GetElement("IP")->GetText();
		PortNumber port = std::atoi(menu->GetElement("PORT")->GetText().c_str());
		std::string name = menu->GetElement("Nickname")->GetText();
		m_stateMgr->GetContext()->m_client->SetServerInformation(ip, port);
		m_stateMgr->GetContext()->m_client->SetPlayerName(name);
	}
	m_stateMgr->SwitchTo(StateType::Game);
}

void State_MainMenu::Disconnect(EventDetails* l_details){
	m_stateMgr->GetContext()->m_client->Disconnect();
}

void State_MainMenu::Quit(EventDetails* l_details){ m_stateMgr->GetContext()->m_wind->Close(); }
void State_MainMenu::Draw(){}
void State_MainMenu::Update(const sf::Time& l_time){}
void State_MainMenu::Deactivate(){}