#include "Game.h"

int main(int argc, char* argv[]){
	// Program entry point.
	{
		Game game;
		while(!game.GetWindow()->IsDone()){
			game.Update();
			game.Render();
			game.LateUpdate();
		}
	}
	system("PAUSE");
}