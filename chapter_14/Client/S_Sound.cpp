#include "S_Sound.h"
#include "System_Manager.h"
#include "Map.h"

S_Sound::S_Sound(SystemManager* l_systemMgr)
	: S_Base(System::Sound, l_systemMgr), m_audioManager(nullptr), m_soundManager(nullptr)
{
	Bitmask req;
	req.TurnOnBit((unsigned int)Component::Position);
	req.TurnOnBit((unsigned int)Component::State);
	req.TurnOnBit((unsigned int)Component::SoundEmitter);
	m_requiredComponents.push_back(req);
	req.ClearBit((unsigned int)Component::SoundEmitter);
	req.TurnOnBit((unsigned int)Component::SoundListener);
	m_requiredComponents.push_back(req);

	m_systemManager->GetMessageHandler()->Subscribe(EntityMessage::Direction_Changed, this);
	m_systemManager->GetMessageHandler()->Subscribe(EntityMessage::Frame_Change, this);
}

S_Sound::~S_Sound(){}

void S_Sound::Update(float l_dT){
	EntityManager* entities = m_systemManager->GetEntityManager();
	for (auto &entity : m_entities){
		C_Position* c_pos = entities->GetComponent<C_Position>(entity, Component::Position);
		sf::Vector2f position = c_pos->GetPosition();
		unsigned int elevation = c_pos->GetElevation();

		bool IsListener = entities->HasComponent(entity, Component::SoundListener);
		if (IsListener){ sf::Listener::setPosition(MakeSoundPosition(position, elevation)); }

		if (!entities->HasComponent(entity, Component::SoundEmitter)){ continue; }
		C_SoundEmitter* c_snd = entities->GetComponent<C_SoundEmitter>(entity, Component::SoundEmitter);
		if (c_snd->GetSoundID() == -1){ continue; }
		if (!IsListener){
			if (!m_soundManager->SetPosition(c_snd->GetSoundID(), MakeSoundPosition(position, elevation)))
			{
				c_snd->SetSoundID(-1);
			}
		} else {
			if (!m_soundManager->IsPlaying(c_snd->GetSoundID())){ c_snd->SetSoundID(-1); }
		}
	}
}

void S_Sound::HandleEvent(const EntityId& l_entity,
	const EntityEvent& l_event){}

void S_Sound::Notify(const Message& l_message){
	if (!HasEntity(l_message.m_receiver)){ return; }
	EntityManager* entities = m_systemManager->GetEntityManager();
	bool IsListener = entities->HasComponent(l_message.m_receiver, Component::SoundListener);
	EntityMessage m = (EntityMessage)l_message.m_type;
	switch (m){
	case EntityMessage::Direction_Changed:
	{
		if (!entities->HasComponent(l_message.m_receiver, Component::SoundListener)){ return; }
		Direction dir = (Direction)l_message.m_int;
		switch (dir){
		case Direction::Up: sf::Listener::setDirection(0, 0, -1); break;
		case Direction::Down: sf::Listener::setDirection(0, 0, 1); break;
		case Direction::Left: sf::Listener::setDirection(-1, 0, 0); break;
		case Direction::Right: sf::Listener::setDirection(1, 0, 0); break;
		}
	}
		break;
	case EntityMessage::Frame_Change:
		if (!entities->HasComponent(l_message.m_receiver, Component::SoundEmitter)){ return; }
		EntityState state = entities->GetComponent<C_State>(l_message.m_receiver, Component::State)->GetState();
		EntitySound sound = EntitySound::None;
		if (state == EntityState::Walking){ sound = EntitySound::Footstep;}
		else if (state == EntityState::Attacking){ sound = EntitySound::Attack; }
		else if (state == EntityState::Hurt){ sound = EntitySound::Hurt; }
		else if (state == EntityState::Dying){ sound = EntitySound::Death; }
		if (sound == EntitySound::None){ return; }
		EmitSound(l_message.m_receiver, sound, false, IsListener, l_message.m_int);
		break;
	}
}

void S_Sound::SetUp(AudioManager* l_audioManager, 
	SoundManager* l_soundManager)
{ 
	m_audioManager = l_audioManager;
	m_soundManager = l_soundManager;
}

sf::Vector3f S_Sound::MakeSoundPosition(const sf::Vector2f& l_entityPos, unsigned int l_elevation){
	return sf::Vector3f(l_entityPos.x, l_elevation * Sheet::Tile_Size, l_entityPos.y);
}

void S_Sound::EmitSound(const EntityId& l_entity, const EntitySound& l_sound,
	bool l_useId, bool l_relative, int l_checkFrame)
{
	if (!HasEntity(l_entity)){ return; }
	if (!m_systemManager->GetEntityManager()->HasComponent(l_entity, Component::SoundEmitter)){ return; }
	// Is a sound emitter.
	EntityManager* entities = m_systemManager->GetEntityManager();
	C_SoundEmitter* c_snd = entities->GetComponent<C_SoundEmitter>(l_entity, Component::SoundEmitter);
	if (c_snd->GetSoundID() != -1 && l_useId){ return; } // Use of ID is required AND sound is occupied.
	// If sound is free or use of ID isn't required.
	if (l_checkFrame != -1 && !c_snd->IsSoundFrame(l_sound, l_checkFrame)){ return; }
	// Frame is irrelevant or correct.
	C_Position* c_pos = entities->GetComponent<C_Position>(l_entity, Component::Position);
	sf::Vector3f pos = (l_relative ? sf::Vector3f(0.f, 0.f, 0.f)
		: MakeSoundPosition(c_pos->GetPosition(), c_pos->GetElevation()));
	if (l_useId){ c_snd->SetSoundID(m_soundManager->Play(c_snd->GetSound(l_sound), pos)); }
	else { m_soundManager->Play(c_snd->GetSound(l_sound), pos, false, l_relative); }
}