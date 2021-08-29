# (Advanced) Snake Game

###### Objective and Requirements
Project developed for the IS1200 Computer Hardware Engineering course at KTH, Stockholm.

Advanced version of a Snake Game:
- the snake moves pixel by pixel. 
- the game allows for both one- and two-player modes (where the one-player plays against the computer).
- difficulty increases for each level (the snake speeds up, obstacles show up at level 4 and higher). 
- when a game ends, the player can enter their initials (three capital letters) in a high-score list (the list can later be accessed in the menu).

###### Solution 

I developed the game on the Chipkit uC32 board as well as the basic I/O shield. The menu, game and highscore list is presented on the shield display. Buttons and switches control the game features. LED lights are turned on (one light for each level, all lights are on when the game is over). Instructions are shown on the screen.  

The project made use of the MCB32 toolchains developing environment which was created specifically for the IS1200 KTH course. The program has been written in C.
