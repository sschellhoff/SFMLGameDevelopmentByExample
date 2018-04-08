#pragma once
#include "C_Base.h"

class C_SoundListener : public C_Base{
public:
	C_SoundListener() : C_Base(Component::SoundListener){}
	void ReadIn(std::stringstream& l_stream){}
private:

};