#pragma once
#include <string>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <sstream>
#include "Utilities.h"

template<typename Derived, typename T>
class ResourceManager{
public:
	ResourceManager(const std::string& l_pathsFile){
		LoadPaths(l_pathsFile);
	}

	virtual ~ResourceManager(){ PurgeResources(); }

	T* GetResource(const std::string& l_id){
		auto res = Find(l_id);
		return(res ? res->first : nullptr);
	}

	std::string GetPath(const std::string& l_id){
		auto path = m_paths.find(l_id);
		return(path != m_paths.end() ? path->second : "");
	}

	bool RequireResource(const std::string& l_id){
		auto res = Find(l_id);
		if(res){
			++res->second;
			return true;
		}
		auto path = m_paths.find(l_id);
		if (path == m_paths.end()){ return false; }
		T* resource = Load(path->second);
		if (!resource){ return false; }
		m_resources.emplace(l_id, std::make_pair(resource, 1));
		return true;
	}

	bool ReleaseResource(const std::string& l_id){
		auto res = Find(l_id);
		if (!res){ return false; }
		--res->second;
		if (!res->second){ Unload(l_id); }
		return true;
	}

	void PurgeResources(){
		std::cout << "Purging all resources:" << std::endl;
		while(m_resources.begin() != m_resources.end()){
			std::cout << "Removing: " 
				<< m_resources.begin()->first << std::endl;
			delete m_resources.begin()->second.first;
			m_resources.erase(m_resources.begin());
		}
		std::cout << "Purging finished." << std::endl;
	}

protected:
	T* Load(const std::string& l_path){
		return static_cast<Derived*>(this)->Load(l_path);
	}
private:
	std::pair<T*,unsigned int>* Find(const std::string& l_id){
		auto itr = m_resources.find(l_id);
		return (itr != m_resources.end() ? &itr->second : nullptr);
	}

	bool Unload(const std::string& l_id){
		auto itr = m_resources.find(l_id);
		if (itr == m_resources.end()){ return false; }
		delete itr->second.first;
		m_resources.erase(itr);
		return true;
	}

	void LoadPaths(const std::string& l_pathFile){
		std::ifstream paths;
		paths.open(Utils::GetWorkingDirectory() + l_pathFile);
		if(paths.is_open()){
			std::string line;
			while(std::getline(paths,line)){
				std::stringstream keystream(line);
				std::string pathName;
				std::string path;
				keystream >> pathName;
				keystream >> path;
				m_paths.emplace(pathName,path);
			}
			paths.close();
			return;
		}
		std::cerr << "! Failed loading the path file: " << l_pathFile << std::endl;
	}

	std::unordered_map<std::string, std::pair<T*, unsigned int>> m_resources;
	std::unordered_map<std::string, std::string> m_paths;
};