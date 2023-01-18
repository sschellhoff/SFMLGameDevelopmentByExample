#pragma once
#include <string>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

enum class EntityType{ Base, Enemy, Player };

enum class EntityState{ Idle, Walking, Jumping, Attacking, Hurt, Dying };

struct TileInfo;

struct CollisionElement{
	CollisionElement(float l_area, TileInfo* l_info, const sf::FloatRect& l_bounds)
		:m_area(l_area), m_tile(l_info), m_tileBounds(l_bounds){}
	float m_area;
	TileInfo* m_tile;
	sf::FloatRect m_tileBounds;
};

using Collisions = std::vector<CollisionElement>;
bool SortCollisions(const CollisionElement& l_1, const CollisionElement& l_2);

class EntityManager;
class EntityBase{
friend class EntityManager;
public:
	EntityBase(EntityManager* l_entityMgr);
	virtual ~EntityBase();

	const sf::Vector2f& GetPosition()const;
	const sf::Vector2f& GetSize()const;
	EntityState GetState()const;
	std::string GetName()const;
	unsigned int GetId()const;
	EntityType GetType()const;

	void SetPosition(float l_x, float l_y);
	void SetPosition(const sf::Vector2f& l_pos);
	void SetSize(float l_x, float l_y);
	void SetState(const EntityState& l_state);

	void Move(float l_x, float l_y);
	void AddVelocity(float l_x, float l_y);
	void Accelerate(float l_x, float l_y);
	void SetAcceleration(float l_x, float l_y);
	void ApplyFriction(float l_x, float l_y);

	virtual void Update(float l_dT);
	virtual void Draw(sf::RenderWindow* l_wind) = 0;
protected:
	// Methods.
	void UpdateAABB();
	void CheckCollisions();
	void ResolveCollisions();

	// Method for what THIS entity does TO the l_collider entity.
	virtual void OnEntityCollision(EntityBase* l_collider, bool l_attack) = 0;

	// Data members.
	std::string m_name;
	EntityType m_type;
	unsigned int m_id; // Entity id in the entity manager.
	sf::Vector2f m_position; // Current position.
	sf::Vector2f m_positionOld; // Position before entity moved.
	sf::Vector2f m_velocity; // Current velocity.
	sf::Vector2f m_maxVelocity; // Maximum velocity.
	sf::Vector2f m_speed; // Value of acceleration.
	sf::Vector2f m_acceleration; // Current acceleration.
	sf::Vector2f m_friction; // Default friction value.
	TileInfo* m_referenceTile; // Tile underneath entity.
	sf::Vector2f m_size; // Size of the collision box.
	sf::FloatRect m_AABB; // The bounding box for collisions.
	EntityState m_state; // Current entity state.
	// Flags for remembering axis collisions.
	bool m_collidingOnX;
	bool m_collidingOnY;

	Collisions m_collisions;
	EntityManager* m_entityManager;
};