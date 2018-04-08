#pragma once
#include "S_Base.h"
#include "C_State.h"
#include "Directions.h"

class S_State : public S_Base{
public:
	S_State(SystemManager* l_systemMgr);
	~S_State();

	void Update(float l_dT);
	void HandleEvent(const EntityId& l_entity, const EntityEvent& l_event);
	void Notify(const Message& l_message);

	void ChangeState(const EntityId& l_entity, const EntityState& l_state, bool l_force);
	EntityState GetState(const EntityId& l_entity);
private:
};