#include "Map.h"
#include "StateManager.h"
#include <cmath>

Map::Map(SharedContext* l_context, BaseState* l_currentState)
	:m_context(l_context), m_defaultTile(l_context), m_maxMapSize(32, 32),
	m_tileCount(0), m_tileSetCount(0), m_mapGravity(512.f), m_loadNextMap(false),
	m_currentState(l_currentState)
{
	m_context->m_gameMap = this;
	LoadTiles("tiles.cfg");
}

Map::~Map(){
	PurgeMap();
	PurgeTileSet();
	m_context->m_gameMap = nullptr;
}

Tile* Map::GetTile(unsigned int l_x, unsigned int l_y){
	auto itr = m_tileMap.find(ConvertCoords(l_x,l_y));
	return(itr != m_tileMap.end() ? itr->second : nullptr);
}
TileInfo* Map::GetDefaultTile(){ return &m_defaultTile; }
float Map::GetGravity()const{ return m_mapGravity; }
unsigned int Map::GetTileSize()const{ return Sheet::Tile_Size; }
const sf::Vector2u& Map::GetMapSize()const{ return m_maxMapSize; }
const sf::Vector2f& Map::GetPlayerStart()const{ return m_playerStart; }

void Map::LoadMap(const std::string& l_path){
	std::ifstream mapFile;
	mapFile.open(Utils::GetResourceDirectory() + l_path);
	if (!mapFile.is_open()){ std::cout << "! Failed loading map file: " << l_path << std::endl; return; }
	EntityManager* entityMgr = m_context->m_entityManager;
	std::string line;
	std::cout << "--- Loading a map: " << l_path << std::endl;

	int playerId = -1;
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
			keystream >> tileCoords.x >> tileCoords.y;
			if (tileCoords.x > m_maxMapSize.x || tileCoords.y > m_maxMapSize.y){
				std::cout << "! Tile is out of range: "<< tileCoords.x << " " << tileCoords.y << std::endl;
				continue;
			}
			Tile* tile = new Tile();
			// Bind properties of a tile from a set.
			tile->m_properties = itr->second;
			if(!m_tileMap.emplace(ConvertCoords(tileCoords.x,tileCoords.y),tile).second)
			{
				// Duplicate tile detected!
				std::cout << "! Duplicate tile! : " << tileCoords.x 
					<< "" << tileCoords.y << std::endl;
				delete tile;
				tile = nullptr;
				continue;
			}
			std::string warp;
			keystream >> warp;
			tile->m_warp = false;
			if(warp == "WARP"){ tile->m_warp = true; }
		} else if(type == "BACKGROUND"){
			if (m_backgroundTexture != ""){ continue; }
			keystream >> m_backgroundTexture;
			if (!m_context->m_textureManager->RequireResource(m_backgroundTexture)){
				m_backgroundTexture = "";
				continue;
			}
			sf::Texture* texture = m_context->m_textureManager->GetResource(m_backgroundTexture);
			m_background.setTexture(*texture);
			sf::Vector2f viewSize = m_currentState->GetView().getSize();
			sf::Vector2u textureSize = texture->getSize();
			sf::Vector2f scaleFactors;
			scaleFactors.x = viewSize.x / textureSize.x;
			scaleFactors.y = viewSize.y / textureSize.y;
			m_background.setScale(scaleFactors);
		} else if(type == "SIZE"){
			keystream >> m_maxMapSize.x >> m_maxMapSize.y;
		} else if(type == "GRAVITY"){
			keystream >> m_mapGravity;
		} else if(type == "DEFAULT_FRICTION"){
			keystream >> m_defaultTile.m_friction.x >> m_defaultTile.m_friction.y;
		} else if(type == "NEXTMAP"){
			keystream >> m_nextMap;
		} else if(type == "PLAYER"){
			if (playerId != -1){ continue; }
			// Set up the player position here.
			playerId = entityMgr->Add(EntityType::Player);
			if (playerId < 0){ continue; }
			float playerX = 0; float playerY = 0;
			keystream >> playerX >> playerY;
			entityMgr->Find(playerId)->SetPosition(playerX,playerY);
			m_playerStart = sf::Vector2f(playerX, playerY);
		} else if(type == "ENEMY"){
			std::string enemyName;
			keystream >> enemyName;
			int enemyId = entityMgr->Add(EntityType::Enemy, enemyName);
			if (enemyId < 0){ continue; }
			float enemyX = 0; float enemyY = 0;
			keystream >> enemyX >> enemyY;
			entityMgr->Find(enemyId)->SetPosition(enemyX, enemyY);
		} else {
			// Something else.
			std::cout << "! Unknown type \"" << type << "\"." << std::endl;
		}
	}
	mapFile.close();
	std::cout << "--- Map Loaded! ---" << std::endl;
}

void Map::LoadNext(){ m_loadNextMap = true; }

void Map::LoadTiles(const std::string& l_path){
	std::ifstream file;
	file.open(Utils::GetResourceDirectory() + l_path);
	if (!file.is_open()){ std::cout << "! Failed loading tile set file: " << l_path << std::endl; return; }
	std::string line;
	while(std::getline(file,line)){
		if (line[0] == '|'){ continue; }
		std::stringstream keystream(line);
		int tileId;
		keystream >> tileId;
		if (tileId < 0){ continue; }
		TileInfo* tile = new TileInfo(m_context,"TileSheet",tileId);
		keystream >> tile->m_name >> tile->m_friction.x >> tile->m_friction.y >> tile->m_deadly;
		if(!m_tileSet.emplace(tileId,tile).second){
			// Duplicate tile detected!
			std::cout << "! Duplicate tile type: " << tile->m_name << std::endl;
			delete tile;
		}
	}
	file.close();
}

void Map::Update(float l_dT){
	if(m_loadNextMap){
		PurgeMap();
		m_loadNextMap = false;
		if(m_nextMap != ""){
			LoadMap("media/Maps/"+m_nextMap);
		} else {
			m_currentState->GetStateManager()->SwitchTo(StateType::GameOver);
		}
		m_nextMap = "";
	}
	sf::FloatRect viewSpace = m_context->m_wind->GetViewSpace();
	m_background.setPosition(viewSpace.left, viewSpace.top);
}

void Map::Draw(){
	sf::RenderWindow* l_wind = m_context->m_wind->GetRenderWindow();
	l_wind->draw(m_background);
	sf::FloatRect viewSpace = m_context->m_wind->GetViewSpace();

	sf::Vector2i tileBegin(floor(viewSpace.left / Sheet::Tile_Size),floor(viewSpace.top / Sheet::Tile_Size));
	sf::Vector2i tileEnd(ceil((viewSpace.left + viewSpace.width) / Sheet::Tile_Size),
		ceil((viewSpace.top + viewSpace.height) / Sheet::Tile_Size));

	unsigned int count = 0;
	for(int x = tileBegin.x; x <= tileEnd.x; ++x){
		for(int y = tileBegin.y; y <= tileEnd.y; ++y){
			if(x < 0 || y < 0){ continue; }
			Tile* tile = GetTile(x,y);
			if (!tile){ continue; }
			sf::Sprite& sprite = tile->m_properties->m_sprite;
			sprite.setPosition(x * Sheet::Tile_Size, y * Sheet::Tile_Size);
			l_wind->draw(sprite);
			++count;

			// Debug.
			if(m_context->m_debugOverlay.Debug()){
				if(tile->m_properties->m_deadly || tile->m_warp){
					sf::RectangleShape* tileMarker = new sf::RectangleShape(
						sf::Vector2f(Sheet::Tile_Size,Sheet::Tile_Size));
					tileMarker->setPosition(x * Sheet::Tile_Size, y * Sheet::Tile_Size);
					if(tile->m_properties->m_deadly){
						tileMarker->setFillColor(sf::Color(255,0,0,100));
					} else if(tile->m_warp){
						tileMarker->setFillColor(sf::Color(0,255,0,150));
					}
					m_context->m_debugOverlay.Add(tileMarker);
				}
			}
			// End debug.
		}
	}
}

unsigned int Map::ConvertCoords(unsigned int l_x, unsigned int l_y){
	return (l_x * m_maxMapSize.x) + l_y; // Row-major.
}

void Map::PurgeMap(){
	m_tileCount = 0;
	for (auto &itr : m_tileMap){
		delete itr.second;
	}
	m_tileMap.clear();
	m_context->m_entityManager->Purge();

	if (m_backgroundTexture == ""){ return; }
	m_context->m_textureManager->ReleaseResource(m_backgroundTexture);
	m_backgroundTexture = "";
}

void Map::PurgeTileSet(){
	for (auto &itr : m_tileSet){
		delete itr.second;
	}
	m_tileSet.clear();
	m_tileSetCount = 0;
}