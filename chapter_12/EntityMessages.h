#pragma once

enum class EntityMessage{ 
	Move, Is_Moving, Frame_Change, State_Changed,
	Direction_Changed, Switch_State, 
	Attack_Action, Dead
};