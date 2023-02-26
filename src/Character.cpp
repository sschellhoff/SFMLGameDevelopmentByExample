#include "Character.h"
#include "EntityManager.h"
#include "StateManager.h"

Character::Character(EntityManager* l_entityMgr)
	:EntityBase(l_entityMgr), 
	m_spriteSheet(m_entityManager->GetContext()->m_textureManager),
	m_jumpVelocity(250), m_hitpoints(5), m_currentHitpoints(5){ m_name = "Character"; }

Character::~Character(){ }

void Character::Move(const Direction& l_dir){
	if (GetState() == EntityState::Dying){ return; }
	m_spriteSheet.SetDirection(l_dir);
	if (l_dir == Direction::Left){ Accelerate(-m_speed.x, 0); }
	else { Accelerate(m_speed.x, 0); }
	if (GetState() == EntityState::Idle){ SetState(EntityState::Walking); }
}

void Character::Jump(){
	if (GetState() == EntityState::Dying || GetState() == EntityState::Jumping || GetState() == EntityState::Hurt){ return; }
	SetState(EntityState::Jumping);
	AddVelocity(0, -m_jumpVelocity);
}

void Character::Attack(){
	if (GetState() == EntityState::Dying || GetState() == EntityState::Jumping ||
		GetState() == EntityState::Hurt || GetState() == EntityState::Attacking)
	{ return; }
	SetState(EntityState::Attacking);
	
}

void Character::GetHurt(const int& l_damage){
	if (GetState() == EntityState::Dying || GetState() == EntityState::Hurt){ return; }
	m_currentHitpoints = (m_currentHitpoints - l_damage > 0 ? m_currentHitpoints - l_damage : 0);
	if(m_type == EntityType::Player)
		m_entityManager->GetContext()->m_characterCurrentHealth = m_currentHitpoints;
	if (m_currentHitpoints){ SetState(EntityState::Hurt); }
	else { 
		SetState(EntityState::Dying);
	}
}

int Character::GetHitpoints(){ return m_hitpoints;}

void Character::Load(const std::string& l_path){
	std::ifstream file;
	file.open(Utils::GetResourceDirectory() + std::string("media/Characters/") + l_path);
	if (!file.is_open()){ std::cout << "! Failed loading the character file: " << l_path << std::endl; return; }
	std::string line;
	while(std::getline(file,line)){
		if (line[0] == '|'){ continue; }
		std::stringstream keystream(line);
		std::string type;
		keystream >> type;
		if(type == "Name"){
			keystream >> m_name;
		} else if(type == "Spritesheet"){
			std::string path;
			keystream >> path;
			m_spriteSheet.LoadSheet("media/Spritesheets/" + path);
		} else if(type == "Hitpoints"){
			keystream >> m_hitpoints;
			m_currentHitpoints = m_hitpoints;
		} else if(type == "BoundingBox"){
			sf::Vector2f boundingSize;
			keystream >> boundingSize.x >> boundingSize.y;
			SetSize(boundingSize.x, boundingSize.y);
		} else if(type == "DamageBox"){
			keystream >> m_attackAABBoffset.x >> m_attackAABBoffset.y 
				>> m_attackAABB.width >> m_attackAABB.height;
		} else if(type == "Speed"){
			keystream >> m_speed.x >> m_speed.y;
		} else if(type == "JumpVelocity"){
			keystream >> m_jumpVelocity;
		} else if(type == "MaxVelocity"){
			keystream >> m_maxVelocity.x >> m_maxVelocity.y;
		} else if(type == "DeathSound"){
			std::string deathSound;
			keystream >> deathSound;
			m_bufferDie.loadFromFile(Utils::GetResourceDirectory() + deathSound);
			m_soundDie.setBuffer(m_bufferDie);
		} else if(type == "AttackSound"){
			std::string attackSound;
			keystream >> attackSound;
			m_bufferAttack.loadFromFile(Utils::GetResourceDirectory() + attackSound);
			m_soundAttack.setBuffer(m_bufferAttack);
		} else if(type == "HurtSound"){
			std::string hurtSound;
			keystream >> hurtSound;
			m_bufferHurt.loadFromFile(Utils::GetResourceDirectory() + hurtSound);
			m_soundHurt.setBuffer(m_bufferHurt);
		} else {
			std::cout << "! Unknown type in character file: " << type << std::endl;
		}
	}
	file.close();

}

void Character::UpdateAttackAABB(){
	m_attackAABB.left = 
		(m_spriteSheet.GetDirection() == Direction::Left ? 
		(m_AABB.left - m_attackAABB.width) - m_attackAABBoffset.x
		: (m_AABB.left + m_AABB.width) + m_attackAABBoffset.x);
	m_attackAABB.top = m_AABB.top + m_attackAABBoffset.y;
}

void Character::Animate(){
	EntityState state = GetState();

	if(state == EntityState::Walking && m_spriteSheet.
		GetCurrentAnim()->GetName() != "Walk")
	{
		m_spriteSheet.SetAnimation("Walk",true,true);
	} 
	else if(state == EntityState::Jumping && m_spriteSheet.
		GetCurrentAnim()->GetName() != "Jump")
	{
		m_spriteSheet.SetAnimation("Jump",true,false);
	}
	else if(state == EntityState::Attacking && m_spriteSheet.
		GetCurrentAnim()->GetName() != "Attack")
	{
		m_spriteSheet.SetAnimation("Attack",true,false);
	} else if(state == EntityState::Hurt && m_spriteSheet.
		GetCurrentAnim()->GetName() != "Hurt")
	{
		m_spriteSheet.SetAnimation("Hurt",true,false);
	}
	else if(state == EntityState::Dying && m_spriteSheet.
		GetCurrentAnim()->GetName() != "Death")
	{
		m_spriteSheet.SetAnimation("Death",true,false);
	}
	else if(state == EntityState::Idle && m_spriteSheet.
		GetCurrentAnim()->GetName() != "Idle")
	{
		m_spriteSheet.SetAnimation("Idle",true,true);
	}
}

void Character::Update(float l_dT){
	EntityBase::Update(l_dT);
	if(m_attackAABB.width != 0 && m_attackAABB.height != 0){
		UpdateAttackAABB();

		// Debug.
		if(m_entityManager->GetContext()->m_debugOverlay.Debug()){
			sf::RectangleShape* arect = new sf::RectangleShape(sf::Vector2f(m_attackAABB.width,m_attackAABB.height));
			arect->setPosition(m_attackAABB.left,m_attackAABB.top);
			arect->setFillColor(sf::Color(255,0,0,
				(m_state == EntityState::Attacking && m_spriteSheet.GetCurrentAnim()->IsInAction() 
				? 200 : 100)));
			m_entityManager->GetContext()->m_debugOverlay.Add(arect);
		}
		// End debug.
	}
	if(GetState() != EntityState::Dying && GetState() != EntityState::Attacking && GetState() != EntityState::Hurt){
		if(abs(m_velocity.y) >= 0.001f){
			SetState(EntityState::Jumping);
		} else if(abs(m_velocity.x) >= 0.1f){
			SetState(EntityState::Walking);
		} else {
			SetState(EntityState::Idle);
		}
	} else if(GetState() == EntityState::Attacking || GetState() == EntityState::Hurt){
		if(!m_spriteSheet.GetCurrentAnim()->IsPlaying()){
			SetState(EntityState::Idle);
		}
	} else if(GetState() == EntityState::Dying){
		if(!m_spriteSheet.GetCurrentAnim()->IsPlaying()){
			m_entityManager->Remove(m_id);
		}
	}

	Animate();

	m_spriteSheet.Update(l_dT);
	m_spriteSheet.SetSpritePosition(m_position);
}

void Character::Draw(sf::RenderWindow* l_wind){
	m_spriteSheet.Draw(l_wind);
}