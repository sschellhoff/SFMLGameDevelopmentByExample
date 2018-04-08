#pragma once
#include "C_Base.h"
#include <string>

class C_Name : public C_Base{
public:
	C_Name() : C_Base(Component::Name){}
	void ReadIn(std::stringstream& l_stream){ l_stream >> m_name; }
	const std::string& GetName()const{ return m_name; }
	void SetName(const std::string& l_name){ m_name = l_name; }
private:
	std::string m_name;
};