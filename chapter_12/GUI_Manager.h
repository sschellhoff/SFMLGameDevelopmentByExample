#pragma once
#include <SFML/Graphics.hpp>
#include "EventManager.h"

#include "GUI_Interface.h"
#include "GUI_Event.h"
#include "GUI_Label.h"
#include "GUI_Scrollbar.h"
#include "GUI_Textfield.h"

#include <unordered_map>
#include <functional>
#include <fstream>

using GUI_Interfaces = std::unordered_map<std::string, GUI_Interface*>;
using GUI_Container = std::unordered_map<StateType, GUI_Interfaces>;
using GUI_Events = std::unordered_map<StateType, std::vector<GUI_Event>>;
using GUI_Factory = std::unordered_map<GUI_ElementType, std::function<GUI_Element*(GUI_Interface*)>>;
using GUI_ElemTypes = std::unordered_map <std::string,GUI_ElementType>;

struct SharedContext;
class GUI_Manager{
	friend class GUI_Interface;
public:
	GUI_Manager(EventManager* l_evMgr, SharedContext* l_shared);
	~GUI_Manager();

	bool AddInterface(const StateType& l_state, 
		const std::string& l_name);
	GUI_Interface* GetInterface(const StateType& l_state,
		const std::string& l_name);
	bool RemoveInterface(const StateType& l_state,
		const std::string& l_name);

	bool LoadInterface(const StateType& l_state,
		const std::string& l_interface, const std::string& l_name);

	void SetCurrentState(const StateType& l_state);

	SharedContext* GetContext();

	void DefocusAllInterfaces();

	void HandleClick(EventDetails* l_details);
	void HandleRelease(EventDetails* l_details);
	void HandleTextEntered(EventDetails* l_details);

	void AddEvent(GUI_Event l_event);
	bool PollEvent(GUI_Event& l_event);

	void Update(float l_dT);
	void Render(sf::RenderWindow* l_wind);

	template<class T>
	void RegisterElement(const GUI_ElementType& l_id){
		m_factory[l_id] = [](GUI_Interface* l_owner) -> GUI_Element*
		{ return new T("",l_owner); };
	}
private:
	GUI_Element* CreateElement(const GUI_ElementType& l_id, GUI_Interface* l_owner);
	GUI_ElementType StringToType(const std::string& l_string);
	bool LoadStyle(const std::string& l_file, GUI_Element* l_element);

	GUI_Container m_interfaces;
	GUI_Events m_events;
	SharedContext* m_context;
	EventManager* m_eventMgr;
	StateType m_currentState;
	GUI_Factory m_factory;
	GUI_ElemTypes m_elemTypes;
};