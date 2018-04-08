#pragma once
#include "System_Manager.h"
#include "S_Movement.h"
#include "S_Collision.h"
#include "S_State.h"
#include "S_Control.h"
#include "S_Network.h"
#include "S_Combat.h"
#include "S_Timers.h"

class ServerSystemManager : public SystemManager{
public:
	ServerSystemManager();
	~ServerSystemManager();
private:
};