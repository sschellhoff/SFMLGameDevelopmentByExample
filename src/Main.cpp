#include "Game.h"

int main(int argc, char* argv[]){
	// Program entry point.
	{
		Game game(900,675);
		while(!game.GetWindow()->IsDone()){
			game.Update();
			game.Render();
			game.LateUpdate();
		}
	}
}