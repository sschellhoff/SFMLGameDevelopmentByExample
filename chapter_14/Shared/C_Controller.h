#pragma once
#include "C_Base.h"

class C_Controller : public C_Base{
public:
	C_Controller() : C_Base(Component::Controller){}
	void ReadIn(std::stringstream& l_stream){}
private:

};