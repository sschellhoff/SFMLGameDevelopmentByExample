#include "EventManager.h"
#include "StateManager.h"

EventManager::EventManager()
	:m_currentState(StateType(0)), m_hasFocus(true)
{
	LoadBindings();
}

EventManager::~EventManager(){
	for(auto &itr : m_bindings){
		delete itr.second;
	}
}

bool EventManager::AddBinding(Binding *l_binding){
	if(m_bindings.find(l_binding->m_name) != m_bindings.end())
		return false;
	return m_bindings.emplace(l_binding->m_name, l_binding).second;
}

bool EventManager::RemoveBinding(std::string l_name){
	auto itr = m_bindings.find(l_name);
	if (itr == m_bindings.end()){ return false; }
	delete itr->second;
	m_bindings.erase(itr);
	return true;
}

void EventManager::SetCurrentState(StateType l_state){
	m_currentState = l_state;
}

void EventManager::SetFocus(bool l_focus){ m_hasFocus = l_focus; }

void EventManager::HandleEvent(sf::Event& l_event){
	// Handling SFML events.
	for(auto &b_itr : m_bindings){
		Binding* bind = b_itr.second;
		for(auto &e_itr : bind->m_events){
			EventType sfmlEvent = (EventType)l_event.type;
			if (e_itr.first == EventType::GUI_Click || e_itr.first == EventType::GUI_Release ||
				e_itr.first == EventType::GUI_Hover || e_itr.first == EventType::GUI_Leave)
			{
				continue;
			}
			if (e_itr.first != sfmlEvent){ continue; }
			if (sfmlEvent == EventType::KeyDown || sfmlEvent == EventType::KeyUp){
				if (e_itr.second.m_code == l_event.key.code){
					// Matching event/keystroke.
					// Increase count.
					if (bind->m_details.m_keyCode != -1){
						bind->m_details.m_keyCode = e_itr.second.m_code;
					}
					++(bind->c);
					break;
				}
			} else if (sfmlEvent == EventType::MButtonDown || sfmlEvent == EventType::MButtonUp){
				if (e_itr.second.m_code == l_event.mouseButton.button){
					// Matching event/keystroke.
					// Increase count.
					bind->m_details.m_mouse.x = l_event.mouseButton.x;
					bind->m_details.m_mouse.y = l_event.mouseButton.y;
					if (bind->m_details.m_keyCode != -1){
						bind->m_details.m_keyCode = e_itr.second.m_code;
					}
					++(bind->c);
					break;
				}
			} else {
				// No need for additional checking.
				if (sfmlEvent == EventType::MouseWheel){
					bind->m_details.m_mouseWheelDelta = l_event.mouseWheel.delta;
				} else if (sfmlEvent == EventType::WindowResized){
					bind->m_details.m_size.x = l_event.size.width;
					bind->m_details.m_size.y = l_event.size.height;
				} else if (sfmlEvent == EventType::TextEntered){
					bind->m_details.m_textEntered = l_event.text.unicode;
				}
				++(bind->c);
			}
		}
	}
}

void EventManager::HandleEvent(GUI_Event& l_event){
	for (auto &b_itr : m_bindings){
		Binding* bind = b_itr.second;
		for (auto &e_itr : bind->m_events)
		{
			if (e_itr.first != EventType::GUI_Click && e_itr.first != EventType::GUI_Release &&
				e_itr.first != EventType::GUI_Hover && e_itr.first != EventType::GUI_Leave)
			{ continue; }
			if ((e_itr.first == EventType::GUI_Click && l_event.m_type != GUI_EventType::Click) ||
				(e_itr.first == EventType::GUI_Release && l_event.m_type != GUI_EventType::Release) ||
				(e_itr.first == EventType::GUI_Hover && l_event.m_type != GUI_EventType::Hover) ||
				(e_itr.first == EventType::GUI_Leave && l_event.m_type != GUI_EventType::Leave))
			{ continue; }
			// REPLACED WITH STRCMP!
			if (strcmp(e_itr.second.m_gui.m_interface, l_event.m_interface) ||
				strcmp(e_itr.second.m_gui.m_element, l_event.m_element))
			{ continue; }
			bind->m_details.m_guiInterface = l_event.m_interface;
			bind->m_details.m_guiElement = l_event.m_element;
			++(bind->c);
		}
	}
}

void EventManager::Update(){
	if (!m_hasFocus){ return; }
	for (auto &b_itr : m_bindings){
		Binding* bind = b_itr.second;
		for(auto &e_itr : bind->m_events){
			switch(e_itr.first){
			case(EventType::Keyboard):
				if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key(e_itr.second.m_code))){
					if (bind->m_details.m_keyCode != -1){
						bind->m_details.m_keyCode = e_itr.second.m_code;
					}
					++(bind->c);
				}
				break;
			case(EventType::Mouse):
				if(sf::Mouse::isButtonPressed(sf::Mouse::Button(e_itr.second.m_code))){
					if (bind->m_details.m_keyCode != -1){
						bind->m_details.m_keyCode = e_itr.second.m_code;
					}
					++(bind->c);
				}
				break;
			case(EventType::Joystick):
				// Up for expansion.
				break;
			}
		}

		if(bind->m_events.size() == bind->c){
			auto stateCallbacks = m_callbacks.find(m_currentState);
			auto otherCallbacks = m_callbacks.find(StateType(0));

			if(stateCallbacks != m_callbacks.end()){
				auto callItr = stateCallbacks->second.find(bind->m_name);
				if(callItr != stateCallbacks->second.end()){
					// Pass in information about events.
					callItr->second(&bind->m_details);
				}
			}

			if(otherCallbacks != m_callbacks.end()){
				auto callItr = otherCallbacks->second.find(bind->m_name);
				if(callItr != otherCallbacks->second.end()){
					// Pass in information about events.
					callItr->second(&bind->m_details);
				}
			}
		}
		bind->c = 0;
		bind->m_details.Clear();
	}
}

void EventManager::LoadBindings(){
	std::string delimiter = ":";

	std::ifstream bindings;
	bindings.open(Utils::GetWorkingDirectory() + "keys.cfg");
	if (!bindings.is_open()){ std::cout << "! Failed loading keys.cfg." << std::endl; return; }
	std::string line;
	while(std::getline(bindings,line)){
		std::stringstream keystream(line);
		std::string callbackName;
		keystream >> callbackName;
		Binding* bind = new Binding(callbackName);
		while(!keystream.eof()){
			std::string keyval;
			keystream >> keyval;
			int start = 0;
			int end = keyval.find(delimiter);
			if (end == std::string::npos){ delete bind; bind = nullptr; break; }
			EventType type = EventType(stoi(keyval.substr(start, end-start)));

			EventInfo eventInfo;
			if (type == EventType::GUI_Click || type == EventType::GUI_Release ||
				type == EventType::GUI_Hover || type == EventType::GUI_Leave)
			{
				start = end + delimiter.length();
				end = keyval.find(delimiter, start);
				std::string window = keyval.substr(start, end - start);
				std::string element;
				if (end != std::string::npos){
					start = end + delimiter.length();
					end = keyval.length();
					element = keyval.substr(start, end);
				}
				char* w = new char[window.length() + 1]; // +1 for \0
				char* e = new char[element.length() + 1];

				// Size in bytes is the same as charater length. 1 char = 1B.
				strcpy_s(w, window.length() + 1, window.c_str());
				strcpy_s(e, element.length() + 1, element.c_str());

				eventInfo.m_gui.m_interface = w;
				eventInfo.m_gui.m_element = e;
			} else {
				int code = stoi(keyval.substr(end + delimiter.length(),
					keyval.find(delimiter,end + delimiter.length())));
				eventInfo.m_code = code;
			}
			bind->BindEvent(type, eventInfo);
		}

		if(!AddBinding(bind)){ delete bind; }
		bind = nullptr;
	}
	bindings.close();
}