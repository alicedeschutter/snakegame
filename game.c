#include <stdint.h>
#include <pic32mx.h>
#include "mipslab.h"
#include <stdio.h>

typedef struct s {
  int x;
  int y;
}Snakecell;

#define HEAD_X_1 54
#define HEAD_Y_1 19
#define HEAD_X_2 84
#define HEAD_Y_2 17
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 26

Snakecell snake_1[50], snake_2[50]; // snake_2 is ai snake, snake_1 is normal snake
int length = 20; // both snakes have length 20 at game_start (level 1)
int dir1 = RIGHT; // direction of snake_1
int dir2; //direction of snake_2

/* Data below is used in move_snake and draw_snake functions */
int INITIAL_TAIL_X_1 = -1;
int INITIAL_TAIL_Y_1 = -1;
int INITIAL_TAIL_X_2 = -1;
int INITIAL_TAIL_Y_2 = -1;

/* Below: global counters and important variables such as level, snake_speed etc. */
int level = 1;
int TIME = 100; // if > 10000 / 2
int snake_speed = 500000;
int time_count = 0;
int players = 1;
int letter_counter = 1;
int score_counter = 0;
int count_player_two = 0; // gives ai snake less of an advantage
int simple_count = 0; // used for ai game logic (first needs to go either up or down before turning left since head is facing right)

/* Data below is used in create_food function */
int FOOD_POSITION_X1 = -1;
int FOOD_POSITION_Y1 = -1;

int FOOD_POSITION_X2 = -1;
int FOOD_POSITION_Y2 = -1;

int FOOD_POSITION_X3 = -1;
int FOOD_POSITION_Y3 = -1;

int FOOD_POSITION_X4 = -1;
int FOOD_POSITION_Y4 = -1;

char score[8][50] = {"SCORE:0","SCORE:1","SCORE:2","SCORE:3","SCORE:4","SCORE:5","SCORE:6","SCORE:7"}; //used to display score: amount of apples eaten

/* Data below is used for the highscore list and for the draw_score function */
char INITIAL_ONE_COUNT[1] = {65}; // starts at letter A, INITIAL_ONE_COUNT is used to save first initial when game is over
char INITIAL_TWO_COUNT[1] = {65}; // A
char INITIAL_THREE_COUNT[1] = {65}; // A

char first_initial_1[1] = {};
char second_initial_1[1] = {};
char third_initial_1[1] = {};
int level_1 = 1;
char new_score_1[10] = {};

char first_initial_2[1] = {};
char second_initial_2[1] = {};
char third_initial_2[1] = {};
int level_2 = 1;
char new_score_2[10] = {};

char first_initial_3[1] = {};
char second_initial_3[1] = {};
char third_initial_3[1] = {};
int level_3 = 1;
char new_score_3[10] = {};

// user_isr is not used
void user_isr() {}

void labinit( void ) {
  TRISDSET = 0xe0; //1110 0000 (bit 5 = button 2, bit 6 = button 3, bit 7 = button 4), Buttons are set as inputs
  TRISDSET = 0xf00; //sw1, sw2, sw3, swi4
  TRISESET = 0xff00; // bit 0 to 7 set as output --> LEDs

  TRISFSET = 0x2; //button 1
  return;
}

int levels() {

  if (level == 1) {
    length = 20;
    PORTE = 1; //LED light 1
  }
  else if (level == 2) {
    length = 25;
    PORTE = 2; //LED light 2...
  }
  else if (level == 3) {

    length = 30;
    PORTE = 4;
  }
  else if (level == 4){

    length = 35;
    PORTE = 8;
  }
  else if (level == 5){

    length = 40;
    PORTE = 16;
  }
  else if (level == 6){

    length = 45;
    PORTE = 32;
  }
  else if (level == 7){

    length = 49;
    PORTE = 64;

  }

return;
}

void highscore_list() {

  while (1) {
      time_count ++; // automatically goes back to menu() when time_count > 500
      int switches = getsw();

      create_screen(0, 0, 0, 128, 32, screen);
      create_string(1, 10, 6, "BEST SCORES", screen);

      if (score_counter == 0){ //no games have been played, no score to display or add to the list
        create_string(1, 20, 20, "NONE TO DISPLAY", screen);
        display_full_bin(screen);
      }

      else if (score_counter == 1) { // add score to highscore list
        create_string(1, 60, 14, new_score_1, screen);
        create_string(1, 10, 14, first_initial_1, screen);
        create_string(1, 16, 14, second_initial_1, screen);
        create_string(1, 22, 14, third_initial_1, screen);
        display_full_bin(screen);
      }

      else if (score_counter == 2 || ((score_counter == 3) && (level_3 < level_1) && (level_3 < level_2))) { //if two games have been played or if three games have been played but the third game had a lower score than the two first

        if (level_1 == level_2 || level_1 > level_2) { //display first game score first

          create_string(1, 60, 14, new_score_1, screen);
          create_string(1, 10, 14, first_initial_1, screen);
          create_string(1, 16, 14, second_initial_1, screen);
          create_string(1, 22, 14, third_initial_1, screen);

          create_string(1, 60, 21, new_score_2, screen);
          create_string(1, 10, 21, first_initial_2, screen);
          create_string(1, 16, 21, second_initial_2, screen);
          create_string(1, 22, 21, third_initial_2, screen);
          display_full_bin(screen);

        }else { //display second game score first

          create_string(1, 60, 14, new_score_2, screen);
          create_string(1, 10, 14, first_initial_2, screen);
          create_string(1, 16, 14, second_initial_2, screen);
          create_string(1, 22, 14, third_initial_2, screen);

          create_string(1, 60, 21, new_score_1, screen);
          create_string(1, 10, 21, first_initial_1, screen);
          create_string(1, 16, 21, second_initial_1, screen);
          create_string(1, 22, 21, third_initial_1, screen);
          display_full_bin(screen);
        }

      }else if (score_counter > 2 && !((level_3 < level_1) && (level_3 < level_2))) { //if three games or more have been played and the third game score is higher than at least one of the two first

        if (level_1 >= level_3) {
          create_string(1, 60, 14, new_score_1, screen);
          create_string(1, 10, 14, first_initial_1, screen);
          create_string(1, 16, 14, second_initial_1, screen);
          create_string(1, 22, 14, third_initial_1, screen);

          create_string(1, 60, 21, new_score_3, screen);
          create_string(1, 10, 21, first_initial_3, screen);
          create_string(1, 16, 21, second_initial_3, screen);
          create_string(1, 22, 21, third_initial_3, screen);
          display_full_bin(screen);
        }
        else if (level_2 >= level_3) {
          create_string(1, 60, 14, new_score_2, screen);
          create_string(1, 10, 14, first_initial_2, screen);
          create_string(1, 16, 14, second_initial_2, screen);
          create_string(1, 22, 14, third_initial_2, screen);

          create_string(1, 60, 21, new_score_3, screen);
          create_string(1, 10, 21, first_initial_3, screen);
          create_string(1, 16, 21, second_initial_3, screen);
          create_string(1, 22, 21, third_initial_3, screen);
          display_full_bin(screen);
        }
        else if (level_3 > level_1) {
          create_string(1, 60, 14, new_score_3, screen);
          create_string(1, 10, 14, first_initial_3, screen);
          create_string(1, 16, 14, second_initial_3, screen);
          create_string(1, 22, 14, third_initial_3, screen);

          create_string(1, 60, 21, new_score_1, screen);
          create_string(1, 10, 21, first_initial_1, screen);
          create_string(1, 16, 21, second_initial_1, screen);
          create_string(1, 22, 21, third_initial_1, screen);
          display_full_bin(screen);
        }
        else if (level_3 > level_2) {
          create_string(1, 60, 14, new_score_3, screen);
          create_string(1, 10, 14, first_initial_3, screen);
          create_string(1, 16, 14, second_initial_3, screen);
          create_string(1, 22, 14, third_initial_3, screen);

          create_string(1, 60, 21, new_score_2, screen);
          create_string(1, 10, 21, first_initial_2, screen);
          create_string(1, 16, 21, second_initial_2, screen);
          create_string(1, 22, 21, third_initial_2, screen);
          display_full_bin(screen);
        }
      }

      if (time_count > 500) {
        while (1) {
          switches = getsw();
          time_count = 0; // reset time_count
          INITIAL_ONE_COUNT[0] = 65; //reset to "A" for next game
          INITIAL_TWO_COUNT[0] = 65; //reset to "A" for next game
          INITIAL_THREE_COUNT[0] = 65; //reset to "A" for next game

          create_screen(0, 0, 0, 128, 32, screen);
          create_string(1, 10, 6, "SW1 DOWN", screen);
          create_string(1, 10, 14, "FOR MENU", screen);
          display_full_bin(screen);

          if (!(switches & 0x1)) {

            while (1) {

          		int switches = getsw();

          		create_screen(0, 0, 0, 128, 32, screen);
          	  create_string(1, 64 - (sizeof("ALL SWITCHES")*6) / 2, 14, "ALL SWITCHES", screen);
          		create_string(1, 64 - (sizeof("DOWN")*6) / 2, 22, "DOWN", screen);
          		display_full_bin(screen);

          		if (switches == 0x0) {
          			menu();
                break;
          		}
          	}
            break;
          }
        }
        break;
    }
  }
  return;
}

void draw_score() {
  score_counter++;

  while (1) {
    int buttons = getbtns();
    create_screen(0, 0, 0, 128, 32, screen);

    create_string(1, 64 - (sizeof(score[level-1])*6) / 2, 7, score[level-1], screen);
    create_string(1, 30, 16, "BTN2 TO SAVE", screen);
    display_full_bin(screen);

    if (buttons == 0x1) {

      while (1) {

    		int switches = getsw();

    		create_screen(0, 0, 0, 128, 32, screen);
    	  create_string(1, 64 - (sizeof("ALL SWITCHES")*6) / 2, 14, "ALL SWITCHES", screen);
    		create_string(1, 64 - (sizeof("DOWN")*6) / 2, 22, "DOWN", screen);
    		display_full_bin(screen);

    		if (switches == 0x0) {
          while (1) {

            buttons = getbtns();
            switches = getsw();
            PORTE = 0;
            create_screen(0, 0, 0, 128, 32, screen);
            create_string(1, 5, 7, "SW1:YES", screen);
            create_string(1, 70, 7, "BTN3:NEXT", screen);

            if (letter_counter == 1) {//store first letter

              create_string(1, 35, 20, "LETTER1:", screen);
              create_string(1, 85, 20, INITIAL_ONE_COUNT, screen);
              display_full_bin(screen);
              if (INITIAL_ONE_COUNT[0] > 90) {
                INITIAL_ONE_COUNT[0] = 65; //when Z --> back to A
              }

              if (switches & 0x1) {
                letter_counter++;
              }
              else if (buttons == 0x2) {
                INITIAL_ONE_COUNT[0] += 1;
                quicksleep(1000000);
              }

            }if (letter_counter == 2) { //store second letter

              if (INITIAL_TWO_COUNT[0] > 90) {
                INITIAL_TWO_COUNT[0] = 65; //when Z --> back to A
              }
              create_screen(0, 0, 0, 128, 32, screen);
              create_string(1, 5, 7, "SW1:YES", screen);
              create_string(1, 70, 7, "BTN3:NEXT", screen);

              create_string(1, 35, 20, "LETTER2:", screen);
              create_string(1, 85, 20, INITIAL_TWO_COUNT, screen);
              display_full_bin(screen);

              if (!(switches & 0x1)) {
                letter_counter ++;
              }
              else if (buttons == 0x2) {
                INITIAL_TWO_COUNT[0] += 1;
                quicksleep(1000000);
              }

            }if (letter_counter == 3) { // store third letter

              if (INITIAL_THREE_COUNT[0] > 90) {
                INITIAL_THREE_COUNT[0] = 65; //when Z --> back to A
              }

              create_screen(0, 0, 0, 128, 32, screen);
              create_string(1, 5, 7, "SW1:YES", screen);
              create_string(1, 70, 7, "BTN3:NEXT", screen);

              create_string(1, 35, 20, "LETTER3:", screen);
              create_string(1, 85, 20, INITIAL_THREE_COUNT, screen);
              display_full_bin(screen);

              if ((switches & 0x1)) {

                if (score_counter == 1) {

                  first_initial_1[0] = INITIAL_ONE_COUNT[0];
                  second_initial_1[0] = INITIAL_TWO_COUNT[0];
                  third_initial_1[0] = INITIAL_THREE_COUNT[0];
                  level_1 = level;
                  str_cpy(new_score_1,score[level-1],sizeof(score[level-1]));

                }else if (score_counter == 2) {
                  first_initial_2[0] = INITIAL_ONE_COUNT[0];
                  second_initial_2[0] = INITIAL_TWO_COUNT[0];
                  third_initial_2[0] = INITIAL_THREE_COUNT[0];
                  level_2 = level;
                  str_cpy(new_score_2,score[level-1],sizeof(score[level-1]));

                }else if (score_counter == 3) {
                  first_initial_3[0] = INITIAL_ONE_COUNT[0];
                  second_initial_3[0] = INITIAL_TWO_COUNT[0];
                  third_initial_3[0] = INITIAL_THREE_COUNT[0];
                  level_3 = level;
                  str_cpy(new_score_3,score[level-1],sizeof(score[level-1]));
                }
                else if (score_counter > 3) {
                  if (level > level_3) { //replace old level_3 only if new level is higher than old level_3
                    first_initial_3[0] = INITIAL_ONE_COUNT[0];
                    second_initial_3[0] = INITIAL_TWO_COUNT[0];
                    third_initial_3[0] = INITIAL_THREE_COUNT[0];
                    level_3 = level;
                    str_cpy(new_score_3,score[level-1],sizeof(score[level-1]));
                  }
                  else if (level > level_2) {
                    first_initial_2[0] = INITIAL_ONE_COUNT[0];
                    second_initial_2[0] = INITIAL_TWO_COUNT[0];
                    third_initial_2[0] = INITIAL_THREE_COUNT[0];
                    level_2 = level;
                    str_cpy(new_score_2,score[level-1],sizeof(score[level-1]));
                  }
                  else if (level > level_1) {
                    first_initial_1[0] = INITIAL_ONE_COUNT[0];
                    second_initial_1[0] = INITIAL_TWO_COUNT[0];
                    third_initial_1[0] = INITIAL_THREE_COUNT[0];
                    level_1 = level;
                    str_cpy(new_score_1,score[level-1],sizeof(score[level-1]));
                  }
                }
                letter_counter = 1; // reset letter counter
                highscore_list(); // go to BEST SCORES list!
              }

              else if (buttons == 0x2) {

                INITIAL_THREE_COUNT[0] += 1;
                quicksleep(1000000);

                }
            }
          }
        }
      }


    }
  }


  return;
}

void draw_snake() {
  if (players == 2){

    int i = 0;
    create_screen(0, INITIAL_TAIL_X_2, INITIAL_TAIL_Y_2, 1, 1, screen);
    for(i = 0; i < length; i++){
      create_screen(1, snake_2[i].x, snake_2[i].y, 1, 1, screen);
    }
    //display_full_bin(screen);
  }

  int i = 0;
  create_screen(0, INITIAL_TAIL_X_1, INITIAL_TAIL_Y_1, 1, 1, screen);
  for(i = 0; i < length; i++){
    create_screen(1, snake_1[i].x, snake_1[i].y, 1, 1, screen);
  }
  if (level >= 4) { // create obstacle att level 4
    create_screen(1, 20, 20, 1, 1, screen);
    create_screen(1, 40, 30, 1, 1, screen);
    create_screen(1, 120, 19, 1, 1, screen);
  }
  if (level == 7) {
    create_screen(1, 110, 25, 1, 1, screen);
    create_screen(1, 10, 10, 1, 1, screen);
  }

  display_full_bin(screen);
  quicksleep(snake_speed - (level *150000));
}

void game_logic(){
    int buttons = getbtns(); // button 2,3,4
    int switches = getsw(); // switch 4
    int button1 = getbtn1();

    if (buttons == 0x1) {
      if (dir1 != LEFT) { // if previous direction was: left, don't do anything!
        //update direction
        dir1 = RIGHT;
        }
      }

    if (buttons == 0x4) {
      if (dir1 != RIGHT) {
        dir1 = LEFT;
        }
      }

    if (buttons == 0x2) {
      if (switches & 0x8) { //Switch4 UP
        if (dir1 != DOWN) {
          dir1 = UP;
        }
      }

    if (!(switches & 0x8)) {
        if (dir1 != UP) {
          dir1 = DOWN;
        }
      }

    }

  return;
}

void move_snake(int* initial_x, int* initial_y, Snakecell* snake, int direction) {

    int i = 0;
    *initial_x = snake[length-1].x;
    *initial_y = snake[length-1].y;

    if (direction == RIGHT) {

      Snakecell copySnake[50];
      copySnake[i].x = snake[0].x + 1; // moving x positively and y is const...
      copySnake[i].y = snake[0].y;
      // head is complete... now copy other values in..
      // first iteration done... so only iterate 19 times..

      for(i = 1; i < length; i++){
        copySnake[i].x = snake[i-1].x;
        copySnake[i].y = snake[i-1].y;
      }

      // finally, copy the values into the array... (snake_1 which is the presentation array.. )
      for(i = 0; i < length; i++){
        snake[i].x = copySnake[i].x;
        snake[i].y = copySnake[i].y;
      }

      draw_snake();

    }else if (direction == LEFT) {

      Snakecell copySnake[50];
      copySnake[i].x = snake[0].x - 1; // moving x negatively and y is const...
      copySnake[i].y = snake[0].y;

      for(i = 1; i < length; i++){
        copySnake[i].x = snake[i-1].x;
        copySnake[i].y = snake[i-1].y;
      }

      for(i = 0; i < length; i++){
        snake[i].x = copySnake[i].x;
        snake[i].y = copySnake[i].y;
      }

      draw_snake();

    }else if (direction == UP) {

      Snakecell copySnake[50];
      copySnake[i].x = snake[0].x;
      copySnake[i].y = snake[0].y - 1;

      for(i = 1; i < length; i++){
        copySnake[i].x = snake[i-1].x;
        copySnake[i].y = snake[i-1].y;
      }

      for(i = 0; i < length; i++){
        snake[i].x = copySnake[i].x;
        snake[i].y = copySnake[i].y;
      }

      draw_snake();

    }else if (direction == DOWN) {

      Snakecell copySnake[50];
      copySnake[i].x = snake[0].x;
      copySnake[i].y = snake[0].y + 1;

      for(i = 1; i < length; i++){
        copySnake[i].x = snake[i-1].x;
        copySnake[i].y = snake[i-1].y;
      }

      for(i = 0; i < length; i++){
        snake[i].x = copySnake[i].x;
        snake[i].y = copySnake[i].y;
      }

      draw_snake();

        }
  return;
}

void create_food() {

  // make random number
  // food is represented by square of 4 bits

  int randomX1 = rand() % (SCREEN_WIDTH - 1) + 1; // use the rand() function we have created, generates a number. Use of % so that the random number is within the bounds
  int randomY1 = rand() % (SCREEN_HEIGHT - 1) + 6; // -1 because 0 should not be within bounds, +6 because screen starts at 6 to 32

  int randomX2 = randomX1 + 1;
  int randomY2 = randomY1;

  int randomX3 = randomX1;
  int randomY3 = randomY1 + 1;

  int randomX4 = randomX1 + 1;
  int randomY4 = randomY1 + 1;

  int count = 0;

      while (count < 20) {
        int s;
        for (s = 0; s < length; s++){
          int xS = snake_1[s].x;
          int yS = snake_1[s].y;

          int xS_2;
          int yS_2;

          if (players == 2) {
            int xS_2 = snake_2[s].x;
            int yS_2 = snake_2[s].y;
          }

          // if this collides with some known snake position, then make a new one.. (or collides with obstacle)
          if ((randomX1 == xS && randomY1 == yS) || (randomX2 == xS && yS == randomY2) || (randomX3 == xS && randomY3 == yS) || (randomX4 == xS && randomY4 == yS) || (players == 2 && ((randomX1 == xS_2 && randomY1 == yS_2) || (randomX2 == xS_2 && yS_2 == randomY2) || (randomX3 == xS_2 && randomY3 == yS_2) || (randomX4 == xS_2 && randomY4 == yS_2))) || ((level >= 4) && ((randomX1 == 20 && randomY1 == 20) || (randomX2 == 20 && randomY2 == 20) || (randomX3 == 20 && randomY3 == 20) || (randomX4 == 20 && randomY4 == 20))) || ((level >= 4) && ((randomX1 == 40 && randomY1 == 30) || (randomX2 == 40 && randomY2 == 30) || (randomX3 == 40 && randomY3 == 30) || (randomX4 == 40 && randomY4 == 30))) || ((level >= 4) && ((randomX1 == 120 && randomY1 == 19) || (randomX2 == 120 && randomY2 == 19) || (randomX3 == 120 && randomY3 == 19) || (randomX4 == 120 && randomY4 == 19))) || ((level == 7) && ((randomX1 == 110 && randomY1 == 25) || (randomX2 == 110 && randomY2 == 25) || (randomX3 == 110 && randomY3 == 25) || (randomX4 == 110 && randomY4 == 25))) || ((level == 7) && ((randomX1 == 120 && randomY1 == 25) || (randomX2 == 10 && randomY2 == 10) || (randomX3 == 10 && randomY3 == 10) || (randomX4 == 10 && randomY4 == 10)))) {
            TIME ++;

            randomX1 = rand() % (SCREEN_WIDTH - 1) + 1;
            randomY1 = rand() % (SCREEN_HEIGHT - 1) + 6;

            randomX2 = randomX1 + 1;
            randomY2 = randomY1;

            randomX3 = randomX1;
            randomY3 = randomY1 + 1;

            randomX4 = randomX1 + 1;
            randomY4 = randomY1 + 1;

            count  = 0; // start over the counting... with new random values
            // means this is a good coordinate... but only for this first position of the snake
            break; // the loop because we need to try again...
          }else {
            count++;

          }
        }
      }
      // now finish .. so draw the food

  FOOD_POSITION_X1 = randomX1;
  FOOD_POSITION_Y1 = randomY1;

  FOOD_POSITION_X2 = randomX2;
  FOOD_POSITION_Y2 = randomY2;

  FOOD_POSITION_X3 = randomX3;
  FOOD_POSITION_Y3 = randomY3;

  FOOD_POSITION_X4 = randomX4;
  FOOD_POSITION_Y4 = randomY4;

  create_screen(1, FOOD_POSITION_X1, FOOD_POSITION_Y1, 1, 1, screen);
  create_screen(1, FOOD_POSITION_X2, FOOD_POSITION_Y2, 1, 1, screen);
  create_screen(1, FOOD_POSITION_X3, FOOD_POSITION_Y3, 1, 1, screen);
  create_screen(1, FOOD_POSITION_X4, FOOD_POSITION_Y4, 1, 1, screen);
  display_full_bin(screen);
}

void create_snake(void){

  /// is head....         <-------------(*)

    snake_1[0].x = HEAD_X_1;
    snake_1[0].y = HEAD_Y_1;

    int i;
    for(i = 1; i < length; i++){
      snake_1[i].x = snake_1[0].x - i; // fills the snake array with coord
      snake_1[i].y = snake_1[0].y;
    }

    // values will range from 54,19 to 35,19
    for(i = 0; i < length; i++){
      create_screen(1, snake_1[i].x, snake_1[i].y, 1, 1, screen);
      //display_full_bin(screen);
    }

  if (players == 2){
    // create another...
    snake_2[0].x = HEAD_X_2;
    snake_2[0].y = HEAD_Y_2;

    int i;
    for(i = 1; i < length; i++){
      snake_2[i].x = snake_2[0].x - i; // fills the snake array with coord
      snake_2[i].y = snake_2[0].y;
    }

    // values will range from 54,19 to 35,19
    for(i = 0; i < length; i++){
      create_screen(1, snake_2[i].x, snake_2[i].y, 1, 1, screen);
      //display_full_bin(screen);
    }
    display_full_bin(screen);

  }

  return;
}

void str_cpy(char* target, char* src, int size){
  int i;
  for(i  = 0; i < size; i ++){
    target[i] = src[i];
  }
}

int rand(){
  return (int)((TIME * 31 ) /3);
}

void ai_game_logic() {
  int xS = snake_2[0].x;
  int yS = snake_2[0].y;

  // chose only 1 of the poistions of the food for simplicity
  int delX1 = FOOD_POSITION_X1 - xS; // CASE 1,, 20 - 120.. -100, IF DIR IS LEFT,, GOOD!! KEEP IT LEFT.. IF RIGHT, THEN DOWN, THEN LEFT
  int delY1 = FOOD_POSITION_Y1 - yS;

  if (delX1 > 0) { // snake needs to move to the right
    dir2 = RIGHT;
  }
  else if (delX1 < 0) { //snake needs to move to the left but FIRST has to go up (because head is facing right)
    if (simple_count < 2) {
      if (delY1 > 0) {
        dir2 = DOWN;
        simple_count++;
      }else if (delY1 < 0) {
        dir2 = UP;
        simple_count++;
      }
    }else {
      dir2 = LEFT;
    }
  }

  else if (delX1 == 0) { // now move in y direction
    if (delY1 > 0) {
      dir2 = DOWN;
    }
    else if (delY1 < 0) {
      dir2 = UP;
    }
  }
}

void game_start(void){

  create_screen(0, 0, 6, 128, 26, screen); // leave space at top for menu and score, the screen is 128 wide and (32-6) = 26 bits high)
  create_screen(1, 0, 0, 128, 6, screen); // black space at top for menu and score
  levels();
  create_snake();
  create_food();
  dir1 = RIGHT; // at each game start, snake_1 is going to the start (since head points to that direction)
  count_player_two = 0; // reset non-ai snake advantage
  simple_count = 0;

  while (1){
    if (TIME > 100000){
      TIME = 100;
    }

    TIME++; //USED FOR RANDOM NUM
    //levels();
    game_logic();
    move_snake(&INITIAL_TAIL_X_1, &INITIAL_TAIL_Y_1, snake_1, dir1);

    if (players == 2){
      if (count_player_two > 20) {
        ai_game_logic();
        move_snake(&INITIAL_TAIL_X_2, &INITIAL_TAIL_Y_2, snake_2, dir2);
      }
      count_player_two++;
    }

    int xS = snake_1[0].x;
    int yS = snake_1[0].y;

    /* EXECUTES AFTER  LOSING!!*/
    // after they have finished moving.. then check to see  if game_start is over...
    if (xS == 0 || xS == 128 || yS == 6 || yS == 32) { //out of bounds
      // lost the game....
      draw_score();
      break;
    }
    /* EXECUTES AFTER  WINNING!!*/
    if ((FOOD_POSITION_X1 == xS && FOOD_POSITION_Y1 == yS) || (FOOD_POSITION_X2 == xS && yS == FOOD_POSITION_Y2) || (FOOD_POSITION_X3 == xS && FOOD_POSITION_Y3 == yS) || (FOOD_POSITION_X4 == xS && FOOD_POSITION_Y4 == yS)) { //eat food
      level ++;
       if (level > 7 ){
         // won all the games....
        PORTE = 255; //All LEDS ON
        create_screen(0, 0, 6, 128, 26, screen);
        create_string(1, 64 - (sizeof("WOW")*6) / 2, 14, "WOW", screen);
        create_string(1, 64 - (sizeof("YOU REACHED MAX LEVEL")*6) / 2, 27, "YOU REACHED MAX LEVEL", screen);
        display_full_bin(screen);
        quicksleep(10000000); //(short delay before saving score)

        draw_score();
        break;

       }else {
         //next level to proceed;
         game_start();
         break;
        }
    }
    int i;
    for (i = 1; i < length; i++) {
      if (xS == snake_1[i].x && yS == snake_1[i].y) { //snake bites itself -> game over
          // lost the game..
          draw_score();
          break;
          break;
      }
    }
    if (players == 2) {
      int xS_2 = snake_2[0].x;
      int yS_2 = snake_2[0].y;

      if ((FOOD_POSITION_X1 == xS_2 && FOOD_POSITION_Y1 == yS_2) || (FOOD_POSITION_X2 == xS_2 && yS_2 == FOOD_POSITION_Y2) || (FOOD_POSITION_X3 == xS_2 && FOOD_POSITION_Y3 == yS_2) || (FOOD_POSITION_X4 == xS_2 && FOOD_POSITION_Y4 == yS_2)) {
        // ai won the game
        PORTE = 255; //All LEDS ON
        draw_score();
        break;
      }
      if (level >= 4) { // obstacles att level 4 or higher
        if ((xS_2 == 20 && yS_2 == 20) || (xS_2 == 40 && yS_2 == 30) || (xS_2 == 120 && yS_2 == 19)) {
          game_start(); //ai lost game
          break;
        }
      }

      if (level == 7) { // added obstacles att level 7
        if ((xS_2 == 110 && yS_2 == 25) || (xS_2 == 10 && yS_2 == 10)) {
          game_start(); //ai lost game
          break;
        }
      }
    }

    if (level >= 4) { // obstacles att level 4 or higher
      if ((xS == 20 && yS == 20) || (xS == 40 && yS == 30) || (xS == 120 && yS == 19)) {
        draw_score(); //lost game
        break;
      }
    }

    if (level == 7) { // added obstacles att level 7
      if ((xS == 110 && yS == 25) || (xS == 10 && yS == 10)) {
        draw_score(); //lost game
        break;
      }
    }
  }
  return;
}

void menu(){

  while (1) {

    int switches = getsw();
    int buttons = getbtns();

    create_screen(1, 0, 0, 128, 32, screen);
    create_string(0, 64 - (sizeof("BTN4")*6) / 2, 1, "BTN4", screen);
    create_string(0, 64 - (sizeof("TO START GAME")*6) / 2, 7, "TO START GAME", screen);
    create_string(0, 64 - (sizeof("SWI1 UP")*6) / 2, 16, "SWI1 UP", screen);
    create_string(0, 64 - (sizeof("FOR NEXT SETTING")*6) / 2, 23, "FOR NEXT SETTING", screen);
    display_full_bin(screen);

    if (buttons == 0x4) {
      PORTE = 1;
      level = 1; //reset level to 1 for next game
      snake_speed = 1000000;
      players = 1;
      length = 20;
      dir1 = RIGHT;
      game_start();
    }

    else if (switches & 0x1) {

      while (1) {

        switches = getsw();
        buttons = getbtns();

        create_screen(1, 0, 0, 128, 32, screen);
        create_string(0, 64 - (sizeof("BTN4")*6) / 2, 1, "BTN4", screen);
        create_string(0, 64 - (sizeof("TO PLAY AGAINST AI")*6) / 2, 7, "TO PLAY AGAINST AI", screen);
        create_string(0, 64 - (sizeof("SWI1 DOWN")*6) / 2, 16, "SWI1 DOWN", screen);
        create_string(0, 64 - (sizeof("FOR NEXT SETTING")*6) / 2, 23, "FOR NEXT SETTING", screen);
        display_full_bin(screen);

        if (buttons == 0x4) {
          PORTE = 1;
          level = 1; //reset level to 1 for next game
          length = 20;
          snake_speed = 1000000; // game with ai snake: snakes move a bit faster
          players = 2;
          game_start();
          break;
        }

        else if (!(switches & 0x1)) {

          while (1) {

            switches = getsw();
            buttons = getbtns();

            create_screen(1, 0, 0, 128, 32, screen);
            create_string(0, 64 - (sizeof("BTN4")*6) / 2, 1, "BTN4", screen);
            create_string(0, 64 - (sizeof("FOR HIGHSCORE")*6) / 2, 7, "FOR HIGHSCORE", screen);
            create_string(0, 64 - (sizeof("SW1 DOWN AND BTN3")*6) / 2, 16, "SW1 DOWN AND BTN3", screen);
            create_string(0, 64 - (sizeof("BACK TO START")*6) / 2, 23, "BACK TO START", screen);
            display_full_bin(screen);

            if (buttons == 0x4) {

              highscore_list();
              break;
            }

            else if (buttons == 0x2) {
              menu();

              break;
            }
          }
          break;
        }
      }
      break;
    }

  }
return;
}
