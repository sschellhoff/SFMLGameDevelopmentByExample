#include "S_CharacterUI.h"
#include "System_Manager.h"

S_CharacterUI::S_CharacterUI(SystemManager* l_systemMgr)
	: S_Base(System::Character_UI, l_systemMgr)
{
	Bitmask req;
	req.TurnOnBit((unsigned int)Component::Position);
	req.TurnOnBit((unsigned int)Component::UI_Element);
	req.TurnOnBit((unsigned int)Component::Health);
	m_requiredComponents.push_back(req);
	req.ClearBit((unsigned int)Component::Health);
	req.TurnOnBit((unsigned int)Component::Name);
	m_requiredComponents.push_back(req);

	ClientSystemManager* mgr = (ClientSystemManager*)m_systemManager;
	mgr->GetTextureManager()->RequireResource("HeartBar");
	mgr->GetFontManager()->RequireResource("Main");
	sf::Texture* txtr = mgr->GetTextureManager()->GetResource("HeartBar");
	txtr->setRepeated(true);
	m_heartBarSize = txtr->getSize();
	m_heartBar.setTexture(*txtr);
	m_heartBar.setScale(0.5f, 0.5f);
	m_heartBar.setOrigin(m_heartBarSize.x / 2, m_heartBarSize.y);
	m_nickname.setFont(*mgr->GetFontManager()->GetResource("Main"));
	m_nickname.setCharacterSize(9);
	m_nickname.setColor(sf::Color::White);
	m_nickbg.setFillColor(sf::Color(100, 100, 100, 100));
}

S_CharacterUI::~S_CharacterUI(){
	ClientSystemManager* mgr = (ClientSystemManager*)m_systemManager;
	mgr->GetTextureManager()->ReleaseResource("HeartBar");
	mgr->GetFontManager()->ReleaseResource("Main");
}

void S_CharacterUI::Update(float l_dT){}
void S_CharacterUI::HandleEvent(const EntityId& l_entity, const EntityEvent& l_event){}
void S_CharacterUI::Notify(const Message& l_message){}

void S_CharacterUI::Render(Window* l_wind)
{
	EntityManager* entities = m_systemManager->GetEntityManager();
	for (auto &entity : m_entities){
		C_Health* health = entities->GetComponent<C_Health>(entity, Component::Health);
		C_Name* name = entities->GetComponent<C_Name>(entity, Component::Name);
		C_Position* pos = entities->GetComponent<C_Position>(entity, Component::Position);
		C_UI_Element* ui = entities->GetComponent<C_UI_Element>(entity, Component::UI_Element);
		if (health){
			m_heartBar.setTextureRect(sf::IntRect(0, 0, m_heartBarSize.x * health->GetHealth(), m_heartBarSize.y));
			m_heartBar.setOrigin((m_heartBarSize.x * health->GetHealth()) / 2, m_heartBarSize.y);
			m_heartBar.setPosition(pos->GetPosition() + ui->GetOffset());
			l_wind->GetRenderWindow()->draw(m_heartBar);
		}
		if (name){
			m_nickname.setString(name->GetName());
			m_nickname.setOrigin(m_nickname.getLocalBounds().width / 2, m_nickname.getLocalBounds().height / 2);
			if (health){
				m_nickname.setPosition(m_heartBar.getPosition().x, m_heartBar.getPosition().y - (m_heartBarSize.y));
			} else {
				m_nickname.setPosition(pos->GetPosition() + ui->GetOffset());
			}
			m_nickbg.setSize(sf::Vector2f(m_nickname.getGlobalBounds().width + 2, m_nickname.getCharacterSize() + 1));
			m_nickbg.setOrigin(m_nickbg.getSize().x / 2, m_nickbg.getSize().y / 2);
			m_nickbg.setPosition(m_nickname.getPosition().x + 1, m_nickname.getPosition().y + 1);
			l_wind->GetRenderWindow()->draw(m_nickbg);
			l_wind->GetRenderWindow()->draw(m_nickname);
		}
	}
}