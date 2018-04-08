#pragma once
#include <unordered_map>
#include <vector>
#include <string>
#include <SFML/Audio.hpp>
#include "AudioManager.h"
#include "SoundProps.h"
#include "Utilities.h"

using SoundID = int;

enum class StateType;

struct SoundInfo{
	SoundInfo(const std::string& l_name) : m_name(l_name), m_manualPaused(false){}
	std::string m_name;
	bool m_manualPaused;
};

using SoundProperties = std::unordered_map<std::string, SoundProps>;
using SoundContainer = std::unordered_map<SoundID, std::pair<SoundInfo, sf::Sound*>>;
using Sounds = std::unordered_map<StateType, SoundContainer>;
using RecycledSounds = std::vector<std::pair<std::pair<SoundID, std::string>, sf::Sound*>>;
using MusicContainer = std::unordered_map<StateType, std::pair<SoundInfo, sf::Music*>>;

class SoundManager{
public:
	SoundManager(AudioManager* l_audioMgr);
	~SoundManager();

	void ChangeState(const StateType& l_state);
	void RemoveState(const StateType& l_state);

	void Update(float l_dT);

	SoundID Play(const std::string& l_sound,
		const sf::Vector3f& l_position,
		bool l_loop = false,
		bool l_relative = false);
	bool Play(const SoundID& l_id);
	bool Stop(const SoundID& l_id);
	bool Pause(const SoundID& l_id);

	bool PlayMusic(const std::string& l_musicId, float l_volume = 100.f, bool l_loop = false);
	bool PlayMusic(const StateType& l_state);
	bool StopMusic(const StateType& l_state);
	bool PauseMusic(const StateType& l_state);

	bool SetPosition(const SoundID& l_id, const sf::Vector3f& l_pos);
	bool IsPlaying(const SoundID& l_id);
	SoundProps* GetSoundProperties(const std::string& l_soundName);

	static const int Max_Sounds = 150;
	static const int Sound_Cache = 75;
private:
	bool LoadProperties(const std::string& l_file);
	void PauseAll(const StateType& l_state);
	void UnpauseAll(const StateType& l_state);

	sf::Sound* CreateSound(SoundID& l_id, const std::string& l_audioName);
	void SetUpSound(sf::Sound* l_snd, const SoundProps* l_props,
		bool l_loop = false, bool l_relative = false);
	void RecycleSound(const SoundID& l_id, sf::Sound* l_snd, const std::string& l_name);

	void Cleanup();

	Sounds m_audio;
	MusicContainer m_music;
	RecycledSounds m_recycled;
	SoundProperties m_properties;
	StateType m_currentState;

	SoundID m_lastID;
	unsigned int m_numSounds;
	float m_elapsed;

	AudioManager* m_audioManager;
};