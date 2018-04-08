#pragma once
#include "Message.h"

class Observer{
public:
	virtual ~Observer(){}
	virtual void Notify(const Message& l_message) = 0;
};