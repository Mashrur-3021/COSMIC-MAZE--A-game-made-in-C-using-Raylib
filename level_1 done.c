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

Texture2D rocketTex[CNT];

const char *rocketPics[CNT] = {
    "D:/Maze-explorer/rocket/1.png",
    "D:/Maze-explorer/rocket/2.png",
    "D:/Maze-explorer/rocket/3.png",
    "D:/Maze-explorer/rocket/4.png"};

Texture2D space_background;
Texture2D moon02;

const Vector2 rocket_position[] = {{1, 2}};
Vector2 moon_position[] = {{9, 9}};
Player Rocket[] = {
    {rocketSpeed, DOWN, rocket_position[0]}};

char *game_title = "MAZE EXPLORER";
char *play_message = "PLAY";

Font font_play;
Vector2 play_button_pos;
Vector2 game_title_pos;
Rectangle play_button_posRec;
Rectangle game_title_posRec;
static Vector2 mousepos;

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
                if (!hitWall(next_pos, wall_level1, sizeof(wall_level1) / sizeof(wall_level1[0])))
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
                if (!hitWall(next_pos, wall_level1, sizeof(wall_level1) / sizeof(wall_level1[0])))
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
                if (!hitWall(next_pos, wall_level1, sizeof(wall_level1) / sizeof(wall_level1[0])))
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
                if (!hitWall(next_pos, wall_level1, sizeof(wall_level1) / sizeof(wall_level1[0])))
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
    space_background = LoadTexture("D:/Maze-explorer/Background/1.png");
    moon02 = LoadTexture("D:/Maze-explorer/Planets/planet.png");
}

void start_gameplay()
{
    static int level = 0;

    switch (level)
    {
    case 0:

        mousepos = GetMousePosition();
        DrawRectangleRounded(play_button_posRec, 1.0f, 8, YELLOW);
        DrawRectangleRoundedLinesEx(play_button_posRec, 1.0f, 8, 2, BLACK);
        DrawRectangleRounded(game_title_posRec, 1.0f, 8, YELLOW);
        DrawRectangleRoundedLinesEx(game_title_posRec, 1.0f, 8, 4, BLACK);

        DrawTextEx(font_play, play_message, play_button_pos, (float)font_play.baseSize + 20, 2, RED);
        DrawTextEx(font_play, game_title, game_title_pos, (float)font_play.baseSize + 40, 2, (Color){250, 2, 100, 255});

        if (CheckCollisionPointRec(mousepos, play_button_posRec) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            level = 1;
        }

        break;

    case 1:

        DrawTexturePro(moon02,
                       (Rectangle){0, 0, moon02.width, moon02.height},
                       (Rectangle){moon_position[0].x * CELL, moon_position[0].y * CELL, moonSize, moonSize},
                       (Vector2){0, 0}, 0.0f, WHITE);

        DrawTexturePro(rocketTex[Rocket[0].dir],
                       (Rectangle){0, 0, rocketTex[Rocket[0].dir].width, rocketTex[Rocket[0].dir].height},
                       (Rectangle){Rocket[0].pos.x * CELL, Rocket[0].pos.y * CELL, rocketSize, rocketSize},
                       (Vector2){0, 0}, 0.0f, WHITE);

        for (int i = 0; i < wallCount1; i++)
        {
            DrawWall(wall_level1[i]);
        }

        updateRocket(&Rocket[0]);

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

    UnloadTexture(moon02);
    UnloadTexture(space_background);
    for (int i = 0; i < CNT; i++)
    {
        UnloadTexture(rocketTex[i]);
    }

    CloseWindow();
}