#pragma once
#include <string>
class SpriteSheet;

using Frame = unsigned int;

class Anim_Base{
	friend class SpriteSheet;
public:
	Anim_Base();
	virtual ~Anim_Base();

	void SetSpriteSheet(SpriteSheet* l_sheet);
	void SetFrame(Frame l_frame);
	void SetStartFrame(Frame l_frame);
	void SetEndFrame(Frame l_frame);
	void SetFrameRow(Frame l_row);
	void SetActionStart(Frame l_frame);
	void SetActionEnd(Frame l_frame);
	void SetFrameTime(float l_time);
	void SetLooping(bool l_loop);
	void SetName(const std::string& l_name);

	SpriteSheet* GetSpriteSheet();
	Frame GetFrame();
	Frame GetStartFrame();
	Frame GetEndFrame();
	Frame GetFrameRow();
	int GetActionStart();
	int GetActionEnd();
	float GetFrameTime();
	float GetElapsedTime();
	bool IsLooping();
	bool IsPlaying();
	bool IsInAction();
	std::string GetName();

	void Play();
	void Pause();
	void Stop();
	void Reset();

	virtual void Update(const float& l_dT);

	friend std::stringstream& operator >>(
		std::stringstream& l_stream, Anim_Base& a)
	{
		a.ReadIn(l_stream);
		return l_stream;
	}
protected:
	virtual void FrameStep() = 0;
	virtual void CropSprite() = 0;
	virtual void ReadIn(std::stringstream& l_stream) = 0;

	Frame m_frameCurrent;
	Frame m_frameStart;
	Frame m_frameEnd;
	Frame m_frameRow;
	int m_frameActionStart;
	int m_frameActionEnd;
	float m_frameTime;
	float m_elapsedTime;
	bool m_loop;
	bool m_playing;

	std::string m_name;

	SpriteSheet* m_spriteSheet;
};