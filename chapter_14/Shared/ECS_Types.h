#pragma once
using ComponentType = unsigned int;
#define N_COMPONENT_TYPES 32

enum class Component{
	Position = 0, SpriteSheet, State, Movable, Controller, Collidable, SoundEmitter, SoundListener,
	Client, Health, Name, UI_Element, Attacker
};

enum class System{
	Renderer = 0, Movement, Collision, Control, State, SheetAnimation, Sound, Network, Combat,
	Character_UI, Timers
};