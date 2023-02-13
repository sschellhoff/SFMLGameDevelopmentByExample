#pragma once
#include "EntityBase.h"
#include "SpriteSheet.h"
#include <SFML/Audio.hpp>

class Character : public EntityBase{
friend class EntityManager;
public:
	Character(EntityManager* l_entityMgr);
	virtual ~Character();

	void Move(const Direction& l_dir);

	void Jump();
	void Attack();
	void GetHurt(const int& l_damage);

	void Load(const std::string& l_path);

	virtual void OnEntityCollision(
		EntityBase* l_collider, bool l_attack) = 0;

	virtual void Update(float l_dT);
	void Draw(sf::RenderWindow* l_wind);

	int GetHitpoints();

protected:
	void UpdateAttackAABB();
	void Animate();
	SpriteSheet m_spriteSheet;
	float m_jumpVelocity;

	int m_hitpoints;
	int m_currentHitpoints;

	sf::FloatRect m_attackAABB;
	sf::Vector2f m_attackAABBoffset;
	
};