#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"
#include "raymath.h"
#define CELL 60
#define THICK 15
#define rocketSpeed 2
#define rocketSize 30
#define moonSize 55
const int HEIGHT = 900;
const int WIDTH = 900;

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

Texture2D rocketTex[CNT];
Direction current_dir = UP;

const char *rocketPics[CNT] = {
    "D:/Raylib project/rocket/1.png",
    "D:/Raylib project/rocket/2.png",
    "D:/Raylib project/rocket/3.png",
    "D:/Raylib project/rocket/4.png"};

Vector2 rocket_position = {1, 2};
Vector2 moon_position = {9, 9};

Wall level1[] = {
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

void DrawWall(Wall w)
{
    int x1 = w.x1 * CELL, y1 = w.y1 * CELL;
    int x2 = w.x2 * CELL, y2 = w.y2 * CELL;
    int half = THICK / 2;

    if (x1 == x2)
    {
        int top = (y1 < y2) ? y1 : y2;
        int height = abs(y2 - y1);
        DrawRectangle(x1 - half, top - half, THICK, height + THICK, BLACK);
    }
    else
    {
        int left = (x1 < x2) ? x1 : x2;
        int width = abs(x2 - x1);
        DrawRectangle(left - half, y1 - half, width + THICK, THICK, BLACK);
    }
}

bool hitWall(Vector2 rocket_pos, Wall *level, int n)
{
    Rectangle rocketRec = {rocket_pos.x * CELL, rocket_pos.y * CELL, rocketSize + 5, rocketSize + 5};

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

void updateRocket(Player *rocket)
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
                if (!hitWall(next_pos, level1, sizeof(level1) / sizeof(level1[0])))
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
                if (!hitWall(next_pos, level1, sizeof(level1) / sizeof(level1[0])))
                {
                    rocket->pos = next_pos;
                    if (rocket->pos.y > HEIGHT)
                    {
                        rocket->pos.y = HEIGHT;
                    }
                }
                break;

            case RIGHT:
                next_pos = rocket->pos;
                next_pos.x = rocket->pos.x + rocket->speed * dt;
                if (!hitWall(next_pos, level1, sizeof(level1) / sizeof(level1[0])))
                {
                    rocket->pos = next_pos;
                    if (rocket->pos.x > WIDTH)
                    {
                        rocket->pos.x = WIDTH;
                    }
                }
                break;

            case LEFT:
                next_pos = rocket->pos;
                next_pos.x = rocket->pos.x - rocket->speed * dt;
                if (!hitWall(next_pos, level1, sizeof(level1) / sizeof(level1[0])))
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

int main()
{
    InitWindow(WIDTH, HEIGHT, "Maze solver");
    SetTargetFPS(60);

    Texture2D space_background = LoadTexture("D:/Raylib project/Background/1.png");
    Texture2D moon02 = LoadTexture("D:/Raylib project/Planets/moon_02.png");

    Player Rocket = {rocketSpeed, DOWN, rocket_position};

    for (int i = 0; i < CNT; i++)
    {
        rocketTex[i] = LoadTexture(rocketPics[i]);
    }

    while (!WindowShouldClose())
    {

        BeginDrawing();
        ClearBackground(DARKBLUE);

        int wallCount = sizeof(level1) / sizeof(Wall);

        Rectangle source = {0, 0, space_background.width, space_background.height};
        Rectangle dest = {0, 0, WIDTH, HEIGHT};
        Vector2 origin = {0, 0};

        DrawTexturePro(space_background, source, dest, origin, 0.0f, WHITE);

        DrawTexturePro(moon02,
                       (Rectangle){0, 0, moon02.width, moon02.height},
                       (Rectangle){moon_position.x * CELL, moon_position.y * CELL, moonSize, moonSize},
                       (Vector2){0, 0}, 0.0f, WHITE);

        DrawTexturePro(rocketTex[Rocket.dir],
                       (Rectangle){0, 0, rocketTex[Rocket.dir].width, rocketTex[Rocket.dir].height},
                       (Rectangle){Rocket.pos.x * CELL, Rocket.pos.y * CELL, rocketSize, rocketSize},
                       (Vector2){0, 0}, 0.0f, WHITE);

        for (int i = 0; i < wallCount; i++)
        {
            DrawWall(level1[i]);
        }

        updateRocket(&Rocket);

        EndDrawing();
    }

    UnloadTexture(moon02);
    unloadtexture(space_background);
    for (int i = 0; i < CNT; i++)
    {
        UnloadTexture(rocketTex[i]);
    }

    CloseWindow();
}