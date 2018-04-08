#pragma once
#include "C_Base.h"
#include "NetworkDefinitions.h"

class C_Client : public C_Base{
public:
	C_Client(): C_Base(Component::Client), m_clientID((ClientID)Network::NullID){}
	void ReadIn(std::stringstream& l_stream){}

	ClientID GetClientID()const{ return m_clientID; }
	void SetClientID(const ClientID& l_id){ m_clientID = l_id; }
private:
	ClientID m_clientID;
};