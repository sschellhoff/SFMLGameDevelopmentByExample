# SFML Simple 2D Game

![GitHub](https://img.shields.io/github/license/mataktelis11/Simple-SFML-2D-Game)
![GitHub top language](https://img.shields.io/github/languages/top/mataktelis11/Simple-SFML-2D-Game)

This is a game based on the source code provided by the book **SFML Game Development By Example (ISBN 9781785287343)** (https://www.packtpub.com/product/sfml-game-development-by-example/9781785287343).

<img src="/docs/screenshots/1.png" alt="Alt text" title="In-Development Screenshot">


## Intro
This repository is a fork from **sschellhoff** (https://github.com/sschellhoff/SFMLGameDevelopmentByExample) who provided the original book's source code in a runnable version.

NOTE: The project is still under development!!

## Build Instructions

### Linux

#### Prerequisites

Install SFML from your package manager. Also make sure you have cmake.

#### Build

Build from source
```
$ git clone https://github.com/mataktelis11/Simple-SFML-2D-Game.git
$ cd Simple-SFML-2D-Game
$ mkdir build && cd build
$ cmake ..
$ cmake --build .
```
This will generate an executable file called **ScrollOfTheUndead** in the current dir.

### Windows

#### Prerequisites
You need to have installed **Cmake**, **MinGW** compiler and **SFML**. All those need to be in you path. You can download **MinGW** and **SFML** from here: https://www.sfml-dev.org/download/sfml/2.5.1/. Also you can check out this helpful guide for building SFML projects in Windows: https://wfale.net/2023/01/02/sfml-c-and-windows-quick-guide-to-awesome-graphics/.

#### Build

1. Clone the repo or download it as a zip and extract it
2. Create a folder called ```build``` and navigate into it using cmd or gitbash
3. Compile by typing the following commands:

```
$ cmake -G "MinGW Makefiles" ..
$ mingw32-make
```

An .exe file should now exists in the build folder.

## Details
This is game is an improvement of the book's **Chapter 7**. More specifically the following additions have been made:
- Added a healthbar for the player.
- Added a new spritesheet for the player.
- Added music and sounds (not with the book's sound manager but with a much less sophisticated implementation)
- ~Added a new tileset (16 * 16)~ Added an new Tile Set (32x32).
- Added a Python script that generates .map files from .png images (This makes it easier to create a map)
- Added **CMakeLists.txt** a based on this amazing guide: https://dane-bulat.medium.com/cmake-building-sfml-and-game-projects-on-linux-3947b3ba6e8
- Minor other changes

This project started because I wanted to create a video game from scratch. Thankfully the book does a good job providing and explaining a template that can be a good start as my very first game. I strongly recommend you check out this book if you want to see what game development really is.

I chose Chapter 7 as I feel it is a good middle ground for everyone how have read the book. Do not expect this to be the most amazing game ever.

Before checking out the source code I strongly recommend to read the book first (especially if you are relatively new to C++ like me).

## Assets used

(Note that this section will be updated in future commits.)

1. Textures:
    - Necromancer sprites: https://creativekind.itch.io/necromancer-free - Author: CreativeKind
    - Skeleton sprites: https://luizmelo.itch.io/monsters-creatures-fantasy - Author: LuizMelo
    - Tileset 16x16 (not used currently): https://poloviiinkin.itch.io/textures - Author: Poloviiinkin
    - Tileset 32x32 (this is the one used by the game): https://opengameart.org/content/dungeon-crawl-32x32-tiles - Author: Chris Hamons
    - Heart sprites: https://nicolemariet.itch.io/pixel-heart-animation-32x32-16x16-freebie - Author: NicoleMarieT
    - HUD Sprite: https://dandann1.itch.io/hud-buttons - - Author: DanDann1
2. Music:
    - https://freepd.com/world.php
    - https://freepd.com/horror.php
    - https://freepd.com/epic.php
3. Sound effects: from https://pixabay.com/
    - Knife slice - NeoSpica
    - Bone Crack - Universfield
    - attack cannibal beast - jvmyka@gmail.com
    - transition explosion - Cristian Changing
    - shield guard - nekoninja
    - Interface - Universfield
    - Failfare - Wagna
4. Backgrounds:
    - https://ansimuz.itch.io/mountain-dusk-parallax-background - Author: ansimuz
5. Fonts:
    - All fonts in the game's Resources are from https://www.dafont.com/
6. Logo made with https://placeit.net/


Huge shout out to all those amazing creators!


## Tools used
- Libresprite: https://libresprite.github.io/#!/
- Gimp: https://www.gimp.org/
- Audacity: https://www.audacityteam.org/

## Important
I do **NOT** own the mentioned book, and this project is not made to be sold in any way.

## License
GPL-3.0 license - Check the LICENSE.md file.
