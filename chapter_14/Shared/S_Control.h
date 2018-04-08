#pragma once
#include "S_Base.h"
#include "Directions.h"
#include "C_Movable.h"

class S_Control : public S_Base{
public:
	S_Control(SystemManager* l_systemMgr);
	~S_Control();

	void Update(float l_dT);
	void HandleEvent(const EntityId& l_entity, const EntityEvent& l_event);
	void Notify(const Message& l_message);
	void MoveEntity(const EntityId& l_entity, const Direction& l_dir);
private:
};