#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <map>
#include <fstream>
#include <sstream>
#include "Utilities.h"
#include "Server_Entity_Manager.h"

enum Sheet{ Tile_Size = 32, Sheet_Width = 256, Sheet_Height = 256, Num_Layers = 4 };
using TileID = unsigned int;

struct TileInfo{
	TileInfo(TileID l_id = 0)
	{
		m_deadly = false;
		m_id = l_id;
	}

	~TileInfo(){}

	unsigned int m_id;
	std::string m_name;
	sf::Vector2f m_friction;
	bool m_deadly;
};

struct Tile{
	TileInfo* m_properties;
	bool m_warp; // Is the tile a warp.
	bool m_solid; // Is the tile a solid.
};

using TileMap = std::unordered_map<TileID, Tile*>;
using TileSet = std::unordered_map<TileID, TileInfo*>;

class Map{
public:
	Map(ServerEntityManager* l_entityMgr);
	~Map();

	Tile* GetTile(unsigned int l_x, unsigned int l_y, unsigned int l_layer);
	TileInfo* GetDefaultTile();

	unsigned int GetTileSize();
	const sf::Vector2u& GetMapSize();
	const sf::Vector2f& GetPlayerStart();

	void LoadMap(const std::string& l_path);
	void Update(float l_dT);
private:
	// Method for converting 2D coordinates to 1D ints.
	unsigned int ConvertCoords(unsigned int l_x, unsigned int l_y, unsigned int l_layer);
	void LoadTiles(const std::string& l_path);
	void PurgeMap();
	void PurgeTileSet();
	TileSet m_tileSet;
	TileMap m_tileMap;
	TileInfo m_defaultTile;
	sf::Vector2u m_maxMapSize;
	sf::Vector2f m_playerStart;
	unsigned int m_tileCount;
	unsigned int m_tileSetCount;
	ServerEntityManager* m_entityManager;
};