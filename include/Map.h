#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <map>
#include <array>
#include <fstream>
#include <sstream>
#include "Utilities.h"
#include "SharedContext.h"
#include "BaseState.h"
#include "Character.h"

enum Sheet{ Tile_Size = 32, Sheet_Width = 1760, Sheet_Height = 1120 };

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
		// sf::IntRect tileBoundaries(m_id % (Sheet::Sheet_Width / Sheet::Tile_Size) * Sheet::Tile_Size,
		// 	m_id / (Sheet::Sheet_Height / Sheet::Tile_Size) * Sheet::Tile_Size,
		// 	Sheet::Tile_Size,Sheet::Tile_Size);

		int y = m_id / (Sheet::Sheet_Width / Sheet::Tile_Size);
		int x = m_id - (y * (Sheet::Sheet_Width / Sheet::Tile_Size));

		sf::IntRect tileBoundaries(x * Sheet::Tile_Size,
			y * Sheet::Tile_Size,
			Sheet::Tile_Size,Sheet::Tile_Size);
		
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
	// Other flags unique to each tile.
};

using TileMap = std::unordered_map<TileID,Tile*>;
using TileSet = std::unordered_map<TileID,TileInfo*>;

class Map{
public:
	Map(SharedContext* l_context, BaseState* l_currentState);
	~Map();

	Tile* GetTile(unsigned int l_x, unsigned int l_y);
	Tile* GetTileBackground(unsigned int l_x, unsigned int l_y);
	TileInfo* GetDefaultTile();

	float GetGravity()const;
	unsigned int GetTileSize()const;
	const sf::Vector2u& GetMapSize()const;
	const sf::Vector2f& GetPlayerStart()const;
	std::string GetMusicName();

	void LoadMap(const std::string& l_path);
	void LoadBackGround(const std::string& l_path);
	void LoadNext();

	void Update(float l_dT);
	void Draw();

	void removeTile(int x, int y);

private:
	// Method for converting 2D coordinates to 1D ints.
	unsigned int ConvertCoords(unsigned int l_x, unsigned int l_y);
	void LoadTiles(const std::string& l_path);
	void PurgeMap();
	void PurgeMapBackground();
	void PurgeTileSet();

	TileSet m_tileSet;
	TileMap m_tileMap;
	TileMap m_tileMapBackground;
	sf::Sprite m_background;
	std::vector<sf::Sprite> hearts;
	sf::Sprite hud;
	int m_characterHp;
	TileInfo m_defaultTile;
	sf::Vector2u m_maxMapSize;
	sf::Vector2f m_playerStart;
	unsigned int m_tileCount;
	unsigned int m_tileSetCount;
	float m_mapGravity;
	std::string m_nextMap;
	bool m_loadNextMap;
	std::string m_backgroundTexture;
	BaseState* m_currentState;
	SharedContext* m_context;

	std::string m_musicName;
	int m_totalEnemies;
};