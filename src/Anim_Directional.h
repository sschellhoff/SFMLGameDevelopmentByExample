#pragma once
#include "Anim_Base.h"
#include "Directions.h"

class Anim_Directional : public Anim_Base{
protected:
	void FrameStep();
	void CropSprite();
	void ReadIn(std::stringstream& l_stream);
};