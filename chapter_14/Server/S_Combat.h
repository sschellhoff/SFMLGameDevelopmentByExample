#pragma once
#include "S_Base.h"
#include "C_Position.h"
#include "C_Attacker.h"
#include "C_Health.h"
#include "S_State.h"
#include "C_Movable.h"

class S_Combat : public S_Base{
public:
	S_Combat(SystemManager* l_systemMgr);
	~S_Combat();

	void Update(float l_dT);
	void HandleEvent(const EntityId& l_entity, const EntityEvent& l_event);
	void Notify(const Message& l_message);
private:

};