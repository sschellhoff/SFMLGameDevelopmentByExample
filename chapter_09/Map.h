#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <map>
#include <fstream>
#include <sstream>
#include "Utilities.h"
#include "SharedContext.h"

enum Sheet{ Tile_Size = 32, Sheet_Width = 256, Sheet_Height = 256, Num_Layers = 4 };
using TileID = unsigned int;

struct TileInfo{
	TileInfo(SharedContext* l_context,
		const std::string& l_texture = "", TileID l_id = 0)
		: m_context(l_context), m_id(0), m_deadly(false)
	{
		TextureManager* tmgr = l_context->m_textureManager;
		if (l_texture == ""){ m_id = l_id; return; }
		if (!tmgr->RequireResource(l_texture)){ return; }
		m_texture = l_texture;
		m_id = l_id;
		m_sprite.setTexture(*tmgr->GetResource(m_texture));
		sf::IntRect tileBoundaries(m_id % (Sheet::Sheet_Width / Sheet::Tile_Size) * Sheet::Tile_Size,
			m_id / (Sheet::Sheet_Height / Sheet::Tile_Size) * Sheet::Tile_Size,
			Sheet::Tile_Size, Sheet::Tile_Size);
		m_sprite.setTextureRect(tileBoundaries);
	}

	~TileInfo(){
		if (m_texture == ""){ return; }
		m_context->m_textureManager->ReleaseResource(m_texture);
	}

	sf::Sprite m_sprite;

	TileID m_id;
	std::string m_name;
	sf::Vector2f m_friction;
	bool m_deadly;

	SharedContext* m_context;
	std::string m_texture;
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
	Map(SharedContext* l_context);
	~Map();

	Tile* GetTile(unsigned int l_x, unsigned int l_y, unsigned int l_layer);
	TileInfo* GetDefaultTile();

	unsigned int GetTileSize()const;
	const sf::Vector2u& GetMapSize()const;
	const sf::Vector2f& GetPlayerStart()const;
	int GetPlayerId()const;

	void LoadMap(const std::string& l_path);

	void Update(float l_dT);
	void Draw(unsigned int l_layer);
private:
	// Method for converting 2D coordinates to 1D ints.
	unsigned int ConvertCoords(unsigned int l_x, unsigned int l_y, unsigned int l_layer)const;

	void LoadTiles(const std::string& l_path);

	void PurgeMap();
	void PurgeTileSet();

	TileSet m_tileSet;
	TileMap m_tileMap;

	TileInfo m_defaultTile;

	sf::Vector2u m_maxMapSize;
	sf::Vector2f m_playerStart;
	int m_playerId;
	unsigned int m_tileCount;
	unsigned int m_tileSetCount;
	SharedContext* m_context;
};