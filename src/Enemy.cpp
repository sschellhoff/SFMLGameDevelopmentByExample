#include "Enemy.h"

Enemy::Enemy(EntityManager* l_entityMgr)
	:Character(l_entityMgr), m_hasDestination(false){
		m_type = EntityType::Enemy;
	}

Enemy::~Enemy(){}

void Enemy::OnEntityCollision(EntityBase* l_collider, bool l_attack)
{
	if (m_state == EntityState::Dying){ return; }
	if (l_attack){ return; }
	if (l_collider->GetType() != EntityType::Player){ return; }
	Character* player = (Character*)l_collider;
	SetState(EntityState::Attacking);
	player->GetHurt(1);
	if(m_position.x > player->GetPosition().x){
		player->AddVelocity(-m_speed.x,0);
		m_spriteSheet.SetDirection(Direction::Left);
	} else {
		player->AddVelocity(m_speed.y,0);
		m_spriteSheet.SetDirection(Direction::Right);
	}
}

void Enemy::Update(float l_dT){
	Character::Update(l_dT);

	if (m_hasDestination){
		if (abs(m_destination.x - m_position.x) < 16){ m_hasDestination = false; return; }
		if (m_destination.x - m_position.x > 0){ Move(Direction::Right); } else { Move(Direction::Left); }
		if (m_collidingOnX){ m_hasDestination = false; }
		return;
	}
	int random = rand() % 100 + 1;
	if (random != 100){ return; }
	int newX = rand() % 65 + 0;
	if (rand() % 2){ newX = -newX; }
	m_destination.x = m_position.x + newX;
	if (m_destination.x < 0){ m_destination.x = 0; }
	m_hasDestination = true;
}