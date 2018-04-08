#include "Client_System_Manager.h"

ClientSystemManager::ClientSystemManager(TextureManager* l_textureMgr, FontManager* l_fontMgr)
	: m_textureMgr(l_textureMgr), m_fontMgr(l_fontMgr)
{
	AddSystem<S_State>(System::State);
	AddSystem<S_Control>(System::Control);
	AddSystem<S_Movement>(System::Movement);
	AddSystem<S_Collision>(System::Collision);
	AddSystem<S_SheetAnimation>(System::SheetAnimation);
	AddSystem<S_Network>(System::Network);
	AddSystem<S_Sound>(System::Sound);
	AddSystem<S_Renderer>(System::Renderer);
	AddSystem<S_CharacterUI>(System::Character_UI);
}
ClientSystemManager::~ClientSystemManager(){}

TextureManager* ClientSystemManager::GetTextureManager(){ return m_textureMgr; }
FontManager* ClientSystemManager::GetFontManager(){ return m_fontMgr; }

void ClientSystemManager::Draw(Window* l_wind, unsigned int l_elevation)
{
	auto itr = m_systems.find(System::Renderer);
	if(itr != m_systems.end()){
		S_Renderer* system = (S_Renderer*)itr->second;
		system->Render(l_wind, l_elevation);
	}
	itr = m_systems.find(System::Character_UI);
	if (itr != m_systems.end()){
		S_CharacterUI* ui = (S_CharacterUI*)itr->second;
		ui->Render(l_wind);
	}
}