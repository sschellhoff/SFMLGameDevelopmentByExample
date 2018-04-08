#pragma once
#include "S_Base.h"
#include "C_State.h"

class S_SheetAnimation : public S_Base{
public:
	S_SheetAnimation(SystemManager* l_systemMgr);
	~S_SheetAnimation();

	void Update(float l_dT);
	void HandleEvent(const EntityId& l_entity,const EntityEvent& l_event);
	void Notify(const Message& l_message);
private:
	void ChangeAnimation(const EntityId& l_entity, 
		const std::string& l_anim, bool l_play, bool l_loop);
};