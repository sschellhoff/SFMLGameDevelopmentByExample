#include "Map.h"

Map::Map(ServerEntityManager* l_entityMgr): m_entityManager(l_entityMgr), m_maxMapSize(32,32){
	LoadTiles("tiles.cfg");
}

Map::~Map(){
	PurgeMap();
	PurgeTileSet();
}

Tile* Map::GetTile(unsigned int l_x, unsigned int l_y, unsigned int l_layer)
{
	if (l_x < 0 || l_y < 0 || l_x >= m_maxMapSize.x || l_y >= m_maxMapSize.y || l_layer < 0 || l_layer >= Sheet::Num_Layers)
	{
		return nullptr;
	}

	auto itr = m_tileMap.find(ConvertCoords(l_x, l_y, l_layer));
	if (itr == m_tileMap.end()){ return nullptr; }
	return itr->second;
}

TileInfo* Map::GetDefaultTile(){ return &m_defaultTile; }
unsigned int Map::GetTileSize(){ return Sheet::Tile_Size; }
const sf::Vector2u& Map::GetMapSize(){ return m_maxMapSize; }
const sf::Vector2f& Map::GetPlayerStart(){ return m_playerStart; }

void Map::LoadMap(const std::string& l_path){
	std::ifstream mapFile;
	mapFile.open(Utils::GetWorkingDirectory() + l_path);
	if (!mapFile.is_open()){ std::cout << "! Failed loading map file: " << l_path << std::endl; return; }
	std::cout << "--- Loading a map: " << l_path << std::endl;
	std::string line;
	while(std::getline(mapFile,line)){
		if (line[0] == '|'){ continue; }
		std::stringstream keystream(line);
		std::string type;
		keystream >> type;
		if(type == "TILE"){
			int tileId = 0;
			keystream >> tileId;
			if (tileId < 0){ std::cout << "! Bad tile id: " << tileId << std::endl; continue; }
			auto itr = m_tileSet.find(tileId);
			if (itr == m_tileSet.end()){ std::cout << "! Tile id(" << tileId << ") was not found in tileset." << std::endl; continue; }
			sf::Vector2i tileCoords;
			unsigned int tileLayer = 0;
			unsigned int tileSolidity = 0;
			keystream >> tileCoords.x >> tileCoords.y
				>> tileLayer >> tileSolidity;
			if (tileCoords.x > m_maxMapSize.x ||
				tileCoords.y > m_maxMapSize.y || tileLayer >= Sheet::Num_Layers)
			{
				std::cout << "! Tile is out of range: "
					<< tileCoords.x << " " << tileCoords.y << std::endl;
				continue;
			}
			Tile* tile = new Tile();
			// Bind properties of a tile from a set.
			tile->m_properties = itr->second;
			tile->m_solid = (bool)tileSolidity;
			if(!m_tileMap.emplace(ConvertCoords(tileCoords.x,tileCoords.y,tileLayer),tile).second)
			{
				// Duplicate tile detected!
				std::cout << "! Duplicate tile! : " << tileCoords.x 
					<< " " << tileCoords.y << std::endl;
				delete tile;
				tile = nullptr;
				continue;
			}
			std::string warp;
			keystream >> warp;
			tile->m_warp = false;
			if(warp == "WARP"){ tile->m_warp = true; }
		} else if(type == "SIZE"){
			keystream >> m_maxMapSize.x >> m_maxMapSize.y;
		} else if(type == "DEFAULT_FRICTION"){
			keystream >> m_defaultTile.m_friction.x >> m_defaultTile.m_friction.y;
		} else if(type == "ENTITY"){
			// Set up entity here.
			std::string name;
			keystream >> name;
			int entityId = m_entityManager->AddEntity(name);
			if (entityId < 0){ continue; }
			C_Base* position = nullptr;
			if (position = m_entityManager->GetComponent<C_Position>(entityId, Component::Position)){ keystream >> *position; }
		} else {
			// Something else.
			std::cout << "! Unknown type \"" << type << "\"." << std::endl;
		}
	}
	mapFile.close();
	std::cout << "--- Map Loaded! ---" << std::endl;
}

void Map::LoadTiles(const std::string& l_path){
	std::ifstream tileSetFile;
	tileSetFile.open(Utils::GetWorkingDirectory() + l_path);
	if (!tileSetFile.is_open()){ std::cout << "! Failed loading tile set file: " << l_path << std::endl; return; }
	std::string line;
	while(std::getline(tileSetFile,line)){
		if (line[0] == '|'){ continue; }
		std::stringstream keystream(line);
		int tileId;
		keystream >> tileId;
		if (tileId < 0){ continue; }
		TileInfo* tile = new TileInfo(tileId);
		keystream >> tile->m_name >> tile->m_friction.x >> tile->m_friction.y >> tile->m_deadly;
		if(!m_tileSet.emplace(tileId,tile).second){
			// Duplicate tile detected!
			std::cout << "! Duplicate tile type: " << tile->m_name << std::endl;
			delete tile;
		}
	}
	tileSetFile.close();
}

void Map::Update(float l_dT){}

unsigned int Map::ConvertCoords(unsigned int l_x, unsigned int l_y, unsigned int l_layer){
	return ((l_layer*m_maxMapSize.y+l_y) * m_maxMapSize.x + l_x);
}

void Map::PurgeMap(){
	while(m_tileMap.begin() != m_tileMap.end()){
		delete m_tileMap.begin()->second;
		m_tileMap.erase(m_tileMap.begin());
	}
	m_tileCount = 0;
	m_entityManager->Purge();
}

void Map::PurgeTileSet(){
	while(m_tileSet.begin() != m_tileSet.end()){
		delete m_tileSet.begin()->second;
		m_tileSet.erase(m_tileSet.begin());
	}
	m_tileSetCount = 0;
}