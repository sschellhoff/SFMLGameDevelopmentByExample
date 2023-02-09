# SFML Simple 2D Game

<img src="/docs/screenshots/1.png" alt="Alt text" title="In-Development Screenshot">


## Intro
This is game based on the source code provided by the book **SFML Game Development By Example (ISBN 9781785287343)** (https://www.packtpub.com/product/sfml-game-development-by-example/9781785287343).

This repository is a fork from **sschellhoff** (https://github.com/sschellhoff/SFMLGameDevelopmentByExample) who provided the original book's source code in a runnable version.

NOTE: The project is still under development!!

## Instructions
### Linux
```
git clone https://github.com/mataktelis11/SFMLGameDevelopmentByExample.git
cd SFMLGameDevelopmentByExample/src/
./build.sh 
```
This will generate an executable file called **main** in the current dir.
### Windows
coming soon....

## Details
This is game is an improvement of the book's **Chapter 7**. More specifically the following additions have been made:
- Added a healthbar for the player.
- Added a new spritesheet for the player.
- Added music and sounds (not with the book's sound manager but with a much less sophisticated implementation)
- Added a new tileset (16 * 16)
- Added a Python script that generates .map files from .png images (This makes it easier to create a map)
- Minor other changes

This project started because I wanted to create a video game from scratch. Thankfully the book does a good job providing and explaining a template that can be a good start as my very first game. I strongly recommend you check out this book if you want to see what game development really is.

I chose Chapter 7 as I feel it is a good middle ground for everyone how have read the book. Do not expect this to be the most amazing game ever.

Before checking out the source code I strongly recommend to read the book first (especially if you are relatively new to C++ like me).

## Assets used

(Note that this section will be updated in future commits.)

1. Textures:
    - Necromancer sprites: https://creativekind.itch.io/necromancer-free - Author: CreativeKind
    - Skeleton sprites: https://luizmelo.itch.io/monsters-creatures-fantasy - Author: LuizMelo
    - Tileset: https://poloviiinkin.itch.io/textures - Author: Poloviiinkin
    - Heart sprites: https://nicolemariet.itch.io/pixel-heart-animation-32x32-16x16-freebie - Author: NicoleMarieT
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
4. Backgrounds:
    - https://ansimuz.itch.io/mountain-dusk-parallax-background - Author: ansimuz
5. Fonts:
    - All fonts in the game's Resources are from https://www.dafont.com/


Huge shout out to all those amazing creators!


## Tools used
- Libresprite: https://libresprite.github.io/#!/
- Gimp: https://www.gimp.org/
- Audacity: https://www.audacityteam.org/

## Important
I do **NOT** own the mentioned book, and this project is not made to be sold in any way.

## License
GPL-3.0 license - Check the LICENSE.md file.
