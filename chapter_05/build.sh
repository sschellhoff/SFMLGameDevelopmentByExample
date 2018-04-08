#!/bin/bash
g++ Main.cpp Game.cpp Window.cpp EventManager.cpp State_Game.cpp State_Intro.cpp State_MainMenu.cpp State_Paused.cpp StateManager.cpp -lsfml-graphics -lsfml-window -lsfml-system -o main