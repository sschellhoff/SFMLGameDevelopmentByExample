#include "S_SheetAnimation.h"
#include "System_Manager.h"

S_SheetAnimation::S_SheetAnimation(SystemManager* l_systemMgr)
	: S_Base(System::SheetAnimation,l_systemMgr)
{
	Bitmask req;
	req.TurnOnBit((unsigned int)Component::SpriteSheet);
	req.TurnOnBit((unsigned int)Component::State);
	m_requiredComponents.push_back(req);

	m_systemManager->GetMessageHandler()->Subscribe(EntityMessage::State_Changed,this);
}

S_SheetAnimation::~S_SheetAnimation(){}

void S_SheetAnimation::Update(float l_dT){
	EntityManager* entities = m_systemManager->GetEntityManager();
	for(auto &entity : m_entities){
		C_SpriteSheet* sheet = entities->GetComponent<C_SpriteSheet>(entity, Component::SpriteSheet);
		C_State* state = entities->GetComponent<C_State>(entity, Component::State);

		sheet->GetSpriteSheet()->Update(l_dT);

		const std::string& animName = sheet->GetSpriteSheet()->GetCurrentAnim()->GetName();
		if(animName == "Attack"){
			if(!sheet->GetSpriteSheet()->GetCurrentAnim()->IsPlaying())
			{
				Message msg((MessageType)EntityMessage::Switch_State);
				msg.m_receiver = entity;
				msg.m_int = (int)EntityState::Idle;
				m_systemManager->GetMessageHandler()->Dispatch(msg);
			} else if(sheet->GetSpriteSheet()->GetCurrentAnim()->IsInAction())
			{
				Message msg((MessageType)EntityMessage::Attack_Action);
				msg.m_sender = entity;
				m_systemManager->GetMessageHandler()->Dispatch(msg);
			}
		} else if(animName == "Death" && !sheet->GetSpriteSheet()->GetCurrentAnim()->IsPlaying()){
			Message msg((MessageType)EntityMessage::Dead);
			msg.m_receiver = entity;
			m_systemManager->GetMessageHandler()->Dispatch(msg);
		}
		if (sheet->GetSpriteSheet()->GetCurrentAnim()->CheckMoved()){
			int frame = sheet->GetSpriteSheet()->GetCurrentAnim()->GetFrame();
			Message msg((MessageType)EntityMessage::Frame_Change);
			msg.m_receiver = entity;
			msg.m_int = frame;
			m_systemManager->GetMessageHandler()->Dispatch(msg);
		}
	}
}

void S_SheetAnimation::HandleEvent(const EntityId& l_entity,const EntityEvent& l_event){}

void S_SheetAnimation::Notify(const Message& l_message){
	if(HasEntity(l_message.m_receiver)){
		EntityMessage m = (EntityMessage)l_message.m_type;
		switch(m){
		case EntityMessage::State_Changed:
			{
				EntityState s = (EntityState)l_message.m_int;
				switch(s){
				case EntityState::Idle:
					ChangeAnimation(l_message.m_receiver,"Idle",true,true);
					break;
				case EntityState::Walking:
					ChangeAnimation(l_message.m_receiver,"Walk",true,true);
					break;
				case EntityState::Attacking:
					ChangeAnimation(l_message.m_receiver,"Attack",true,false);
					break;
				case EntityState::Hurt: break;
				case EntityState::Dying:
					ChangeAnimation(l_message.m_receiver,"Death",true,false);
					break;
				}
			}
			break;
		}
	}
}

void S_SheetAnimation::ChangeAnimation(const EntityId& l_entity, 
	const std::string& l_anim, bool l_play, bool l_loop)
{
	C_SpriteSheet* sheet = m_systemManager->GetEntityManager()->GetComponent<C_SpriteSheet>(l_entity,Component::SpriteSheet);
	sheet->GetSpriteSheet()->SetAnimation(l_anim,l_play,l_loop);
}