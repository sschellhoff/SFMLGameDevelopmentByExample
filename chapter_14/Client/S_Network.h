#pragma once
#include "S_Base.h"
#include "EntitySnapshot.h"
#include "Client_Entity_Manager.h"
#include "EntityMessages.h"
#include "Client.h"
#include "S_Movement.h"
#include "S_State.h"
#include <unordered_map>
#include <map>
#include "NetSettings.h"

template<class T>
inline T Interpolate(const sf::Int32& T1, const sf::Int32& T2, const T& T1_val, const T& T2_val, const sf::Int32& T_X){
	return (((T2_val - T1_val) / (T2 - T1)) * (T_X - T1)) + T1_val;
}

void InterpolateSnapshot(const EntitySnapshot& l_s1, const sf::Int32& T1,
	const EntitySnapshot& l_s2, const sf::Int32& T2, EntitySnapshot& l_target, const sf::Int32& T_X);

bool CompareSnapshots(const EntitySnapshot& l_s1, const EntitySnapshot& l_s2, 
	bool l_position = true, bool l_physics = true, bool l_state = true);

using SnapshotMap = std::unordered_map<EntityId, EntitySnapshot>;
struct SnapshotDetails{
	SnapshotMap m_snapshots;
};
using SnapshotContainer = std::map<sf::Int32, SnapshotDetails>;
using OutgoingMessages = std::unordered_map<EntityMessage, std::vector<Message>>;

class S_Network : public S_Base{
public:
	S_Network(SystemManager* l_systemMgr);
	~S_Network();

	void Update(float l_dT);
	void HandleEvent(const EntityId& l_entity, const EntityEvent& l_event);
	void Notify(const Message& l_message);

	void SetClient(Client* m_client);
	void SetPlayerID(const EntityId& l_entity);

	void AddSnapshot(const EntityId& l_entity, 
		const sf::Int32& l_timestamp, 
		EntitySnapshot& l_snapshot);
	void SendPlayerOutgoing();
	void ClearSnapshots();
private:
	void ApplyEntitySnapshot(const EntityId& l_entity,
		const EntitySnapshot& l_snapshot,
		bool l_applyPhysics);

	void PerformInterpolation();
	SnapshotContainer m_entitySnapshots;
	EntityId m_player;
	OutgoingMessages m_outgoing;
	Client* m_client;
	sf::Time m_playerUpdateTimer;
};