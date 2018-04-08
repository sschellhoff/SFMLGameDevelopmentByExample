#include "S_Collision.h"
#include "System_Manager.h"
#include "Map.h"

S_Collision::S_Collision(SystemManager* l_systemMgr)
	: S_Base(System::Collision, l_systemMgr)
{
	Bitmask req;
	req.TurnOnBit((unsigned int)Component::Position);
	req.TurnOnBit((unsigned int)Component::Collidable);
	m_requiredComponents.push_back(req);
	req.Clear();

	m_gameMap = nullptr;
}

S_Collision::~S_Collision(){}

void S_Collision::SetMap(Map* l_map){ m_gameMap = l_map; }

void S_Collision::Update(float l_dT){
	if (!m_gameMap){ return; }
	EntityManager* entities = m_systemManager->GetEntityManager();
	for (auto &entity : m_entities){
		C_Position* position = entities->GetComponent<C_Position>(entity, Component::Position);
		C_Collidable* collidable = entities->GetComponent<C_Collidable>(entity, Component::Collidable);

		collidable->SetPosition(position->GetPosition());
		collidable->ResetCollisionFlags();
		CheckOutOfBounds(position, collidable);
		MapCollisions(entity, position, collidable);
	}
	EntityCollisions();
}

void S_Collision::EntityCollisions(){
	EntityManager* entities = m_systemManager->GetEntityManager();
	for (auto itr = m_entities.begin(); itr != m_entities.end(); ++itr)
	{
		for (auto itr2 = std::next(itr); itr2 != m_entities.end(); ++itr2){
			C_Collidable* collidable1 = entities->GetComponent<C_Collidable>(*itr, Component::Collidable);
			C_Collidable* collidable2 = entities->GetComponent<C_Collidable>(*itr2, Component::Collidable);
			if (collidable1->GetCollidable().intersects(collidable2->GetCollidable()))
			{
				// Entity-on-entity collision!
			}
		}
	}
}

void S_Collision::CheckOutOfBounds(C_Position* l_pos, C_Collidable* l_col){
	unsigned int TileSize = m_gameMap->GetTileSize();

	if (l_pos->GetPosition().x < 0){
		l_pos->SetPosition(0.0f, l_pos->GetPosition().y);
		l_col->SetPosition(l_pos->GetPosition());
	} else if (l_pos->GetPosition().x > m_gameMap->GetMapSize().x * TileSize){
		l_pos->SetPosition(m_gameMap->GetMapSize().x * TileSize, l_pos->GetPosition().y);
		l_col->SetPosition(l_pos->GetPosition());
	}

	if (l_pos->GetPosition().y < 0){
		l_pos->SetPosition(l_pos->GetPosition().x, 0.0f);
		l_col->SetPosition(l_pos->GetPosition());
	} else if (l_pos->GetPosition().y > m_gameMap->GetMapSize().y * TileSize){
		l_pos->SetPosition(l_pos->GetPosition().x, m_gameMap->GetMapSize().y * TileSize);
		l_col->SetPosition(l_pos->GetPosition());
	}
}

void S_Collision::MapCollisions(const EntityId& l_entity, C_Position* l_pos, C_Collidable* l_col){
	unsigned int TileSize = m_gameMap->GetTileSize();
	Collisions c;

	sf::FloatRect EntityAABB = l_col->GetCollidable();
	int FromX = floor(EntityAABB.left / TileSize);
	int ToX = floor((EntityAABB.left + EntityAABB.width) / TileSize);
	int FromY = floor(EntityAABB.top / TileSize);
	int ToY = floor((EntityAABB.top + EntityAABB.height) / TileSize);

	for (int x = FromX; x <= ToX; ++x){
		for (int y = FromY; y <= ToY; ++y){
			for (int l = 0; l < Sheet::Num_Layers; ++l){
				Tile* t = m_gameMap->GetTile(x, y, l);
				if (!t){ continue; }
				if (!t->m_solid){ continue; }
				sf::FloatRect TileAABB(x*TileSize, y*TileSize, TileSize, TileSize);
				sf::FloatRect Intersection;
				EntityAABB.intersects(TileAABB, Intersection);
				float S = Intersection.width * Intersection.height;
				c.emplace_back(S, t->m_properties, TileAABB);
				break;
			}
		}
	}

	if (c.empty()){ return; }
	std::sort(c.begin(), c.end(), [](CollisionElement& l_1, CollisionElement& l_2){
		return l_1.m_area > l_2.m_area;
	});

	for (auto &col : c){
		EntityAABB = l_col->GetCollidable();
		if (!EntityAABB.intersects(col.m_tileBounds)){ continue; }
		float xDiff = (EntityAABB.left + (EntityAABB.width / 2)) - (col.m_tileBounds.left + (col.m_tileBounds.width / 2));
		float yDiff = (EntityAABB.top + (EntityAABB.height / 2)) - (col.m_tileBounds.top + (col.m_tileBounds.height / 2));
		float resolve = 0;
		if (std::abs(xDiff) > std::abs(yDiff)){
			if (xDiff > 0){
				resolve = (col.m_tileBounds.left + TileSize) - EntityAABB.left;
			} else {
				resolve = -((EntityAABB.left + EntityAABB.width) - col.m_tileBounds.left);
			}
			l_pos->MoveBy(resolve, 0);
			l_col->SetPosition(l_pos->GetPosition());
			m_systemManager->AddEvent(l_entity, (EventID)EntityEvent::Colliding_X);
			l_col->CollideOnX();
		} else {
			if (yDiff > 0){
				resolve = (col.m_tileBounds.top + TileSize) - EntityAABB.top;
			} else {
				resolve = -((EntityAABB.top + EntityAABB.height) - col.m_tileBounds.top);
			}
			l_pos->MoveBy(0, resolve);
			l_col->SetPosition(l_pos->GetPosition());
			m_systemManager->AddEvent(l_entity, (EventID)EntityEvent::Colliding_Y);
			l_col->CollideOnY();
		}
	}
}

void S_Collision::HandleEvent(const EntityId& l_entity,const EntityEvent& l_event){}
void S_Collision::Notify(const Message& l_message){}