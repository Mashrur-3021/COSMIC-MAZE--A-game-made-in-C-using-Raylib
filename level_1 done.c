#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"
#include "raymath.h"
#define CELL 60
#define THICK 15
#define rocketSpeed 2
#define rocketSize 30
#define moonSize 68
const int screen_height = 900;
const int screen_width = 1500;

typedef struct
{
    int x1, y1, x2, y2;
} Wall;

typedef enum
{
    UP,
    RIGHT,
    DOWN,
    LEFT,
    CNT,
} Direction;

typedef struct
{
    float speed;
    Direction dir;
    Vector2 pos;
} Player;

typedef enum
{
    ZERO_WINDOW,
    LEVEL_1,
    TR_WIN_1,
    LEVEL2,
    TR_WIN_2,
    LEVEL_3,
    TR_WIN_3,
    LEVEL4,
    TR_WIN_4,
    TOTAL_WINDOW,
} WINDOW_NAME;

// rocket pics directories
const char *rocketPics[CNT] = {
    "D:/Maze-explorer/rocket/1.png",
    "D:/Maze-explorer/rocket/2.png",
    "D:/Maze-explorer/rocket/3.png",
    "D:/Maze-explorer/rocket/4.png"};

// textures
Texture2D rocketTex[CNT];
Texture2D space_background;
Texture2D planet;

// vector arrays
const Vector2 rocket_position[] = {{2, 9}, {2, 12}, {1, 1}, {2, 11}};
Vector2 planet_position[] = {{23, 8}, {22, 2}, {15, 13}, {22, 2}};

Player rocket = {rocketSpeed, DOWN, rocket_position[0]};

// game messages
char *game_title = "COSMIC MAZE";
char *play_message = "PLAY";
char *transition_msg1 = "Level-1 Done!!!";
char *transition_msg2 = "Click to Move in next level";
char *transition2_msg1 = "Level-2 Done!!!";
char *transition2_msg2 = "Click to Move in next level";
char *transition3_msg1 = "Level-3 Done!!!";
char *transition3_msg2 = "Click to Move in next level";
char *transition4_msg1 = "Level-4 Done!!!";
char *transition4_msg2 = "Click to Move in next level";

// variables for manu windows
Font font_play;
Vector2 play_button_pos;
Vector2 game_title_pos;
Rectangle play_button_posRec;
Rectangle game_title_posRec;
static Vector2 mousepos;

Vector2 message1_pos;
Vector2 message2_pos;
Rectangle message_box;
Vector2 message3_pos;
Vector2 message4_pos;
Rectangle message_box2;
Vector2 message5_pos;
Vector2 message6_pos;
Rectangle message_box3;

// audio
Music backgrnd_music;
Sound clicksound;
Sound crashsound;
Sound level_up_sound;

float font_size = 30;
float spacing = 2;

// wall levels
Wall wall_level1[] = {
    {0, 0, 25, 0},
    {0, 15, 25, 15},
    {0, 0, 0, 15},
    {25, 0, 25, 15},
    {1, 1, 2, 1},
    {2, 1, 2, 2},
    {2, 1, 3, 1},
    {3, 1, 4, 1},
    {4, 2, 5, 2},
    {5, 2, 5, 3},
    {5, 3, 6, 3},
    {6, 2, 6, 3},
    {6, 2, 7, 2},
    {7, 2, 7, 3},
    {7, 3, 8, 3},
    {8, 3, 8, 4},
    {8, 4, 9, 4},
    {9, 4, 9, 5},
    {9, 5, 10, 5},
    {10, 5, 10, 6},
    {10, 6, 11, 6},
    {11, 5, 11, 6},
    {11, 4, 11, 5},
    {11, 3, 11, 4},
    {11, 3, 12, 3},
    {12, 2, 12, 3},
    {12, 2, 13, 2},
    {13, 2, 13, 3},
    {13, 3, 13, 4},
    {13, 4, 13, 5},
    {13, 5, 13, 6},
    {13, 6, 14, 6},
    {14, 6, 15, 6},
    {15, 6, 15, 7},
    {15, 7, 16, 7},
    {16, 6, 16, 7},
    {16, 6, 17, 6},
    {17, 5, 17, 6},
    {17, 4, 17, 5},
    {17, 4, 18, 4},
    {18, 4, 18, 5},
    {18, 5, 18, 6},
    {18, 6, 19, 6},
    {19, 6, 19, 7},
    {19, 7, 20, 7},
    {20, 7, 20, 8},
    {20, 8, 21, 8},
    {21, 8, 22, 8},
    {21, 10, 22, 10},
    {1, 2, 1, 3},
    {1, 3, 1, 4},
    {1, 4, 1, 5},
    {1, 6, 2, 6},
    {1, 5, 1, 6},
    {2, 5, 2, 6},
    {2, 5, 3, 5},
    {3, 4, 3, 5},
    {3, 4, 4, 4},
    {4, 4, 4, 5},
    {4, 5, 5, 5},
    {5, 5, 6, 5},
    {6, 5, 6, 6},
    {6, 6, 6, 7},
    {6, 5, 7, 5},
    {7, 6, 7, 7},
    {8, 7, 8, 8},
    {8, 8, 9, 8},
    {9, 7, 9, 8},
    {9, 7, 10, 7},
    {10, 7, 10, 8},
    {10, 8, 11, 8},
    {11, 7, 11, 8},
    {11, 7, 12, 7},
    {12, 6, 12, 7},
    {12, 5, 12, 6},
    {12, 7, 13, 7},
    {13, 7, 13, 8},
    {13, 8, 14, 8},
    {14, 8, 14, 9},
    {14, 9, 15, 9},
    {15, 8, 15, 9},
    {15, 8, 16, 8},
    {16, 8, 16, 9},
    {16, 9, 17, 9},
    {17, 8, 17, 9},
    {17, 8, 18, 8},
    {18, 8, 18, 9},
    {18, 9, 19, 9},
    {19, 9, 19, 10},
    {19, 10, 20, 10},
    {20, 9, 20, 10},
    {20, 9, 21, 9},
    {21, 9, 21, 10},
    {5, 1, 6, 1},
    {7, 1, 8, 1},
    {9, 1, 10, 1},
    {10, 1, 10, 2},
    {10, 2, 11, 2},
    {11, 1, 11, 2},
    {11, 1, 12, 1},
    {12, 1, 13, 1},
    {13, 1, 14, 1},
    {14, 1, 15, 1},
    {15, 1, 16, 1},
    {16, 1, 17, 1},
    {17, 1, 18, 1},
    {18, 1, 19, 1},
    {19, 1, 20, 1},
    {20, 1, 21, 1},
    {21, 1, 22, 1},
    {22, 1, 23, 1},
    {24, 1, 24, 2},
    {24, 2, 24, 3},
    {23, 3, 24, 3},
    {23, 2, 23, 3},
    {22, 2, 23, 2},
    {21, 2, 22, 2},
    {20, 2, 20, 3},
    {20, 3, 20, 4},
    {20, 5, 20, 6},
    {20, 6, 21, 6},
    {21, 6, 22, 6},
    {22, 6, 23, 6},
    {23, 6, 23, 7},
    {23, 7, 24, 7},
    {22, 7, 22, 8},
    {14, 2, 14, 3},
    {14, 3, 14, 4},
    {14, 4, 15, 4},
    {15, 3, 15, 4},
    {15, 3, 16, 3},
    {16, 3, 16, 4},
    {16, 4, 16, 5},
    {15, 5, 16, 5},
    {17, 3, 18, 3},
    {19, 4, 20, 4},
    {19, 5, 20, 5},
    {24, 6, 24, 7},
    {24, 5, 24, 6},
    {23, 5, 24, 5},
    {23, 4, 23, 5},
    {22, 4, 23, 4},
    {21, 4, 22, 4},
    {21, 3, 21, 4},
    {21, 3, 22, 3},
    {24, 4, 25, 4},
    {24, 7, 25, 7},
    {3, 6, 4, 6},
    {1, 7, 2, 7},
    {3, 7, 4, 7},
    {3, 7, 3, 8},
    {3, 8, 4, 8},
    {2, 3, 2, 4},
    {5, 4, 6, 4},
    {8, 5, 8, 6},
    {6, 9, 7, 9},
    {6, 8, 7, 8},
    {6, 8, 6, 9},
    {5, 9, 6, 9},
    {5, 9, 5, 10},
    {3, 9, 4, 9},
    {3, 11, 4, 11},
    {3, 11, 3, 12},
    {1, 11, 2, 11},
    {2, 11, 3, 11},
    {1, 8, 1, 9},
    {1, 7, 1, 8},
    {1, 11, 1, 12},
    {1, 12, 1, 13},
    {1, 13, 1, 14},
    {2, 13, 2, 14},
    {2, 13, 3, 13},
    {3, 13, 3, 14},
    {3, 14, 4, 14},
    {24, 14, 25, 14},
    {23, 14, 24, 14},
    {22, 14, 23, 14},
    {5, 14, 6, 14},
    {7, 14, 8, 14},
    {23, 10, 24, 10},
    {24, 9, 24, 10},
    {24, 9, 25, 9},
    {9, 14, 9, 15},
    {10, 14, 10, 15},
    {9, 14, 10, 14},
    {10, 13, 11, 13},
    {10, 12, 10, 13},
    {9, 12, 10, 12},
    {9, 12, 9, 13},
    {8, 13, 9, 13},
    {8, 12, 8, 13},
    {7, 12, 8, 12},
    {7, 12, 7, 13},
    {6, 13, 7, 13},
    {6, 12, 6, 13},
    {5, 12, 6, 12},
    {5, 12, 5, 13},
    {4, 13, 5, 13},
    {4, 12, 4, 13},
    {5, 11, 6, 11},
    {7, 11, 8, 11},
    {8, 10, 8, 11},
    {7, 10, 8, 10},
    {9, 11, 10, 11},
    {10, 11, 11, 11},
    {11, 11, 12, 11},
    {9, 9, 10, 9},
    {10, 9, 11, 9},
    {11, 9, 12, 9},
    {12, 9, 13, 9},
    {13, 9, 14, 9},
    {12, 11, 13, 11},
    {13, 11, 14, 11},
    {14, 11, 15, 11},
    {15, 11, 15, 12},
    {14, 12, 15, 12},
    {12, 12, 13, 12},
    {12, 12, 12, 13},
    {13, 13, 14, 13},
    {14, 13, 14, 14},
    {15, 13, 15, 14},
    {15, 13, 16, 13},
    {16, 12, 16, 13},
    {16, 11, 16, 12},
    {16, 11, 17, 11},
    {17, 10, 17, 11},
    {17, 10, 18, 10},
    {18, 10, 18, 11},
    {18, 11, 19, 11},
    {19, 11, 19, 12},
    {19, 12, 20, 12},
    {20, 12, 21, 12},
    {21, 11, 21, 12},
    {21, 11, 22, 11},
    {22, 10, 22, 11},
    {16, 14, 17, 14},
    {17, 13, 17, 14},
    {17, 12, 17, 13},
    {17, 12, 18, 12},
    {18, 12, 18, 13},
    {18, 13, 18, 14},
    {18, 14, 19, 14},
    {19, 13, 19, 14},
    {19, 13, 20, 13},
    {20, 13, 20, 14},
    {20, 14, 21, 14},
    {21, 13, 21, 14},
    {21, 13, 22, 13},
    {22, 12, 22, 13},
    {22, 12, 23, 12},
    {23, 11, 23, 12},
    {23, 11, 24, 11},
    {24, 11, 24, 12},
    {23, 13, 24, 13},
    {12, 14, 12, 15},
    {12, 14, 13, 14},
    {17, 2, 18, 2},
    {19, 2, 19, 3},
    {18, 2, 19, 2},
    {15, 2, 16, 2},
    {5, 7, 5, 8},
    {5, 6, 5, 7},
    {4, 6, 5, 6},
    {4, 1, 5, 1},
    {21, 7, 22, 7},
    {21, 6, 21, 7},
    {8, 13, 8, 14},
};

int wallCount1 = sizeof(wall_level1) / sizeof(Wall);

Wall wall_level2[] = {
    {0, 0, 25, 0},
    {0, 15, 25, 15},
    {0, 0, 0, 15},
    {25, 0, 25, 15},
    {2, 11, 3, 11},
    {3, 10, 3, 11},
    {3, 10, 4, 10},
    {4, 9, 4, 10},
    {4, 9, 5, 9},
    {5, 8, 5, 9},
    {6, 8, 6, 9},
    {5, 8, 6, 8},
    {7, 7, 7, 8},
    {6, 9, 7, 9},
    {7, 8, 7, 9},
    {7, 7, 8, 7},
    {8, 6, 8, 7},
    {8, 6, 9, 6},
    {9, 5, 9, 6},
    {10, 4, 10, 5},
    {10, 4, 11, 4},
    {11, 3, 11, 4},
    {11, 3, 12, 3},
    {12, 2, 12, 3},
    {12, 2, 13, 2},
    {13, 1, 13, 2},
    {13, 1, 14, 1},
    {14, 1, 15, 1},
    {15, 1, 16, 1},
    {16, 1, 17, 1},
    {17, 1, 18, 1},
    {18, 1, 19, 1},
    {19, 1, 20, 1},
    {20, 1, 21, 1},
    {3, 12, 4, 12},
    {4, 11, 4, 12},
    {4, 11, 5, 11},
    {5, 10, 5, 11},
    {5, 10, 6, 10},
    {6, 10, 6, 11},
    {7, 11, 8, 11},
    {8, 10, 8, 11},
    {8, 10, 9, 10},
    {8, 9, 9, 9},
    {9, 8, 9, 9},
    {8, 8, 9, 8},
    {9, 7, 9, 8},
    {10, 7, 11, 7},
    {10, 6, 10, 7},
    {10, 6, 11, 6},
    {11, 5, 11, 6},
    {12, 4, 12, 5},
    {11, 5, 12, 5},
    {12, 4, 13, 4},
    {13, 3, 13, 4},
    {13, 3, 14, 3},
    {14, 2, 14, 3},
    {16, 2, 16, 3},
    {16, 3, 16, 4},
    {16, 4, 17, 4},
    {17, 3, 17, 4},
    {17, 3, 18, 3},
    {18, 2, 18, 3},
    {18, 2, 19, 2},
    {15, 4, 15, 5},
    {15, 3, 15, 4},
    {16, 5, 17, 5},
    {17, 5, 18, 5},
    {18, 4, 18, 5},
    {18, 4, 19, 4},
    {19, 3, 19, 4},
    {19, 3, 20, 3},
    {19, 2, 20, 2},
    {24, 1, 25, 1},
    {23, 1, 24, 1},
    {22, 1, 23, 1},
    {21, 1, 22, 1},
    {21, 1, 21, 2},
    {20, 2, 21, 2},
    {15, 2, 16, 2},
    {24, 3, 24, 4},
    {22, 4, 22, 5},
    {23, 4, 23, 5},
    {23, 5, 23, 6},
    {23, 5, 24, 5},
    {24, 5, 24, 6},
    {23, 6, 23, 7},
    {22, 6, 22, 7},
    {22, 8, 22, 9},
    {23, 9, 23, 10},
    {22, 10, 22, 11},
    {23, 11, 23, 12},
    {23, 8, 24, 8},
    {21, 8, 22, 8},
    {21, 7, 22, 7},
    {22, 7, 23, 7},
    {21, 6, 22, 6},
    {21, 5, 22, 5},
    {20, 5, 21, 5},
    {20, 5, 20, 6},
    {19, 6, 20, 6},
    {19, 6, 19, 7},
    {20, 7, 20, 8},
    {19, 7, 20, 7},
    {18, 6, 19, 6},
    {17, 6, 18, 6},
    {17, 6, 17, 7},
    {16, 6, 17, 6},
    {16, 6, 16, 7},
    {15, 6, 16, 6},
    {20, 4, 21, 4},
    {21, 3, 21, 4},
    {13, 5, 14, 5},
    {13, 5, 13, 6},
    {12, 6, 13, 6},
    {12, 6, 12, 7},
    {12, 7, 13, 7},
    {13, 7, 13, 8},
    {12, 8, 13, 8},
    {12, 8, 12, 9},
    {11, 9, 12, 9},
    {11, 8, 11, 9},
    {10, 8, 11, 8},
    {10, 8, 10, 9},
    {10, 10, 11, 10},
    {11, 10, 12, 10},
    {13, 9, 13, 10},
    {12, 10, 13, 10},
    {13, 9, 14, 9},
    {14, 9, 14, 10},
    {14, 10, 15, 10},
    {15, 9, 15, 10},
    {15, 8, 15, 9},
    {14, 8, 15, 8},
    {15, 8, 16, 8},
    {15, 7, 15, 8},
    {14, 6, 14, 7},
    {7, 5, 7, 6},
    {7, 4, 7, 5},
    {7, 4, 8, 4},
    {8, 4, 8, 5},
    {9, 3, 9, 4},
    {8, 3, 9, 3},
    {7, 3, 8, 3},
    {7, 2, 7, 3},
    {6, 2, 7, 2},
    {6, 2, 6, 3},
    {5, 3, 6, 3},
    {5, 3, 5, 4},
    {4, 4, 5, 4},
    {4, 4, 4, 5},
    {4, 5, 5, 5},
    {5, 5, 6, 5},
    {6, 4, 6, 5},
    {5, 6, 6, 6},
    {4, 6, 5, 6},
    {3, 6, 4, 6},
    {3, 6, 3, 7},
    {2, 7, 3, 7},
    {2, 7, 2, 8},
    {1, 8, 2, 8},
    {1, 9, 2, 9},
    {2, 9, 2, 10},
    {1, 9, 1, 10},
    {2, 9, 3, 9},
    {3, 8, 4, 8},
    {4, 7, 4, 8},
    {4, 7, 5, 7},
    {4, 13, 5, 13},
    {6, 13, 7, 13},
    {7, 12, 8, 12},
    {7, 13, 8, 13},
    {8, 13, 9, 13},
    {8, 12, 9, 12},
    {9, 13, 10, 13},
    {9, 12, 10, 12},
    {10, 13, 11, 13},
    {10, 12, 11, 12},
    {11, 12, 12, 12},
    {11, 13, 12, 13},
    {12, 13, 13, 13},
    {11, 11, 12, 11},
    {9, 11, 10, 11},
    {12, 11, 12, 12},
    {14, 13, 15, 13},
    {14, 11, 15, 11},
    {15, 11, 16, 11},
    {16, 10, 16, 11},
    {16, 9, 16, 10},
    {16, 9, 17, 9},
    {17, 8, 18, 8},
    {18, 9, 18, 10},
    {18, 9, 19, 9},
    {19, 8, 19, 9},
    {15, 12, 15, 13},
    {15, 12, 16, 12},
    {16, 12, 16, 13},
    {16, 13, 17, 13},
    {17, 12, 17, 13},
    {17, 11, 17, 12},
    {17, 11, 18, 11},
    {18, 11, 18, 12},
    {18, 12, 18, 13},
    {18, 13, 19, 13},
    {19, 12, 19, 13},
    {19, 12, 20, 12},
    {20, 11, 20, 12},
    {19, 11, 20, 11},
    {19, 10, 19, 11},
    {19, 10, 20, 10},
    {20, 9, 20, 10},
    {20, 9, 21, 9},
    {21, 9, 21, 10},
    {21, 10, 21, 11},
    {21, 11, 21, 12},
    {21, 12, 22, 12},
    {22, 12, 22, 13},
    {22, 13, 23, 13},
    {23, 13, 24, 13},
    {24, 13, 24, 14},
    {6, 14, 7, 14},
    {3, 14, 4, 14},
    {1, 14, 2, 14},
    {5, 14, 5, 15},
    {14, 13, 14, 14},
    {14, 14, 14, 15},
    {20, 13, 21, 13},
    {20, 13, 20, 14},
    {19, 14, 20, 14},
    {17, 14, 18, 14},
    {16, 14, 17, 14},
    {15, 14, 16, 14},
    {21, 13, 21, 14},
    {21, 14, 22, 14},
    {9, 14, 10, 14},
    {11, 14, 12, 14},
    {13, 13, 13, 14},
    {12, 14, 13, 14},
    {11, 1, 12, 1},
    {9, 1, 10, 1},
    {7, 1, 8, 1},
    {8, 1, 8, 2},
    {8, 2, 9, 2},
    {10, 2, 10, 3},
    {9, 2, 10, 2},
    {6, 1, 7, 1},
    {5, 1, 6, 1},
    {5, 1, 5, 2},
    {4, 2, 5, 2},
    {4, 1, 4, 2},
    {3, 1, 4, 1},
    {1, 1, 2, 1},
    {1, 1, 1, 2},
    {1, 3, 1, 4},
    {0, 5, 1, 5},
    {1, 6, 1, 7},
    {1, 7, 1, 8},
    {2, 5, 2, 6},
    {2, 5, 3, 5},
    {3, 4, 3, 5},
    {3, 3, 3, 4},
    {2, 3, 3, 3},
    {1, 12, 1, 13},
    {1, 11, 1, 12},
    {15, 5, 15, 6},
    {21, 5, 21, 6},
    {9, 6, 10, 6},
};

int wallCount2 = sizeof(wall_level2) / sizeof(Wall);

Wall wall_level3[] = {
    {0, 0, 25, 0},
    {0, 15, 25, 15},
    {0, 0, 0, 15},
    {25, 0, 25, 15},
    {1, 3, 2, 3},
    {2, 3, 3, 3},
    {3, 2, 3, 3},
    {3, 2, 4, 2},
    {4, 2, 4, 3},
    {4, 3, 5, 3},
    {5, 3, 6, 3},
    {6, 3, 6, 4},
    {6, 4, 7, 4},
    {7, 3, 8, 3},
    {7, 3, 7, 4},
    {8, 3, 8, 4},
    {8, 4, 9, 4},
    {9, 3, 9, 4},
    {9, 3, 10, 3},
    {10, 3, 10, 4},
    {10, 4, 11, 4},
    {11, 4, 11, 5},
    {11, 5, 12, 5},
    {12, 5, 12, 6},
    {12, 6, 13, 6},
    {13, 6, 14, 6},
    {15, 6, 16, 6},
    {16, 5, 16, 6},
    {16, 5, 17, 5},
    {17, 4, 17, 5},
    {17, 4, 18, 4},
    {18, 4, 19, 4},
    {19, 4, 20, 4},
    {20, 4, 21, 4},
    {21, 4, 22, 4},
    {22, 4, 23, 4},
    {23, 4, 23, 5},
    {23, 5, 23, 6},
    {23, 6, 23, 7},
    {22, 7, 23, 7},
    {22, 7, 22, 8},
    {21, 8, 22, 8},
    {21, 8, 21, 9},
    {21, 9, 22, 9},
    {22, 9, 22, 10},
    {22, 10, 22, 11},
    {22, 11, 22, 12},
    {21, 12, 22, 12},
    {20, 12, 21, 12},
    {20, 12, 20, 13},
    {18, 12, 19, 12},
    {17, 12, 18, 12},
    {4, 1, 5, 1},
    {6, 1, 7, 1},
    {6, 2, 7, 2},
    {7, 1, 8, 1},
    {8, 1, 9, 1},
    {9, 1, 10, 1},
    {10, 1, 11, 1},
    {11, 1, 12, 1},
    {12, 1, 13, 1},
    {14, 1, 14, 2},
    {13, 1, 14, 1},
    {17, 1, 18, 1},
    {18, 0, 18, 1},
    {8, 2, 9, 2},
    {11, 2, 11, 3},
    {11, 3, 12, 3},
    {11, 2, 12, 2},
    {13, 2, 13, 3},
    {12, 4, 13, 4},
    {14, 4, 14, 5},
    {13, 5, 14, 5},
    {14, 4, 15, 4},
    {15, 4, 15, 5},
    {15, 4, 16, 4},
    {16, 3, 16, 4},
    {15, 3, 16, 3},
    {15, 2, 15, 3},
    {15, 2, 16, 2},
    {16, 2, 17, 2},
    {17, 2, 17, 3},
    {17, 3, 18, 3},
    {18, 2, 18, 3},
    {18, 2, 19, 2},
    {19, 2, 19, 3},
    {19, 3, 20, 3},
    {20, 2, 20, 3},
    {21, 2, 21, 3},
    {21, 3, 22, 3},
    {22, 2, 22, 3},
    {22, 2, 23, 2},
    {23, 2, 23, 3},
    {21, 1, 21, 2},
    {21, 1, 22, 1},
    {23, 1, 24, 1},
    {24, 1, 24, 2},
    {24, 3, 25, 3},
    {24, 2, 24, 3},
    {23, 8, 24, 8},
    {24, 8, 24, 9},
    {24, 6, 24, 7},
    {24, 5, 24, 6},
    {24, 5, 25, 5},
    {1, 3, 1, 4},
    {1, 4, 1, 5},
    {1, 5, 1, 6},
    {1, 6, 1, 7},
    {1, 7, 1, 8},
    {1, 8, 1, 9},
    {1, 9, 1, 10},
    {1, 5, 2, 5},
    {2, 5, 3, 5},
    {3, 5, 3, 6},
    {3, 6, 4, 6},
    {4, 5, 4, 6},
    {4, 4, 4, 5},
    {4, 4, 5, 4},
    {5, 4, 5, 5},
    {5, 5, 5, 6},
    {5, 6, 6, 6},
    {6, 6, 7, 6},
    {7, 5, 7, 6},
    {8, 5, 8, 6},
    {8, 6, 8, 7},
    {8, 6, 9, 6},
    {9, 6, 9, 7},
    {9, 7, 9, 8},
    {1, 10, 2, 10},
    {1, 10, 1, 11},
    {1, 11, 1, 12},
    {1, 11, 2, 11},
    {2, 11, 2, 12},
    {1, 12, 1, 13},
    {2, 12, 2, 13},
    {2, 13, 3, 13},
    {3, 12, 3, 13},
    {3, 12, 4, 12},
    {4, 11, 4, 12},
    {4, 11, 5, 11},
    {5, 11, 5, 12},
    {5, 12, 6, 12},
    {6, 11, 6, 12},
    {6, 11, 7, 11},
    {3, 14, 4, 14},
    {4, 13, 4, 14},
    {4, 13, 5, 13},
    {12, 14, 12, 15},
    {12, 13, 12, 14},
    {12, 13, 13, 13},
    {13, 14, 13, 15},
    {13, 12, 13, 13},
    {13, 12, 14, 12},
    {14, 12, 15, 12},
    {15, 11, 15, 12},
    {17, 11, 17, 12},
    {17, 10, 17, 11},
    {15, 10, 15, 11},
    {16, 10, 16, 11},
    {16, 9, 16, 10},
    {15, 9, 15, 10},
    {15, 9, 16, 9},
    {17, 9, 17, 10},
    {17, 9, 18, 9},
    {18, 8, 18, 9},
    {17, 8, 18, 8},
    {17, 7, 17, 8},
    {16, 7, 17, 7},
    {16, 7, 16, 8},
    {15, 8, 16, 8},
    {14, 8, 15, 8},
    {14, 8, 14, 9},
    {13, 9, 14, 9},
    {13, 9, 13, 10},
    {13, 11, 14, 11},
    {12, 10, 13, 10},
    {12, 10, 12, 11},
    {14, 10, 14, 11},
    {11, 12, 12, 12},
    {11, 11, 12, 11},
    {11, 12, 11, 13},
    {10, 13, 11, 13},
    {10, 12, 10, 13},
    {9, 12, 10, 12},
    {10, 11, 10, 12},
    {9, 12, 9, 13},
    {9, 14, 10, 14},
    {10, 7, 11, 7},
    {11, 6, 11, 7},
    {12, 7, 13, 7},
    {19, 10, 19, 11},
    {19, 10, 20, 10},
    {20, 9, 20, 10},
    {20, 8, 20, 9},
    {20, 7, 21, 7},
    {20, 7, 20, 8},
    {21, 6, 21, 7},
    {21, 6, 22, 6},
    {22, 5, 22, 6},
    {21, 5, 22, 5},
    {20, 5, 20, 6},
    {19, 6, 20, 6},
    {19, 5, 19, 6},
    {18, 5, 19, 5},
    {18, 5, 18, 6},
    {18, 6, 18, 7},
    {18, 7, 19, 7},
    {19, 7, 19, 8},
    {19, 8, 19, 9},
    {18, 10, 18, 11},
    {20, 11, 21, 11},
    {19, 12, 19, 13},
    {19, 14, 20, 14},
    {18, 13, 18, 14},
    {17, 14, 17, 15},
    {18, 13, 19, 13},
    {20, 14, 21, 14},
    {21, 13, 22, 13},
    {21, 13, 21, 14},
    {22, 13, 22, 14},
    {22, 14, 23, 14},
    {23, 13, 23, 14},
    {23, 13, 24, 13},
    {24, 12, 24, 13},
    {23, 12, 24, 12},
    {23, 11, 23, 12},
    {23, 11, 24, 11},
    {24, 10, 24, 11},
    {23, 9, 23, 10},
    {23, 9, 24, 9},
    {14, 9, 14, 10},
    {11, 13, 11, 14},
    {8, 14, 9, 14},
    {8, 13, 8, 14},
    {8, 12, 8, 13},
    {8, 11, 8, 12},
    {8, 11, 9, 11},
    {9, 10, 9, 11},
    {9, 10, 10, 10},
    {10, 10, 11, 10},
    {10, 9, 10, 10},
    {10, 9, 11, 9},
    {11, 8, 11, 9},
    {10, 8, 11, 8},
    {8, 9, 9, 9},
    {7, 9, 8, 9},
    {5, 9, 6, 9},
    {6, 9, 7, 9},
    {4, 9, 5, 9},
    {3, 9, 4, 9},
    {2, 9, 3, 9},
    {2, 8, 2, 9},
    {2, 8, 3, 8},
    {4, 7, 4, 8},
    {3, 8, 4, 8},
    {4, 7, 5, 7},
    {5, 7, 5, 8},
    {5, 8, 6, 8},
    {7, 7, 7, 8},
    {14, 6, 14, 7},
    {12, 8, 12, 9},
    {12, 8, 13, 8},
    {13, 7, 13, 8},
    {6, 14, 7, 14},
    {7, 13, 7, 14},
    {6, 13, 7, 13},
    {5, 14, 5, 15},
    {2, 14, 2, 15},
    {1, 14, 1, 15},
};
int wallCount3 = sizeof(wall_level3) / sizeof(Wall);

Wall wall_level4[] = {
    {0, 0, 25, 0},
    {0, 15, 25, 15},
    {0, 0, 0, 15},
    {25, 0, 25, 15},
    {11, 13, 12, 13},
    {11, 12, 11, 13},
    {10, 12, 11, 12},
    {9, 12, 10, 12},
    {9, 11, 9, 12},
    {9, 11, 10, 11},
    {10, 11, 11, 11},
    {11, 11, 12, 11},
    {12, 11, 13, 11},
    {13, 11, 14, 11},
    {14, 11, 15, 11},
    {15, 11, 16, 11},
    {16, 11, 17, 11},
    {17, 10, 17, 11},
    {17, 10, 18, 10},
    {18, 10, 18, 11},
    {18, 11, 18, 12},
    {18, 12, 19, 12},
    {19, 12, 20, 12},
    {20, 11, 20, 12},
    {21, 10, 21, 11},
    {20, 11, 21, 11},
    {21, 9, 21, 10},
    {21, 8, 21, 9},
    {21, 7, 21, 8},
    {21, 6, 21, 7},
    {21, 6, 22, 6},
    {22, 6, 23, 6},
    {23, 6, 24, 6},
    {24, 5, 24, 6},
    {23, 5, 24, 5},
    {23, 4, 23, 5},
    {23, 3, 23, 4},
    {23, 3, 24, 3},
    {24, 2, 24, 3},
    {24, 1, 24, 2},
    {23, 1, 24, 1},
    {22, 1, 23, 1},
    {21, 1, 22, 1},
    {20, 1, 21, 1},
    {19, 1, 20, 1},
    {18, 1, 19, 1},
    {18, 1, 18, 2},
    {17, 2, 18, 2},
    {16, 2, 17, 2},
    {15, 4, 16, 4},
    {15, 3, 15, 4},
    {16, 4, 17, 4},
    {17, 3, 17, 4},
    {17, 3, 18, 3},
    {18, 3, 19, 3},
    {19, 2, 19, 3},
    {19, 2, 20, 2},
    {20, 2, 21, 2},
    {21, 2, 22, 2},
    {22, 2, 22, 3},
    {21, 3, 22, 3},
    {20, 3, 21, 3},
    {20, 3, 20, 4},
    {19, 4, 20, 4},
    {18, 4, 19, 4},
    {18, 4, 18, 5},
    {18, 5, 19, 5},
    {19, 5, 19, 6},
    {18, 6, 19, 6},
    {18, 6, 18, 7},
    {18, 7, 19, 7},
    {19, 7, 20, 7},
    {20, 6, 20, 7},
    {20, 5, 20, 6},
    {20, 5, 21, 5},
    {21, 4, 21, 5},
    {21, 4, 22, 4},
    {19, 8, 20, 8},
    {20, 8, 20, 9},
    {19, 9, 20, 9},
    {19, 9, 19, 10},
    {18, 9, 19, 9},
    {18, 8, 18, 9},
    {17, 8, 18, 8},
    {16, 8, 17, 8},
    {15, 8, 16, 8},
    {15, 9, 16, 9},
    {16, 9, 17, 9},
    {15, 10, 16, 10},
    {17, 8, 17, 9},
    {10, 14, 11, 14},
    {10, 13, 10, 14},
    {9, 13, 10, 13},
    {8, 13, 9, 13},
    {8, 12, 8, 13},
    {8, 11, 8, 12},
    {8, 10, 8, 11},
    {8, 10, 9, 10},
    {9, 10, 10, 10},
    {10, 10, 11, 10},
    {11, 10, 12, 10},
    {13, 10, 14, 10},
    {14, 9, 14, 10},
    {12, 9, 12, 10},
    {13, 9, 13, 10},
    {13, 8, 13, 9},
    {12, 8, 12, 9},
    {11, 8, 12, 8},
    {11, 7, 11, 8},
    {10, 7, 11, 7},
    {10, 6, 10, 7},
    {10, 5, 10, 6},
    {10, 5, 11, 5},
    {11, 4, 11, 5},
    {11, 3, 11, 4},
    {12, 7, 13, 7},
    {12, 6, 12, 7},
    {12, 6, 13, 6},
    {13, 5, 13, 6},
    {12, 5, 13, 5},
    {12, 4, 12, 5},
    {12, 3, 12, 4},
    {12, 1, 12, 2},
    {11, 1, 11, 2},
    {12, 2, 12, 3},
    {14, 7, 14, 8},
    {14, 7, 15, 7},
    {15, 6, 15, 7},
    {14, 6, 15, 6},
    {14, 5, 14, 6},
    {14, 5, 15, 5},
    {13, 4, 14, 4},
    {14, 4, 14, 5},
    {13, 4, 13, 5},
    {16, 1, 17, 1},
    {17, 0, 17, 1},
    {13, 1, 14, 1},
    {15, 1, 15, 2},
    {16, 6, 16, 7},
    {17, 5, 17, 6},
    {9, 1, 10, 1},
    {8, 1, 9, 1},
    {7, 2, 8, 2},
    {8, 14, 9, 14},
    {6, 14, 7, 14},
    {6, 13, 6, 14},
    {7, 12, 7, 13},
    {7, 11, 7, 12},
    {6, 11, 6, 12},
    {6, 10, 6, 11},
    {6, 9, 6, 10},
    {6, 8, 6, 9},
    {6, 8, 7, 8},
    {7, 8, 8, 8},
    {8, 6, 8, 7},
    {8, 5, 8, 6},
    {8, 5, 9, 5},
    {9, 4, 9, 5},
    {9, 4, 10, 4},
    {10, 3, 10, 4},
    {10, 2, 10, 3},
    {10, 2, 11, 2},
    {7, 9, 7, 10},
    {7, 9, 8, 9},
    {8, 9, 9, 9},
    {9, 9, 10, 9},
    {10, 9, 11, 9},
    {9, 8, 10, 8},
    {9, 7, 9, 8},
    {9, 6, 9, 7},
    {1, 1, 2, 1},
    {2, 1, 3, 1},
    {4, 0, 4, 1},
    {3, 1, 4, 1},
    {5, 1, 5, 2},
    {7, 1, 8, 1},
    {7, 0, 7, 1},
    {8, 3, 9, 3},
    {9, 2, 9, 3},
    {9, 2, 10, 2},
    {6, 3, 7, 3},
    {6, 3, 6, 4},
    {6, 4, 6, 5},
    {6, 5, 6, 6},
    {6, 6, 7, 6},
    {7, 6, 8, 6},
    {7, 4, 7, 5},
    {7, 2, 7, 3},
    {8, 2, 8, 3},
    {7, 4, 8, 4},
    {13, 13, 13, 14},
    {13, 12, 14, 12},
    {14, 12, 14, 13},
    {15, 13, 15, 14},
    {14, 14, 15, 14},
    {15, 13, 16, 13},
    {16, 12, 16, 13},
    {16, 12, 17, 12},
    {17, 12, 17, 13},
    {16, 14, 17, 14},
    {17, 13, 17, 14},
    {18, 13, 18, 14},
    {18, 14, 19, 14},
    {19, 13, 19, 14},
    {19, 13, 20, 13},
    {20, 13, 20, 14},
    {20, 14, 21, 14},
    {21, 13, 21, 14},
    {21, 13, 22, 13},
    {22, 13, 22, 14},
    {22, 14, 23, 14},
    {23, 12, 23, 13},
    {22, 12, 23, 12},
    {22, 11, 22, 12},
    {22, 11, 23, 11},
    {23, 10, 23, 11},
    {22, 10, 23, 10},
    {22, 9, 22, 10},
    {22, 9, 23, 9},
    {23, 8, 23, 9},
    {22, 8, 23, 8},
    {22, 7, 22, 8},
    {22, 7, 23, 7},
    {24, 12, 24, 13},
    {24, 11, 24, 12},
    {24, 10, 24, 11},
    {24, 9, 24, 10},
    {24, 8, 24, 9},
    {24, 7, 24, 8},
    {11, 1, 12, 1},
    {5, 2, 6, 2},
    {4, 2, 4, 3},
    {3, 2, 4, 2},
    {3, 2, 3, 3},
    {2, 3, 3, 3},
    {3, 1, 3, 2},
    {1, 2, 2, 2},
    {1, 2, 1, 3},
    {1, 4, 1, 5},
    {1, 4, 2, 4},
    {2, 5, 3, 5},
    {2, 4, 3, 4},
    {3, 5, 4, 5},
    {3, 4, 4, 4},
    {4, 5, 5, 5},
    {5, 3, 5, 4},
    {2, 5, 2, 6},
    {1, 6, 1, 7},
    {1, 7, 2, 7},
    {2, 7, 2, 8},
    {1, 8, 2, 8},
    {1, 9, 2, 9},
    {2, 9, 3, 9},
    {3, 9, 4, 9},
    {4, 8, 4, 9},
    {4, 8, 5, 8},
    {5, 7, 5, 8},
    {4, 7, 5, 7},
    {4, 6, 4, 7},
    {3, 6, 3, 7},
    {3, 6, 4, 6},
    {3, 7, 3, 8},
    {6, 1, 6, 2},
    {6, 0, 6, 1},
    {6, 2, 6, 3},
    {5, 9, 5, 10},
    {3, 10, 4, 10},
    {1, 10, 2, 10},
    {1, 11, 1, 12},
    {2, 12, 3, 12},
    {4, 12, 5, 12},
    {1, 14, 2, 14},
    {3, 13, 3, 14},
    {3, 12, 3, 13},
    {4, 13, 4, 14},
    {5, 13, 5, 14},
    {3, 11, 4, 11},
    {1, 13, 2, 13},
};

int wallCount4 = sizeof(wall_level4) / sizeof(wall_level4[0]);

// functions
// functions

// functions

void DrawWall(Wall w)
{
    int x1 = w.x1 * CELL, y1 = w.y1 * CELL;
    int x2 = w.x2 * CELL, y2 = w.y2 * CELL;
    int half = THICK / 2;

    if (x1 == x2)
    {
        int top = (y1 < y2) ? y1 : y2;
        int height = abs(y2 - y1);
        DrawRectangle(x1 - half, top - half, THICK, height + THICK, (Color){27, 42, 82, 255});
    }
    else
    {
        int left = (x1 < x2) ? x1 : x2;
        int width = abs(x2 - x1);
        DrawRectangle(left - half, y1 - half, width + THICK, THICK, (Color){27, 42, 82, 255});
    }
}

bool hitWall(Vector2 rocket_pos, Wall *level, int n)
{
    Rectangle rocketRec = {rocket_pos.x * CELL, rocket_pos.y * CELL, rocketSize + 0, rocketSize + 0};

    Rectangle wallRec;

    for (int i = 0; i < n; i++)
    {
        Wall w = level[i];

        int x1 = w.x1 * CELL;
        int x2 = w.x2 * CELL;
        int y1 = w.y1 * CELL;
        int y2 = w.y2 * CELL;

        int half = THICK / 2;

        if (x1 == x2)
        {
            int top = (y1 < y2) ? y1 : y2;
            int height = abs(y1 - y2);
            wallRec = (Rectangle){x1 - half, top - half, THICK, height + THICK};
        }

        else if (y1 == y2)
        {
            int left = (x1 < x2) ? x1 : x2;
            int width = abs(x1 - x2);
            wallRec = (Rectangle){left - half, y1 - half, width + THICK, THICK};
        }

        if (CheckCollisionRecs(rocketRec, wallRec))
            return true;
    }

    return false;
}

bool is_at_same_place(Vector2 planet_pos, Vector2 rocket_pos)
{
    int rx = (int)(rocket_pos.x + 0.0f);
    int ry = (int)(rocket_pos.y + 0.0f);
    int px = (int)(planet_pos.x);
    int py = (int)(planet_pos.y);
    if (rx == px && ry == py)
        return true;
    else
        return false;
}

void updateRocket(Player *rocket, Wall wall_level[], int wall_count)
{
    float dt = GetFrameTime();
    Direction pressed = rocket->dir;
    bool is_Key_pressed = false;
    rocket->speed = rocketSpeed;

    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
    {
        pressed = UP;
        is_Key_pressed = true;
    }

    if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
    {
        pressed = DOWN;
        is_Key_pressed = true;
    }

    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
    {
        pressed = RIGHT;
        is_Key_pressed = true;
    }

    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
    {
        pressed = LEFT;
        is_Key_pressed = true;
    }

    if (is_Key_pressed)
    {
        if (pressed != rocket->dir)
        {
            rocket->dir = pressed;
        }
        else
        {
            switch (pressed)
            {
            case UP:
                Vector2 next_pos = rocket->pos;
                next_pos.y = rocket->pos.y - rocket->speed * dt;
                if (!hitWall(next_pos, wall_level, wall_count))
                {
                    rocket->pos = next_pos;
                    if (rocket->pos.y < 0)
                    {
                        rocket->pos.y = 0;
                    }
                }
                else if (hitWall(next_pos, wall_level, wall_count))
                {
                    PlaySound(crashsound);
                }
                break;

            case DOWN:
                next_pos = rocket->pos;
                next_pos.y = rocket->pos.y + rocket->speed * dt;
                if (!hitWall(next_pos, wall_level, wall_count))
                {
                    rocket->pos = next_pos;
                    if (rocket->pos.y > screen_height)
                    {
                        rocket->pos.y = screen_height;
                    }
                }
                else if (hitWall(next_pos, wall_level, wall_count))
                {
                    PlaySound(crashsound);
                }
                break;

            case RIGHT:
                next_pos = rocket->pos;
                next_pos.x = rocket->pos.x + rocket->speed * dt;
                if (!hitWall(next_pos, wall_level, wall_count))
                {
                    rocket->pos = next_pos;
                    if (rocket->pos.x > screen_width)
                    {
                        rocket->pos.x = screen_width;
                    }
                }
                else if (hitWall(next_pos, wall_level, wall_count))
                {
                    PlaySound(crashsound);
                }
                break;

            case LEFT:
                next_pos = rocket->pos;
                next_pos.x = rocket->pos.x - rocket->speed * dt;
                if (!hitWall(next_pos, wall_level, wall_count))
                {
                    rocket->pos = next_pos;
                    if (rocket->pos.x < 0)
                    {
                        rocket->pos.x = 0;
                    }
                }
                else if (hitWall(next_pos, wall_level, wall_count))
                {
                    PlaySound(crashsound);
                }
                break;

            default:
                break;
            }
        }
    }
}

void load_data_0()
{

    font_play = LoadFont("D:/Raylib project/Fonts/ALIEN CYBERNETICS.ttf");
    play_button_pos = (Vector2){screen_width / 2 - MeasureTextEx(font_play, play_message, (float)font_play.baseSize + 20, 2).x / 2,
                                screen_height / 2 - 30 - MeasureTextEx(font_play, play_message, (float)font_play.baseSize, 2).y / 2};

    game_title_pos = (Vector2){screen_width / 2 - MeasureTextEx(font_play, game_title, (float)font_play.baseSize + 40, 4).x / 2,
                               screen_height / 10 - MeasureTextEx(font_play, game_title, (float)font_play.baseSize + 40, 4).y / 2};

    float paddingX = 30;
    float paddingY = 15;

    play_button_posRec = (Rectangle){play_button_pos.x - paddingX,
                                     play_button_pos.y - paddingY,
                                     MeasureTextEx(font_play, play_message, (float)font_play.baseSize + 20, 2).x + paddingX * 2,
                                     MeasureTextEx(font_play, play_message, (float)font_play.baseSize + 20, 2).y + paddingY * 2};

    game_title_posRec = (Rectangle){game_title_pos.x - paddingX,
                                    game_title_pos.y - paddingY,
                                    MeasureTextEx(font_play, game_title, (float)font_play.baseSize + 40, 4).x + paddingX * 4,
                                    MeasureTextEx(font_play, game_title, (float)font_play.baseSize + 40, 4).y / 2 + paddingY * 4};
}

void load_data_1()
{
    space_background = LoadTexture("D:/Maze-explorer/Background/2.png");
    planet = LoadTexture("D:/Maze-explorer/Planets/planet.png");
}

void load_data_transition_window()
{
    float padding_x = 50;
    float padding_y = 30;
    float line_gap = 20;

    Vector2 msg1_size = MeasureTextEx(font_play, transition_msg1, font_size, spacing);
    Vector2 msg2_size = MeasureTextEx(font_play, transition_msg2, font_size, spacing);

    float text_width = (msg1_size.x > msg2_size.x) ? msg1_size.x : msg2_size.x;

    float box_width = text_width + padding_x * 2;
    float box_height = msg1_size.y + msg2_size.y + line_gap + padding_y * 2;

    message_box = (Rectangle){
        screen_width / 2 - box_width / 2,
        screen_height / 2 - box_height / 2,
        box_width,
        box_height};

    message1_pos = (Vector2){
        screen_width / 2 - msg1_size.x / 2,
        message_box.y + padding_y};

    message2_pos = (Vector2){
        screen_width / 2 - msg2_size.x / 2,
        message_box.y + padding_y + msg1_size.y + line_gap};
}
void load_data_transition_window2()
{
    float padding_x = 50;
    float padding_y = 30;
    float line_gap = 20;

    Vector2 msg1_size = MeasureTextEx(font_play, transition2_msg1, font_size, spacing);
    Vector2 msg2_size = MeasureTextEx(font_play, transition2_msg2, font_size, spacing);

    float text_width = (msg1_size.x > msg2_size.x) ? msg1_size.x : msg2_size.x;

    float box_width = text_width + padding_x * 2;
    float box_height = msg1_size.y + msg2_size.y + line_gap + padding_y * 2;

    message_box2 = (Rectangle){
        screen_width / 2 - box_width / 2,
        screen_height / 2 - box_height / 2,
        box_width,
        box_height};

    message3_pos = (Vector2){
        screen_width / 2 - msg1_size.x / 2,
        message_box2.y + padding_y};

    message4_pos = (Vector2){
        screen_width / 2 - msg2_size.x / 2,
        message_box2.y + padding_y + msg1_size.y + line_gap};
}
void load_data_transition_window3()
{
    float padding_x = 50;
    float padding_y = 30;
    float line_gap = 20;

    Vector2 msg1_size = MeasureTextEx(font_play, transition3_msg1, font_size, spacing);
    Vector2 msg2_size = MeasureTextEx(font_play, transition3_msg2, font_size, spacing);

    float text_width = (msg1_size.x > msg2_size.x) ? msg1_size.x : msg2_size.x;

    float box_width = text_width + padding_x * 2;
    float box_height = msg1_size.y + msg2_size.y + line_gap + padding_y * 2;

    message_box3 = (Rectangle){
        screen_width / 2 - box_width / 2,
        screen_height / 2 - box_height / 2,
        box_width,
        box_height};

    message5_pos = (Vector2){
        screen_width / 2 - msg1_size.x / 2,
        message_box3.y + padding_y};

    message6_pos = (Vector2){
        screen_width / 2 - msg2_size.x / 2,
        message_box3.y + padding_y + msg1_size.y + line_gap};
}
void start_gameplay()
{
    static int level = 0;

    switch (level)
    {
    case ZERO_WINDOW:

        mousepos = GetMousePosition();
        DrawRectangleRounded(play_button_posRec, 1.0f, 8, (Color){10, 15, 40, 255});
        DrawRectangleRoundedLinesEx(play_button_posRec, 1.0f, 8, 2, BLACK);
        DrawRectangleRounded(game_title_posRec, 1.0f, 8, (Color){10, 15, 40, 255});
        DrawRectangleRoundedLinesEx(game_title_posRec, 1.0f, 8, 4, BLACK);

        DrawTextEx(font_play, play_message, play_button_pos, (float)font_play.baseSize + 20, 2, BLUE);
        DrawTextEx(font_play, game_title, game_title_pos, (float)font_play.baseSize + 40, 2, BLUE);

        if (CheckCollisionPointRec(mousepos, play_button_posRec) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            PlaySound(clicksound);
            level = LEVEL_1;
        }

        break;

    case LEVEL_1:

        DrawTexturePro(planet,
                       (Rectangle){0, 0, planet.width, planet.height},
                       (Rectangle){planet_position[0].x * CELL, planet_position[0].y * CELL, moonSize, moonSize},
                       (Vector2){0, 0}, 0.0f, WHITE);

        DrawTexturePro(rocketTex[rocket.dir],
                       (Rectangle){0, 0, rocketTex[rocket.dir].width, rocketTex[rocket.dir].height},
                       (Rectangle){rocket.pos.x * CELL, rocket.pos.y * CELL, rocketSize, rocketSize},
                       (Vector2){0, 0}, 0.0f, WHITE);

        for (int i = 0; i < wallCount1; i++)
        {
            DrawWall(wall_level1[i]);
        }

        updateRocket(&rocket, wall_level1, wallCount1);

        if (is_at_same_place(planet_position[0], rocket.pos))
        {
            PlaySound(level_up_sound);
            level = TR_WIN_1;
        }
        break;

    case TR_WIN_1:
        mousepos = GetMousePosition();
        DrawRectangleRounded(message_box, 1.0f, 8, (Color){10, 15, 40, 255});
        DrawRectangleRoundedLinesEx(message_box, 1.0f, 8, 2, BLACK);

        DrawTextEx(font_play, transition_msg1, message1_pos, font_size, spacing, BLUE);
        DrawTextEx(font_play, transition_msg2, message2_pos, font_size, spacing, BLUE);

        if (CheckCollisionPointRec(mousepos, message_box) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            PlaySound(clicksound);
            rocket.pos = rocket_position[1];
            rocket.dir = DOWN;
            level = LEVEL2;
        }

        break;

    case LEVEL2:

        DrawTexturePro(planet,
                       (Rectangle){0, 0, planet.width, planet.height},
                       (Rectangle){planet_position[1].x * CELL, planet_position[1].y * CELL, moonSize, moonSize},
                       (Vector2){0, 0}, 0.0f, WHITE);

        DrawTexturePro(rocketTex[rocket.dir],
                       (Rectangle){0, 0, rocketTex[rocket.dir].width, rocketTex[rocket.dir].height},
                       (Rectangle){rocket.pos.x * CELL, rocket.pos.y * CELL, rocketSize, rocketSize},
                       (Vector2){0, 0}, 0.0f, WHITE);

        for (int i = 0; i < wallCount2; i++)
        {
            DrawWall(wall_level2[i]);
        }

        updateRocket(&rocket, wall_level2, wallCount2);

        if (is_at_same_place(planet_position[1], rocket.pos))
        {
            PlaySound(level_up_sound);
            level = TR_WIN_2;
        }
        break;

    case TR_WIN_2:
        mousepos = GetMousePosition();
        DrawRectangleRounded(message_box2, 1.0f, 8, (Color){10, 15, 40, 255});
        DrawRectangleRoundedLinesEx(message_box2, 1.0f, 8, 2, BLACK);

        DrawTextEx(font_play, transition2_msg1, message3_pos, font_size, spacing, BLUE);
        DrawTextEx(font_play, transition2_msg2, message4_pos, font_size, spacing, BLUE);

        if (CheckCollisionPointRec(mousepos, message_box2) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            PlaySound(clicksound);
            rocket.pos = rocket_position[2];
            rocket.dir = RIGHT;
            level = LEVEL_3;
        }

        break;

    case LEVEL_3:

        DrawTexturePro(planet,
                       (Rectangle){0, 0, planet.width, planet.height},
                       (Rectangle){planet_position[2].x * CELL, planet_position[2].y * CELL, moonSize, moonSize},
                       (Vector2){0, 0}, 0.0f, WHITE);

        DrawTexturePro(rocketTex[rocket.dir],
                       (Rectangle){0, 0, rocketTex[rocket.dir].width, rocketTex[rocket.dir].height},
                       (Rectangle){rocket.pos.x * CELL, rocket.pos.y * CELL, rocketSize, rocketSize},
                       (Vector2){0, 0}, 0.0f, WHITE);

        for (int i = 0; i < wallCount3; i++)
        {
            DrawWall(wall_level3[i]);
        }

        updateRocket(&rocket, wall_level3, wallCount3);

        if (is_at_same_place(planet_position[2], rocket.pos))
        {
            PlaySound(level_up_sound);
            level = TR_WIN_3;
        }
        break;

    case TR_WIN_3:
        mousepos = GetMousePosition();
        DrawRectangleRounded(message_box3, 1.0f, 8, (Color){10, 15, 40, 255});
        DrawRectangleRoundedLinesEx(message_box3, 1.0f, 8, 2, BLACK);

        DrawTextEx(font_play, transition3_msg1, message5_pos, font_size, spacing, BLUE);
        DrawTextEx(font_play, transition3_msg2, message6_pos, font_size, spacing, BLUE);

        if (CheckCollisionPointRec(mousepos, message_box3) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            PlaySound(clicksound);
            rocket.pos = rocket_position[3];
            rocket.dir = LEFT;
            level = LEVEL4;
        }

        break;

    case LEVEL4:

        DrawTexturePro(planet,
                       (Rectangle){0, 0, planet.width, planet.height},
                       (Rectangle){planet_position[3].x * CELL, planet_position[3].y * CELL, moonSize, moonSize},
                       (Vector2){0, 0}, 0.0f, WHITE);

        DrawTexturePro(rocketTex[rocket.dir],
                       (Rectangle){0, 0, rocketTex[rocket.dir].width, rocketTex[rocket.dir].height},
                       (Rectangle){rocket.pos.x * CELL, rocket.pos.y * CELL, rocketSize, rocketSize},
                       (Vector2){0, 0}, 0.0f, WHITE);

        for (int i = 0; i < wallCount4; i++)
        {
            DrawWall(wall_level4[i]);
        }

        updateRocket(&rocket, wall_level4, wallCount4);

        if (is_at_same_place(planet_position[3], rocket.pos))
        {
            PlaySound(level_up_sound);
            level = TR_WIN_4;
        }
        break;

    case TR_WIN_4:
        mousepos = GetMousePosition();
        DrawRectangleRounded(message_box3, 1.0f, 8, (Color){10, 25, 40, 255});
        DrawRectangleRoundedLinesEx(message_box3, 1.0f, 8, 2, BLACK);

        DrawTextEx(font_play, transition4_msg1, message5_pos, font_size, spacing, BLUE);
        DrawTextEx(font_play, transition4_msg2, message6_pos, font_size, spacing, BLUE);

        if (CheckCollisionPointRec(mousepos, message_box3) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            PlaySound(clicksound);
            level = TOTAL_WINDOW;
        }
        break;

    case TOTAL_WINDOW:
        break;

        break;

    default:
        break;
    }
}
int main()
{
    InitWindow(screen_width, screen_height, "Maze solver");
    InitAudioDevice();
    SetMasterVolume(0.5f);
    SetTargetFPS(60);

    for (int i = 0; i < CNT; i++)
    {
        rocketTex[i] = LoadTexture(rocketPics[i]);
    }

    load_data_0();
    load_data_1();
    load_data_transition_window();
    load_data_transition_window2();
    load_data_transition_window3();

    backgrnd_music = LoadMusicStream("D:/Maze-explorer/Audio/background.mp3");
    clicksound = LoadSound("D:/Maze-explorer/Audio/click.wav");
    crashsound = LoadSound("D:/Maze-explorer/Audio/crash.wav");
    level_up_sound = LoadSound("D:/Maze-explorer/Audio/level_up.mp3");

    PlayMusicStream(backgrnd_music);

    while (!WindowShouldClose())
    {

        UpdateMusicStream(backgrnd_music);

        BeginDrawing();
        ClearBackground(DARKBLUE);

        DrawTexturePro(space_background,
                       (Rectangle){0, 0, space_background.width, space_background.height},
                       (Rectangle){0, 0, screen_width, screen_height},
                       (Vector2){0, 0}, 0.0f, WHITE);

        start_gameplay();

        EndDrawing();
    }

    UnloadMusicStream(backgrnd_music);
    UnloadSound(clicksound);
    CloseAudioDevice();

    UnloadTexture(planet);
    UnloadTexture(space_background);
    for (int i = 0; i < CNT; i++)
    {
        UnloadTexture(rocketTex[i]);
    }

    CloseWindow();
}