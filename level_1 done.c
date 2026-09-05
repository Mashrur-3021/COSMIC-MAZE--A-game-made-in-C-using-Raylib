#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"
#include "raymath.h"
#define CELL 60
#define THICK 15
#define rocketSpeed 2
#define rocketSize 30
#define moonSize 55
const int screen_height = 900;
const int screen_width = 900;

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

const Vector2 rocket_position[] = {{1, 2}, {6, 13}, {1, 14}};
Vector2 planet_position[] = {{9, 9}, {7, 7}, {10, 1}};
// vector arrays

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

float font_size = 30;
float spacing = 2;

// wall levels
Wall wall_level1[] = {
    {0, 0, 15, 0},
    {0, 15, 15, 15},
    {0, 0, 0, 15},
    {15, 0, 15, 15},
    {2, 3, 3, 3},
    {3, 3, 4, 3},
    {4, 3, 5, 3},
    {5, 3, 6, 3},
    {6, 3, 7, 3},
    {7, 3, 8, 3},
    {8, 3, 9, 3},
    {9, 3, 10, 3},
    {10, 3, 11, 3},
    {14, 3, 15, 3},
    {13, 3, 14, 3},
    {12, 3, 13, 3},
    {11, 3, 12, 3},
    {2, 3, 2, 4},
    {2, 4, 2, 5},
    {4, 5, 5, 5},
    {5, 5, 6, 5},
    {6, 5, 7, 5},
    {7, 5, 8, 5},
    {9, 5, 10, 5},
    {10, 5, 11, 5},
    {11, 5, 12, 5},
    {12, 5, 13, 5},
    {13, 5, 13, 6},
    {13, 6, 13, 7},
    {13, 7, 13, 8},
    {10, 8, 11, 8},
    {9, 8, 10, 8},
    {7, 8, 8, 8},
    {8, 8, 9, 8},
    {6, 8, 7, 8},
    {5, 8, 6, 8},
    {4, 8, 5, 8},
    {3, 8, 4, 8},
    {2, 8, 3, 8},
    {2, 8, 2, 9},
    {2, 9, 2, 10},
    {2, 10, 2, 11},
    {3, 10, 3, 11},
    {2, 11, 3, 11},
    {3, 11, 3, 12},
    {2, 11, 2, 12},
    {2, 12, 2, 13},
    {2, 13, 2, 14},
    {5, 11, 6, 11},
    {6, 11, 7, 11},
    {7, 11, 8, 11},
    {8, 11, 9, 11},
    {9, 11, 10, 11},
    {10, 11, 11, 11},
    {9, 10, 9, 11},
    {9, 11, 9, 12},
    {9, 12, 9, 13},
    {9, 13, 10, 13},
    {10, 13, 11, 13},
    {11, 13, 12, 13},
    {12, 13, 12, 14},
    {5, 10, 6, 10},
    {6, 10, 7, 10},
    {7, 10, 7, 11},
    {5, 6, 6, 6},
    {6, 5, 6, 6},
    {7, 7, 7, 8},
    {6, 7, 7, 7},
    {9, 7, 10, 7},
    {0, 8, 1, 8},
    {1, 8, 2, 8},
    {5, 7, 6, 7},
    {10, 7, 11, 7},
    {9, 6, 9, 7},
    {9, 5, 9, 6},
    {11, 6, 11, 7},
};

int wallCount1 = sizeof(wall_level1) / sizeof(Wall);

Wall wall_level2[] = {
    {0, 0, 15, 0},
    {0, 15, 15, 15},
    {0, 0, 0, 15},
    {15, 0, 15, 15},
    {0, 1, 1, 1},
    {1, 1, 2, 1},
    {3, 0, 3, 1},
    {3, 1, 3, 2},
    {2, 2, 3, 2},
    {1, 2, 2, 2},
    {1, 2, 1, 3},
    {1, 3, 2, 3},
    {2, 3, 3, 3},
    {3, 3, 4, 3},
    {4, 2, 4, 3},
    {0, 4, 1, 4},
    {1, 4, 2, 4},
    {3, 4, 3, 5},
    {3, 5, 3, 6},
    {3, 6, 3, 7},
    {2, 6, 3, 6},
    {2, 5, 2, 6},
    {3, 7, 4, 7},
    {4, 6, 4, 7},
    {4, 5, 4, 6},
    {4, 5, 5, 5},
    {0, 6, 1, 6},
    {1, 6, 1, 7},
    {1, 7, 1, 8},
    {1, 8, 2, 8},
    {2, 8, 3, 8},
    {3, 8, 4, 8},
    {4, 8, 5, 8},
    {5, 7, 5, 8},
    {5, 6, 5, 7},
    {5, 8, 5, 9},
    {1, 8, 1, 9},
    {1, 9, 2, 9},
    {2, 9, 3, 9},
    {2, 9, 2, 10},
    {0, 10, 1, 10},
    {1, 10, 1, 11},
    {1, 11, 1, 12},
    {1, 12, 2, 12},
    {2, 12, 2, 13},
    {1, 13, 2, 13},
    {1, 13, 1, 14},
    {5, 6, 6, 6},
    {4, 2, 5, 2},
    {4, 1, 4, 2},
    {4, 1, 5, 1},
    {5, 1, 6, 1},
    {6, 1, 7, 1},
    {6, 0, 6, 1},
    {5, 2, 5, 3},
    {5, 3, 6, 3},
    {6, 3, 7, 3},
    {6, 3, 6, 4},
    {6, 4, 7, 4},
    {6, 4, 6, 5},
    {7, 1, 7, 2},
    {7, 2, 8, 2},
    {8, 2, 9, 2},
    {9, 2, 9, 3},
    {6, 5, 7, 5},
    {7, 5, 8, 5},
    {8, 4, 8, 5},
    {9, 3, 10, 3},
    {8, 4, 9, 4},
    {6, 6, 6, 7},
    {6, 7, 7, 7},
    {7, 7, 7, 8},
    {7, 8, 7, 9},
    {3, 9, 3, 10},
    {3, 10, 4, 10},
    {4, 10, 4, 11},
    {4, 10, 5, 10},
    {5, 10, 6, 10},
    {5, 10, 5, 11},
    {5, 11, 5, 12},
    {5, 12, 5, 13},
    {4, 12, 5, 12},
    {3, 12, 4, 12},
    {3, 11, 3, 12},
    {1, 11, 2, 11},
    {3, 12, 3, 13},
    {3, 13, 3, 14},
    {3, 14, 3, 15},
    {2, 14, 3, 14},
    {3, 13, 4, 13},
    {4, 13, 4, 14},
    {4, 14, 5, 14},
    {5, 14, 6, 14},
    {6, 10, 7, 10},
    {7, 10, 7, 11},
    {6, 11, 7, 11},
    {6, 11, 6, 12},
    {6, 12, 7, 12},
    {7, 12, 8, 12},
    {8, 12, 9, 12},
    {8, 12, 8, 13},
    {8, 13, 9, 13},
    {9, 13, 10, 13},
    {10, 13, 11, 13},
    {11, 13, 11, 14},
    {11, 13, 12, 13},
    {11, 14, 12, 14},
    {7, 13, 8, 13},
    {8, 13, 8, 14},
    {8, 14, 9, 14},
    {9, 14, 10, 14},
    {7, 14, 7, 15},
    {9, 4, 10, 4},
    {10, 2, 10, 3},
    {10, 1, 10, 2},
    {10, 1, 11, 1},
    {11, 1, 12, 1},
    {12, 1, 12, 2},
    {12, 2, 12, 3},
    {12, 3, 13, 3},
    {13, 3, 14, 3},
    {12, 1, 13, 1},
    {13, 1, 14, 1},
    {10, 0, 10, 1},
    {12, 2, 13, 2},
    {10, 4, 10, 5},
    {9, 5, 10, 5},
    {9, 5, 9, 6},
    {9, 7, 9, 8},
    {8, 7, 9, 7},
    {9, 8, 10, 8},
    {10, 8, 11, 8},
    {11, 7, 11, 8},
    {11, 6, 11, 7},
    {11, 5, 11, 6},
    {11, 4, 11, 5},
    {11, 4, 12, 4},
    {12, 4, 13, 4},
    {13, 4, 14, 4},
    {9, 8, 9, 9},
    {11, 8, 11, 9},
    {11, 9, 11, 10},
    {11, 8, 12, 8},
    {11, 7, 12, 7},
    {12, 4, 12, 5},
    {12, 5, 13, 5},
    {13, 5, 14, 5},
    {12, 5, 12, 6},
    {12, 6, 13, 6},
    {13, 6, 13, 7},
    {12, 8, 12, 9},
    {12, 9, 12, 10},
    {6, 9, 7, 9},
    {4, 9, 4, 10},
    {7, 11, 8, 11},
    {8, 11, 9, 11},
    {10, 12, 10, 13},
    {10, 10, 10, 11},
    {9, 9, 10, 9},
    {9, 10, 9, 11},
    {10, 10, 11, 10},
    {12, 10, 12, 11},
    {12, 11, 13, 11},
    {13, 11, 13, 12},
    {13, 12, 14, 12},
    {13, 13, 14, 13},
    {14, 13, 14, 14},
    {8, 9, 9, 9},
    {10, 11, 11, 11},
    {11, 11, 11, 12},
    {11, 12, 12, 12},
    {13, 12, 13, 13},
    {12, 14, 13, 14},
    {13, 7, 13, 8},
    {14, 5, 14, 6},
    {14, 7, 15, 7},
    {14, 7, 14, 8},
    {14, 8, 14, 9},
    {14, 9, 14, 10},
    {13, 9, 14, 9},
    {14, 10, 14, 11},
    {9, 9, 9, 10},
};

int wallCount2 = sizeof(wall_level2) / sizeof(Wall);

Wall wall_level3[] = {
    {0, 0, 15, 0},
    {0, 15, 15, 15},
    {0, 0, 0, 15},
    {15, 0, 15, 15},
    {1, 1, 2, 1},
    {2, 2, 2, 3},
    {2, 1, 3, 1},
    {3, 1, 3, 2},
    {2, 2, 3, 2},
    {1, 3, 2, 3},
    {1, 3, 1, 4},
    {0, 4, 1, 4},
    {1, 5, 1, 6},
    {1, 6, 2, 6},
    {2, 6, 3, 6},
    {2, 6, 2, 7},
    {2, 7, 2, 8},
    {2, 8, 3, 8},
    {3, 8, 4, 8},
    {4, 8, 4, 9},
    {4, 9, 5, 9},
    {5, 9, 5, 10},
    {5, 10, 5, 11},
    {4, 11, 5, 11},
    {3, 11, 4, 11},
    {3, 10, 3, 11},
    {3, 11, 3, 12},
    {2, 11, 2, 12},
    {1, 12, 2, 12},
    {1, 12, 1, 13},
    {2, 10, 3, 10},
    {2, 14, 3, 14},
    {3, 13, 3, 14},
    {3, 13, 4, 13},
    {4, 13, 5, 13},
    {5, 13, 5, 14},
    {5, 14, 6, 14},
    {6, 13, 6, 14},
    {6, 12, 6, 13},
    {6, 12, 7, 12},
    {7, 12, 8, 12},
    {8, 12, 8, 13},
    {8, 13, 8, 14},
    {8, 13, 9, 13},
    {9, 13, 10, 13},
    {10, 13, 11, 13},
    {10, 12, 10, 13},
    {10, 11, 10, 12},
    {9, 10, 9, 11},
    {9, 10, 10, 10},
    {10, 10, 11, 10},
    {7, 10, 8, 10},
    {7, 11, 8, 11},
    {6, 11, 7, 11},
    {6, 10, 6, 11},
    {6, 9, 6, 10},
    {6, 9, 7, 9},
    {7, 8, 7, 9},
    {7, 7, 7, 8},
    {7, 7, 8, 7},
    {8, 7, 9, 7},
    {8, 6, 8, 7},
    {7, 6, 8, 6},
    {6, 6, 7, 6},
    {6, 5, 6, 6},
    {4, 8, 5, 8},
    {5, 7, 5, 8},
    {4, 7, 5, 7},
    {4, 6, 4, 7},
    {4, 5, 4, 6},
    {3, 5, 4, 5},
    {3, 4, 3, 5},
    {3, 4, 4, 4},
    {6, 5, 7, 5},
    {1, 9, 2, 9},
    {1, 9, 1, 10},
    {5, 3, 5, 4},
    {5, 2, 5, 3},
    {4, 3, 5, 3},
    {4, 1, 4, 2},
    {4, 1, 5, 1},
    {5, 1, 6, 1},
    {6, 1, 7, 1},
    {6, 1, 6, 2},
    {7, 3, 8, 3},
    {7, 3, 7, 4},
    {7, 4, 8, 4},
    {8, 4, 8, 5},
    {8, 5, 9, 5},
    {9, 5, 9, 6},
    {9, 5, 10, 5},
    {10, 4, 10, 5},
    {6, 3, 7, 3},
    {8, 9, 9, 9},
    {9, 9, 9, 10},
    {11, 9, 11, 10},
    {11, 9, 12, 9},
    {12, 10, 13, 10},
    {12, 10, 12, 11},
    {12, 11, 12, 12},
    {11, 12, 12, 12},
    {11, 13, 12, 13},
    {13, 12, 13, 13},
    {13, 11, 13, 12},
    {13, 11, 14, 11},
    {11, 14, 12, 14},
    {10, 14, 10, 15},
    {8, 14, 9, 14},
    {7, 14, 8, 14},
    {6, 13, 7, 13},
    {4, 12, 5, 12},
    {6, 7, 6, 8},
    {5, 5, 5, 6},
    {0, 7, 1, 7},
    {0, 11, 1, 11},
    {9, 3, 10, 3},
    {9, 2, 9, 3},
    {9, 1, 9, 2},
    {9, 1, 10, 1},
    {8, 2, 9, 2},
    {9, 8, 9, 9},
    {9, 8, 10, 8},
    {10, 8, 11, 8},
    {11, 7, 11, 8},
    {11, 7, 12, 7},
    {10, 7, 11, 7},
    {10, 6, 10, 7},
    {10, 5, 11, 5},
    {11, 5, 12, 5},
    {11, 4, 11, 5},
    {11, 4, 12, 4},
    {12, 3, 12, 4},
    {12, 3, 13, 3},
    {12, 2, 12, 3},
    {11, 3, 12, 3},
    {12, 2, 13, 2},
    {13, 2, 14, 2},
    {11, 6, 12, 6},
    {12, 6, 13, 6},
    {13, 6, 13, 7},
    {13, 7, 13, 8},
    {12, 8, 13, 8},
    {13, 9, 14, 9},
    {14, 9, 14, 10},
    {13, 14, 14, 14},
    {14, 12, 14, 13},
    {14, 8, 14, 9},
    {14, 8, 15, 8},
    {13, 6, 14, 6},
    {14, 5, 14, 6},
    {14, 4, 14, 5},
    {14, 4, 15, 4},
    {14, 3, 14, 4},
    {13, 4, 13, 5},
    {13, 1, 13, 2},
    {12, 1, 13, 1},
};
int wallCount3 = sizeof(wall_level3) / sizeof(Wall);

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
        DrawRectangle(x1 - half, top - half, THICK, height + THICK, (Color){37, 42, 82, 255});
    }
    else
    {
        int left = (x1 < x2) ? x1 : x2;
        int width = abs(x2 - x1);
        DrawRectangle(left - half, y1 - half, width + THICK, THICK, (Color){37, 42, 82, 255});
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
                                screen_height * 2 / 3 - MeasureTextEx(font_play, play_message, (float)font_play.baseSize, 2).y / 2};

    game_title_pos = (Vector2){screen_width / 2 - MeasureTextEx(font_play, game_title, (float)font_play.baseSize + 40, 4).x / 2,
                               screen_height / 10 - MeasureTextEx(font_play, game_title, (float)font_play.baseSize + 40, 4).y / 2};

    float paddingX = 20;
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
    case 0:

        mousepos = GetMousePosition();
        DrawRectangleRounded(play_button_posRec, 1.0f, 8, (Color){10, 15, 40, 255});
        DrawRectangleRoundedLinesEx(play_button_posRec, 1.0f, 8, 2, BLACK);
        DrawRectangleRounded(game_title_posRec, 1.0f, 8, (Color){10, 15, 40, 255});
        DrawRectangleRoundedLinesEx(game_title_posRec, 1.0f, 8, 4, BLACK);

        DrawTextEx(font_play, play_message, play_button_pos, (float)font_play.baseSize + 20, 2, BLUE);
        DrawTextEx(font_play, game_title, game_title_pos, (float)font_play.baseSize + 40, 2, BLUE);

        if (CheckCollisionPointRec(mousepos, play_button_posRec) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            level = 1;
        }

        break;

    case 1:

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
            level = 2;
        }
        break;

    case 2:
        mousepos = GetMousePosition();
        DrawRectangleRounded(message_box, 1.0f, 8, (Color){10, 15, 40, 255});
        DrawRectangleRoundedLinesEx(message_box, 1.0f, 8, 2, BLACK);

        DrawTextEx(font_play, transition_msg1, message1_pos, font_size, spacing, BLUE);
        DrawTextEx(font_play, transition_msg2, message2_pos, font_size, spacing, BLUE);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) // শুধু ক্লিক হচ্ছে কিনা টেস্ট
        {
            printf("click at (%.1f, %.1f) | box: (%.1f,%.1f,%.1f,%.1f)\n",
                   mousepos.x, mousepos.y,
                   message_box.x, message_box.y, message_box.width, message_box.height);
        }

        if (CheckCollisionPointRec(mousepos, message_box) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            rocket.pos = rocket_position[1];
            rocket.dir = DOWN;
            level = 3;
        }

        break;

    case 3:

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
            level = 4;
        }
        break;

    case 4:
        mousepos = GetMousePosition();
        DrawRectangleRounded(message_box2, 1.0f, 8, (Color){10, 15, 40, 255});
        DrawRectangleRoundedLinesEx(message_box2, 1.0f, 8, 2, BLACK);

        DrawTextEx(font_play, transition2_msg1, message3_pos, font_size, spacing, BLUE);
        DrawTextEx(font_play, transition2_msg2, message4_pos, font_size, spacing, BLUE);

        if (CheckCollisionPointRec(mousepos, message_box2) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            rocket.pos = rocket_position[2];
            rocket.dir = DOWN;
            level = 5;
        }

        break;

    case 5:

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
            level = 6;
        }
        break;

    case 6:
        mousepos = GetMousePosition();
        DrawRectangleRounded(message_box3, 1.0f, 8, DARKBLUE);
        DrawRectangleRoundedLinesEx(message_box3, 1.0f, 8, 2, BLACK);

        DrawTextEx(font_play, transition3_msg1, message5_pos, font_size, spacing, BLUE);
        DrawTextEx(font_play, transition3_msg2, message6_pos, font_size, spacing, BLUE);

        if (CheckCollisionPointRec(mousepos, message_box3) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            level = 7;
        }

        break;

    default:
        break;
    }
}
int main()
{
    InitWindow(screen_width, screen_height, "Maze solver");
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

    while (!WindowShouldClose())
    {

        BeginDrawing();
        ClearBackground(DARKBLUE);

        DrawTexturePro(space_background,
                       (Rectangle){0, 0, space_background.width, space_background.height},
                       (Rectangle){0, 0, screen_width, screen_height},
                       (Vector2){0, 0}, 0.0f, WHITE);

        start_gameplay();

        EndDrawing();
    }

    UnloadTexture(planet);
    UnloadTexture(space_background);
    for (int i = 0; i < CNT; i++)
    {
        UnloadTexture(rocketTex[i]);
    }

    CloseWindow();
}