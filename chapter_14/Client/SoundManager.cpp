#include "SoundManager.h"
#include "StateManager.h"

SoundManager::SoundManager(AudioManager* l_audioMgr)
	: m_lastID(0), m_audioManager(l_audioMgr), m_elapsed(0.f), m_numSounds(0){}

SoundManager::~SoundManager(){ Cleanup(); }

void SoundManager::ChangeState(const StateType& l_state){
	PauseAll(m_currentState);
	UnpauseAll(l_state);
	m_currentState = l_state;

	if (m_music.find(m_currentState) != m_music.end()){ return; }
	SoundInfo info("");
	sf::Music* music = nullptr;
	m_music.emplace(m_currentState, std::make_pair(info, music));
}

void SoundManager::RemoveState(const StateType& l_state){
	auto& StateSounds = m_audio.find(l_state)->second;
	for (auto &itr : StateSounds){
		RecycleSound(itr.first, itr.second.second, itr.second.first.m_name);
	}
	m_audio.erase(l_state);
	auto music = m_music.find(l_state);
	if (music == m_music.end()){ return; }
	if (music->second.second){
		delete music->second.second;
		--m_numSounds;
	}
	m_music.erase(l_state);
}

void SoundManager::Cleanup(){
	for (auto &state : m_audio){
		for (auto &sound : state.second){
			m_audioManager->ReleaseResource(sound.second.first.m_name);
			delete sound.second.second;
		}
	}
	m_audio.clear();
	for (auto &recycled : m_recycled){
		m_audioManager->ReleaseResource(recycled.first.second);
		delete recycled.second;
	}
	m_recycled.clear();
	for (auto &music : m_music){
		if (music.second.second){
			delete music.second.second;
		}
	}
	m_music.clear();

	m_properties.clear();
	m_numSounds = 0;
	m_lastID = 0;
}

void SoundManager::Update(float l_dT){
	m_elapsed += l_dT;
	if (m_elapsed < 0.33f){ return; }
	// Run once every third of a second.
	m_elapsed = 0;
	auto& container = m_audio[m_currentState];
	for (auto itr = container.begin(); itr != container.end();){
		if (!itr->second.second->getStatus()){
			RecycleSound(itr->first, itr->second.second, itr->second.first.m_name);
			itr = container.erase(itr); // Remove sound.
			continue;
		}
		++itr;
	}
	auto music = m_music.find(m_currentState);
	if (music == m_music.end()){ return; }
	if (!music->second.second){ return; }
	if (music->second.second->getStatus()){ return; }
	delete music->second.second;
	music->second.second = nullptr;
	--m_numSounds;
}

SoundID SoundManager::Play(const std::string& l_sound,
	const sf::Vector3f& l_position, bool l_loop, bool l_relative)
{
	SoundProps* props = GetSoundProperties(l_sound);
	if (!props){ return -1; } // Failed to load sound properties.
	SoundID id;
	sf::Sound* sound = CreateSound(id, props->m_audioName);
	if (!sound){ return -1; }
	// Sound created successfully.
	SetUpSound(sound, props, l_loop, l_relative);
	sound->setPosition(l_position);
	SoundInfo info(props->m_audioName);
	m_audio[m_currentState].emplace(id, std::make_pair(info, sound));
	sound->play();
	return id;
}

bool SoundManager::Play(const SoundID& l_id){
	auto& container = m_audio[m_currentState];
	auto sound = container.find(l_id);
	if (sound == container.end()){ return false; }
	sound->second.second->play();
	sound->second.first.m_manualPaused = false;
	return true;
}

bool SoundManager::Stop(const SoundID& l_id){
	auto& container = m_audio[m_currentState];
	auto sound = container.find(l_id);
	if (sound == container.end()){ return false; }
	sound->second.second->stop();
	sound->second.first.m_manualPaused = true;
	return true;
}

bool SoundManager::Pause(const SoundID& l_id){
	auto& container = m_audio[m_currentState];
	auto sound = container.find(l_id);
	if (sound == container.end()){ return false; }
	sound->second.second->pause();
	sound->second.first.m_manualPaused = true;
	return true;
}

bool SoundManager::PlayMusic(const std::string& l_musicId, float l_volume, bool l_loop){
	auto s = m_music.find(m_currentState);
	if (s == m_music.end()){ return false; }
	std::string path = m_audioManager->GetPath(l_musicId);
	if (path == ""){ return false; }
	if (!s->second.second){
		s->second.second = new sf::Music();
		++m_numSounds;
	}
	sf::Music* music = s->second.second;
	if (!music->openFromFile(Utils::GetWorkingDirectory() + path)){
		delete music;
		--m_numSounds;
		s->second.second = nullptr;
		std::cerr << "[SoundManager] Failed to load music from file: " << l_musicId << std::endl;
		return false;
	}
	music->setLoop(l_loop);
	music->setVolume(l_volume);
	music->setRelativeToListener(true); // Always relative.
	music->play();
	s->second.first.m_name = l_musicId;
	return true;
}

bool SoundManager::PlayMusic(const StateType& l_state){
	auto music = m_music.find(m_currentState);
	if (music == m_music.end()){ return false; }
	if (!music->second.second){ return false; }
	music->second.second->play();
	music->second.first.m_manualPaused = false;
	return true;
}

bool SoundManager::StopMusic(const StateType& l_state){
	auto music = m_music.find(m_currentState);
	if (music == m_music.end()){ return false; }
	if (!music->second.second){ return false; }
	music->second.second->stop();
	delete music->second.second;
	music->second.second = nullptr;
	--m_numSounds;
	return true;
}

bool SoundManager::PauseMusic(const StateType& l_state){
	auto music = m_music.find(m_currentState);
	if (music == m_music.end()){ return false; }
	if (!music->second.second){ return false; }
	music->second.second->pause();
	music->second.first.m_manualPaused = true;
	return true;
}

bool SoundManager::SetPosition(const SoundID& l_id, const sf::Vector3f& l_pos){
	auto& container = m_audio[m_currentState];
	auto sound = container.find(l_id);
	if (sound == container.end()){ return false; }
	sound->second.second->setPosition(l_pos);
	return true;
}

bool SoundManager::IsPlaying(const SoundID& l_id){
	auto& container = m_audio[m_currentState];
	auto sound = container.find(l_id);
	return (sound != container.end() ? sound->second.second->getStatus() : false);
}

SoundProps* SoundManager::GetSoundProperties(const std::string& l_soundName){
	auto& properties = m_properties.find(l_soundName);
	if (properties == m_properties.end()){
		if (!LoadProperties(l_soundName)){ return nullptr; }
		properties = m_properties.find(l_soundName);
	}
	return &properties->second;
}

bool SoundManager::LoadProperties(const std::string& l_name){
	std::ifstream file;
	file.open(Utils::GetWorkingDirectory() + "media/Sounds/" + l_name + ".sound");
	if (!file.is_open()){
		std::cerr << "Failed to load sound: " << l_name << std::endl;
		return false;
	}
	SoundProps props("");
	std::string line;
	while (std::getline(file, line)){
		if (line[0] == '|'){ continue; }
		std::stringstream keystream(line);
		std::string type;
		keystream >> type;
		if (type == "Audio"){
			keystream >> props.m_audioName;
		} else if (type == "Volume"){
			keystream >> props.m_volume;
		} else if (type == "Pitch"){
			keystream >> props.m_pitch;
		} else if (type == "Distance"){
			keystream >> props.m_minDistance;
		} else if (type == "Attenuation"){
			keystream >> props.m_attenuation;
		} else {
			// ?
		}
	}
	file.close();
	if (props.m_audioName == ""){ return false; }
	m_properties.emplace(l_name, props);
	return true;
}

void SoundManager::PauseAll(const StateType& l_state){
	auto& container = m_audio[l_state];
	for (auto itr = container.begin(); itr != container.end();){
		if (!itr->second.second->getStatus()){
			RecycleSound(itr->first, itr->second.second, itr->second.first.m_name);
			itr = container.erase(itr);
			continue;
		}
		itr->second.second->pause();
		++itr;
	}
	auto music = m_music.find(l_state);
	if (music == m_music.end()){ return; }
	if (!music->second.second){ return; }
	music->second.second->pause();
}

void SoundManager::UnpauseAll(const StateType& l_state){
	auto& container = m_audio[l_state];
	for (auto &itr : container){
		if (itr.second.first.m_manualPaused){ continue; }
		itr.second.second->play();
	}

	auto music = m_music.find(l_state);
	if (music == m_music.end()){ return; }
	if (!music->second.second || music->second.first.m_manualPaused){ return; }
	music->second.second->play();
}

sf::Sound* SoundManager::CreateSound(SoundID& l_id, const std::string& l_audioName){
	sf::Sound* sound = nullptr;
	if (!m_recycled.empty() && (m_numSounds >= Max_Sounds || m_recycled.size() >= Sound_Cache)){
		auto itr = m_recycled.begin();
		while (itr != m_recycled.end()){
			if (itr->first.second == l_audioName){ break; }
			++itr;
		}
		if (itr == m_recycled.end()){
			// If a sound with the same name hasn't been found!
			auto element = m_recycled.begin();
			l_id = element->first.first;
			m_audioManager->ReleaseResource(element->first.second);
			m_audioManager->RequireResource(l_audioName);
			sound = element->second;
			sound->setBuffer(*m_audioManager->GetResource(l_audioName));
			m_recycled.erase(element);
		} else {
			l_id = itr->first.first;
			sound = itr->second;
			m_recycled.erase(itr);
		}
		return sound;
	}
	if (m_numSounds < Max_Sounds){
		if (m_audioManager->RequireResource(l_audioName)){
			sound = new sf::Sound();
			l_id = m_lastID;
			++m_lastID;
			++m_numSounds;
			sound->setBuffer(*m_audioManager->GetResource(l_audioName));
			return sound;
		}
	}
	std::cerr << "[SoundManager] Failed to create sound." << std::endl;
	return nullptr;
}

void SoundManager::SetUpSound(sf::Sound* l_snd, const SoundProps* l_props, bool l_loop, bool l_relative)
{
	l_snd->setVolume(l_props->m_volume);
	l_snd->setPitch(l_props->m_pitch);
	l_snd->setMinDistance(l_props->m_minDistance);
	l_snd->setAttenuation(l_props->m_attenuation);
	l_snd->setLoop(l_loop);
	l_snd->setRelativeToListener(l_relative);
}

void SoundManager::RecycleSound(const SoundID& l_id,
	sf::Sound* l_snd, const std::string& l_name)
{
	m_recycled.emplace_back(std::make_pair(l_id, l_name), l_snd);
}