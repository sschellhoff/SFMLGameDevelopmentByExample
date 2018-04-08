//#include <vld.h>
#include <iostream>
#include <SFML/Network.hpp>
#include "World.h"

int main(){
	World world;
	sf::Clock clock;
	clock.restart();

	while (world.IsRunning()){
		world.Update(clock.restart());
	}
	return 0;
}