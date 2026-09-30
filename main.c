#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "raylib.h"
#include "raymath.h"
#include "Wall.h"

#define LEVEL_COUNT 4
#define CELL 60
#define THICK 15
#define rocketSpeed 3.25
#define rocketSize 27
#define moonSize 42
#define meteorSize 34
#define BLACKHOLE_RADIUS_CELLS 0.45f
#define BLACKHOLE_COOLDOWN 0.6f
#define PARTICLES_PER_EMITTER 1

#define FLAME_SPACING 90.0f
#define FLAME_RISE_HEIGHT 22.0f
#define PLANET_WALL_THICK 6
#define MAX_NAME_LEN 16

#define screen_height 900
#define screen_width 1500

Color Button_color = (Color){10, 15, 40, 255};
int MAX_LEVEL_EMITTERS = 700;

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

typedef struct
{
    float speed;
    Vector2 pos;
    int currentIndex;
    int direction;
    Vector2 *path;
    int pathSize;
} spaceShip;

typedef enum
{
    ZERO_WINDOW,
    CREDENTIAL,
    NAME_INPUT,
    RULES,
    LEVEL_1,
    TR_WIN_1,
    LEVEL_2,
    TR_WIN_2,
    LEVEL_3,
    TR_WIN_3,
    LEVEL4,
    TR_WIN_4,
    TOTAL_WINDOW,
    LEADERBOARD_WINDOW,
    HOW_TO_PLAY_WINDOW,
} WINDOW_NAME;

typedef enum
{
    KEY_RED,
    KEY_GREEN,
    KEY_YELLOW,
    KEY_BLUE,
    KEY_TYPE_COUNT,
} KeyType;

typedef struct
{
    Vector2 pos;
    KeyType type;
    bool collected;
} Key;

typedef struct
{
    float age;
    float maxLife;
    float phase;
    float speed;
    float amplitude;
    float size;
    bool active;
} FlameParticle;

typedef struct
{
    Vector2 basePos;
    float spawnTimer;
    FlameParticle particles[PARTICLES_PER_EMITTER];
} FlameEmitter;

const char *rocketPics[CNT] = {
    "D:/Maze-explorer/rocket/1.png",
    "D:/Maze-explorer/rocket/2.png",
    "D:/Maze-explorer/rocket/3.png",
    "D:/Maze-explorer/rocket/4.png"};

Texture2D rocketTex[CNT];
Texture2D space_background;
Texture2D space_background2;
Texture2D space_background3;
Texture2D space_background4;
Texture2D space_background5;
Texture2D planet;
Texture2D meteor;
Texture2D alien_spaceship;

const Vector2 rocket_position[] = {{2, 9}, {2, 2}, {3, 4}, {2, 11}};
Vector2 planet_position[] = {{23, 8}, {22, 2}, {15, 13}, {22, 2}};

Vector2 meteors_level2[] = {{6, 12}, {24, 6}, {24, 10}, {22, 14}, {17, 9}, {21, 4}, {4, 14}};
int meteors_level2_size = sizeof(meteors_level2) / sizeof(Vector2);
Vector2 meteors_level3[] = {
    {18, 4}, {8, 8}, {1, 13}, {24, 9}, {16, 8}, {11, 12}, {7, 10}, {16, 6}, {17, 13}, {15, 11}, {20, 2}, {13, 2}, {21, 12}, {5, 12}, {19, 10}, {20, 24}, {23, 33}, {12, 10}, {11, 6}};
int meteors_level3_size = sizeof(meteors_level3) / sizeof(Vector2);
Vector2 meteors_level4[] = {{12, 8}, {5, 5}};

Vector2 blackholes_level1[2] = {{6, 10}, {22, 5}};
Vector2 blackholes_level2[2] = {{5, 12}, {19, 5}};
Vector2 blackholes_level3[2] = {{2, 4}, {21, 10}};
Vector2 blackholes_level4[2] = {{5, 11}, {22, 5}};

float teleportCooldown = 0.0f;
float blackholeAnimTime = 0.0f;

Key keys_level1[KEY_TYPE_COUNT];
Key keys_level2[KEY_TYPE_COUNT];
Key keys_level3[KEY_TYPE_COUNT];
Key keys_level4[KEY_TYPE_COUNT];

Wall redWalls_level1[4];
Wall redWalls_level2[4];
Wall redWalls_level3[4];
Wall redWalls_level4[4];

Player rocket = {rocketSpeed, DOWN, rocket_position[0]};

spaceShip alienShip1 = {
    .speed = 2.0f,
    .pos = {11, 14},
    .currentIndex = 0,
    .direction = 1,
    .path = alien_path1,
    .pathSize = 19};

spaceShip alienShip2 = {
    .speed = 2.0f,
    .pos = {12, 1},
    .currentIndex = 0,
    .direction = 1,
    .path = alien_path2,
    .pathSize = 11};

void ResetSpaceShips(void)
{
    alienShip1 = (spaceShip){
        .speed = 2.0f,
        .pos = {alien_path1[0].x, alien_path1[0].y},
        .currentIndex = 0,
        .direction = 1,
        .path = alien_path1,
        .pathSize = alien_path1_size};

    alienShip2 = (spaceShip){
        .speed = 2.0f,
        .pos = {alien_path2[0].x, alien_path2[0].y},
        .currentIndex = 0,
        .direction = 1,
        .path = alien_path2,
        .pathSize = alien_path2_size};
}

char playerName[MAX_NAME_LEN + 1] = "\0";
int nameLetterCount = 0;

char *game_title = "COSMIC MAZE";
char *play_message = "PLAY";
char *name_label = "Name: ";
char *transition_msg1 = "Level-1 Done!!!";
char *transition_msg2 = "Click to Move in next level";
char *transition2_msg1 = "Level-2 Done!!!";
char *transition2_msg2 = "Click to Move in next level";
char *transition3_msg1 = "Level-3 Done!!!";
char *transition3_msg2 = "Click to Move in next level";
char *transition4_msg1 = "Level-4 Done!!!";
char *transition4_msg2 = "     Click to See Results";
char *credential_title = "CREDENTIAL";
char *credential_name1 = "Mujahidul Islam Nafi - 2505095";
char *credential_name2 = "Md. Mashrur Hasan - 2505115";
char *back_message = "BACK";
char rules_line1[100];
char *rules_line2 = "You step into the uncharted depths of a cosmic maze.";
char *rules_line3 = "Across four forgotten worlds lie four celestial keys.";
char *rules_line4 = "Shatter the dark void to claim your ultimate freedom.";
char *rules_line5 = "Navigate abyssal corridors as meteor storms rage on.";
char *rules_line6 = "Beware the alien watchers lurking silently in shadows.";
char *rules_line7 = "Touching temporal fields will fracture time itself,";
char *rules_line8 = "casting your ship back to where your journey started.";
char *rules_line9 = "Are you prepared to face the mysteries of the dark?";

char *start_button_message = "START";

char *supervisor_label = "SUPERVISOR-";
char *supervisor_name = "ABU BASHIR SHUAIB SIR";

Vector2 supervisor_label_pos;
Vector2 supervisor_name_pos;

Rectangle rules_box;
Vector2 rules_line_pos[9];
Vector2 start_button_pos;
Rectangle start_button_posRec;

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

Vector2 credential_name1_pos;
Vector2 credential_name2_pos;
Vector2 credential_back_pos;
Rectangle credential_back_posRec;
Vector2 credentialButton_pos;
Rectangle credentialButton_posRec;

char *leaderboard_button_message = "LEADERBOARD";
Vector2 leaderboard_button_pos;
Rectangle leaderboard_button_posRec;

char *leaderboard_title = "LEADERBOARD";
Vector2 leaderboard_title_pos;

char *leaderboard_back_message = "BACK";
Vector2 leaderboard_back_pos;
Rectangle leaderboard_back_posRec;

char *howto_button_message = "HOW TO PLAY";
Vector2 howto_button_pos;
Rectangle howto_button_posRec;

char *howto_title = "HOW TO PLAY";
Vector2 howto_title_pos;
Vector2 howto_back_pos;
Rectangle howto_back_posRec;
Rectangle howto_box;

#define HOWTO_LINE_COUNT 8
char *howto_lines[HOWTO_LINE_COUNT] = {
    "1. Move the rocket with the ARROW keys or W, A, S, D.",
    "2. Press SPACEBAR to pause the game. Press it again to resume.",
    "3. First, collect all 4 keys in the level.",
    "4. After the 4th key, the red walls around the planet will open.",
    "5. Avoid crashing into meteors and walls.",
    "6. If you touch an alien spaceship, you restart from the beginning.",
    "7. Black holes come in pairs. Enter one to teleport to the other.",
    "8. Land on the planet to complete the level."};
Vector2 howto_line_pos[HOWTO_LINE_COUNT];

Vector2 name_label_pos;
Vector2 name_input_text_pos;
Rectangle name_label_box;
Rectangle name_input_box;

Vector2 main_exit_button_pos;
Rectangle main_exit_button_posRec;

Music backgrnd_music;
Sound clicksound;
Sound crashsound;
Sound level_up_sound;

float font_size = 30;
float spacing = 2;

FlameEmitter flameEmitters1[700];
FlameEmitter flameEmitters2[700];
FlameEmitter flameEmitters3[700];
FlameEmitter flameEmitters4[700];
int flameEmitterCount1 = 0;
int flameEmitterCount2 = 0;
int flameEmitterCount3 = 0;
int flameEmitterCount4 = 0;

char score_title[64];
Vector2 score_title_pos;

Vector2 level_label_pos[LEVEL_COUNT];
Vector2 level_time_pos[LEVEL_COUNT];
Vector2 avg_time_pos;
char *exit_message = "EXIT";
char *play_again_message = "PLAY AGAIN";

Vector2 exit_button_pos;
Rectangle exit_button_posRec;
bool musicOn = true;
char *music_on_message = "MUSIC: ON";
char *music_off_message = "MUSIC: OFF";
Vector2 music_button_pos;
Rectangle music_button_posRec;

Vector2 playagain_button_pos;
Rectangle playagain_button_posRec;

bool exitGameRequested = false;
bool gamePaused = false;
char *pause_message = "PAUSED";
char *resume_button_message = "RESUME";
char *exit_game_button_message = "EXIT GAME";
static int level = 0;
Rectangle pause_box;
Vector2 pause_message_pos;
Vector2 resume_button_pos;
Rectangle resume_button_posRec;
Vector2 exit_game_button_pos;
Rectangle exit_game_button_posRec;
Rectangle credential_box;

static Color GetButtonColor(Rectangle rect, Vector2 mouse, Color baseColor)
{
    return CheckCollisionPointRec(mouse, rect) ? WHITE : baseColor;
}

const char *leaderboardFilePath = "D:/Maze-explorer/leaderboard.txt";

double levelElapsedTime = 0.0;
bool timerRunning = false;
int currentLevelNumber = 1;
float levelTimes[LEVEL_COUNT] = {0};
float bestLevelTimes[LEVEL_COUNT] = {0};
void LoadLeaderboard(void)
{
    FILE *f = fopen(leaderboardFilePath, "r");
    if (f == NULL)
        return;

    for (int i = 0; i < LEVEL_COUNT; i++)
    {
        if (fscanf(f, "%f", &bestLevelTimes[i]) != 1)
        {
            bestLevelTimes[i] = 0.0f;
            break;
        }
    }
    fclose(f);
}

void SaveLeaderboard(void)
{
    FILE *f = fopen(leaderboardFilePath, "w");
    if (f == NULL)
        return;

    for (int i = 0; i < LEVEL_COUNT; i++)
        fprintf(f, "%.2f\n", bestLevelTimes[i]);
    fclose(f);
}

typedef struct
{
    char name[MAX_NAME_LEN + 1];
    float avgTime;
} PlayerScore;

#define MAX_LEADERBOARD_ENTRIES 200
#define LEADERBOARD_DISPLAY_COUNT 10
const char *playersLeaderboardFilePath = "D:/Maze-explorer/players_leaderboard.txt";

PlayerScore playerScores[MAX_LEADERBOARD_ENTRIES];
int playerScoreCount = 0;

float avgLevelTime = 0.0f;
bool scoreSaved = false;
float ComputeAverageTime(void)
{
    float sum = 0.0f;
    for (int i = 0; i < LEVEL_COUNT; i++)
        sum += levelTimes[i];
    return sum / LEVEL_COUNT;
}

void SavePlayerScore(const char *name, float avgTime)
{
    FILE *f = fopen(playersLeaderboardFilePath, "a");
    if (f == NULL)
        return;
    fprintf(f, "%s %.2f\n", name, avgTime);
    fclose(f);
}

void LoadPlayerLeaderboard(void)
{
    playerScoreCount = 0;
    FILE *f = fopen(playersLeaderboardFilePath, "r");
    if (f == NULL)
        return;

    char nameBuf[MAX_NAME_LEN + 1];
    float t;
    while (playerScoreCount < MAX_LEADERBOARD_ENTRIES && fscanf(f, "%16s %f", nameBuf, &t) == 2)
    {
        strncpy(playerScores[playerScoreCount].name, nameBuf, MAX_NAME_LEN);
        playerScores[playerScoreCount].name[MAX_NAME_LEN] = '\0';
        playerScores[playerScoreCount].avgTime = t;
        playerScoreCount++;
    }
    fclose(f);

    for (int i = 0; i < playerScoreCount - 1; i++)
    {
        for (int j = 0; j < playerScoreCount - 1 - i; j++)
        {
            if (playerScores[j].avgTime > playerScores[j + 1].avgTime)
            {
                PlayerScore tmp = playerScores[j];
                playerScores[j] = playerScores[j + 1];
                playerScores[j + 1] = tmp;
            }
        }
    }
}

void StartLevelTimer(void)
{
    levelElapsedTime = 0.0;
    timerRunning = true;
}

void StopLevelTimer(int levelIndex)
{
    timerRunning = false;
    levelTimes[levelIndex] = (float)levelElapsedTime;

    if (bestLevelTimes[levelIndex] <= 0.0f || levelElapsedTime < bestLevelTimes[levelIndex])
    {
        bestLevelTimes[levelIndex] = (float)levelElapsedTime;
        SaveLeaderboard();
    }
}

const char *FormatTime(float seconds)
{
    static char buf[32];
    if (seconds < 0)
        seconds = 0;
    int minutes = (int)seconds / 60;
    float secs = seconds - minutes * 60;
    snprintf(buf, sizeof(buf), "%02d:%05.2f", minutes, secs);
    return buf;
}

void DrawHUD(int levelNumber, int keysRemaining)
{
    DrawRectangle(0, 0, screen_width, CELL, Fade((Color){10, 15, 40, 255}, 0.6f));
    DrawLine(0, CELL, screen_width, CELL, Fade(BLUE, 0.5f));

    char levelText[48];
    snprintf(levelText, sizeof(levelText), "LEVEL %d    KEYS LEFT: %d", levelNumber, keysRemaining);

    char timeText[40];
    snprintf(timeText, sizeof(timeText), "TIME  %s", FormatTime((float)levelElapsedTime));

    char bestText[48];
    float best = bestLevelTimes[levelNumber - 1];
    if (best > 0.0f)
        snprintf(bestText, sizeof(bestText), "BEST  %s", FormatTime(best));

    else
        snprintf(bestText, sizeof(bestText), "BEST  --:--.--");

    Vector2 timeSize = MeasureTextEx(font_play, timeText, 26, spacing);

    DrawTextEx(font_play, levelText, (Vector2){20, 14}, 26, spacing, (Color){170, 210, 255, 255});
    DrawTextEx(font_play, timeText, (Vector2){screen_width / 2 - timeSize.x / 2, 14}, 26, spacing, (Color){0, 255, 180, 255});
    DrawTextEx(font_play, bestText, (Vector2){screen_width - 260, 14}, 22, spacing, (Color){255, 200, 0, 255});
}

float Master_volume_value = 0.5;
void HandlePauseMenu(void)
{
    DrawRectangle(0, 0, screen_width, screen_height, Fade(BLACK, 0.65f));

    DrawRectangleRounded(pause_box, 0.1f, 8, Fade((Color){10, 15, 40, 255}, 0.95f));
    DrawRectangleRoundedLinesEx(pause_box, 0.1f, 8, 2, GOLD);

    DrawTextEx(font_play, pause_message, pause_message_pos, (float)font_play.baseSize + 20, 2, GOLD);

    Color resumeBtnColor = GetButtonColor(resume_button_posRec, mousepos, Button_color);
    Color exitBtnColor = GetButtonColor(exit_game_button_posRec, mousepos, Button_color);

    DrawRectangleRounded(resume_button_posRec, 1.0f, 8, resumeBtnColor);
    DrawRectangleRoundedLinesEx(resume_button_posRec, 1.0f, 8, 2, BLACK);
    DrawTextEx(font_play, resume_button_message, resume_button_pos, (float)font_play.baseSize + 10, 2, GREEN);

    DrawRectangleRounded(exit_game_button_posRec, 1.0f, 8, exitBtnColor);
    DrawRectangleRoundedLinesEx(exit_game_button_posRec, 1.0f, 8, 2, BLACK);
    DrawTextEx(font_play, exit_game_button_message, exit_game_button_pos, (float)font_play.baseSize + 10, 2, RED);

    Color musicBtnColor = GetButtonColor(music_button_posRec, mousepos, Button_color);
    char *musicText = musicOn ? music_on_message : music_off_message;
    Vector2 musicSize = MeasureTextEx(font_play, musicText, (float)font_play.baseSize + 10, 2);
    music_button_pos = (Vector2){
        music_button_posRec.x + music_button_posRec.width / 2 - musicSize.x / 2,
        music_button_posRec.y + music_button_posRec.height / 2 - musicSize.y / 2};

    DrawRectangleRounded(music_button_posRec, 1.0f, 8, musicBtnColor);
    DrawRectangleRoundedLinesEx(music_button_posRec, 1.0f, 8, 2, BLACK);
    DrawTextEx(font_play, musicText, music_button_pos, (float)font_play.baseSize + 10, 2, musicOn ? GREEN : RED);

    if (CheckCollisionPointRec(mousepos, music_button_posRec) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        PlaySound(clicksound);
        musicOn = !musicOn;

        if (musicOn)
        {
            Master_volume_value = 0.5f;
        }
        else
        {
            Master_volume_value = 0.0f;
        }

        SetMasterVolume(Master_volume_value);
        SetMusicVolume(backgrnd_music, musicOn ? 1.0f : 0.0f);
    }

    if (CheckCollisionPointRec(mousepos, resume_button_posRec) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        PlaySound(clicksound);
        gamePaused = false;
        timerRunning = true;
        ResumeMusicStream(backgrnd_music);
    }

    if (CheckCollisionPointRec(mousepos, exit_game_button_posRec) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        PlaySound(clicksound);
        gamePaused = false;
        timerRunning = false;
        ResumeMusicStream(backgrnd_music);
        level = ZERO_WINDOW;
    }
}

void AddEmitter(FlameEmitter *arr, int *count, Vector2 pos)
{
    if (*count >= MAX_LEVEL_EMITTERS)
        return;
    FlameEmitter *e = &arr[*count];
    e->basePos = pos;
    e->spawnTimer = (float)GetRandomValue(0, 30) / 100.0f;
    for (int i = 0; i < PARTICLES_PER_EMITTER; i++)
        e->particles[i].active = false;
    (*count)++;
}

void BuildFlameEmitters(Wall *walls, int wallCount, FlameEmitter *arr, int *count)
{
    *count = 0;
    for (int i = 0; i < wallCount; i++)
    {
        Wall w = walls[i];
        float x1 = w.x1 * CELL, y1 = w.y1 * CELL;
        float x2 = w.x2 * CELL, y2 = w.y2 * CELL;
        float len = (x1 == x2) ? fabsf(y2 - y1) : fabsf(x2 - x1);

        int segs = (int)(len / FLAME_SPACING + 0.5f);

        if (segs < 1)
            segs = 1;

        for (int s = 0; s < segs; s++)
        {
            float t = ((float)s + 0.5f) / (float)segs; // midpoint of sub-segment
            AddEmitter(arr, count, (Vector2){x1 + (x2 - x1) * t, y1 + (y2 - y1) * t});
        }
    }
}

Color LerpColor(Color a, Color b, float t)
{
    if (t < 0)
        t = 0;
    if (t > 1)
        t = 1;
    return (Color){
        (unsigned char)(a.r + (b.r - a.r) * t),
        (unsigned char)(a.g + (b.g - a.g) * t),
        (unsigned char)(a.b + (b.b - a.b) * t),
        (unsigned char)(a.a + (b.a - a.a) * t)};
}

void UpdateAndDrawFlames(FlameEmitter *emitters, int emitterCount, float dt)
{
    Color baseColor = (Color){255, 255, 255, 255}; // hot near the wall
    Color midColor = (Color){255, 255, 255, 255};  // orange body
    Color tipColor = (Color){255, 255, 255, 0};    // fades to nothing

    for (int i = 0; i < emitterCount; i++)
    {
        FlameEmitter *e = &emitters[i];

        e->spawnTimer -= dt;
        if (e->spawnTimer <= 0)
        {
            for (int k = 0; k < PARTICLES_PER_EMITTER; k++)
            {
                if (!e->particles[k].active)
                {
                    FlameParticle *p = &e->particles[k];
                    p->age = 0.0f;
                    p->maxLife = (float)GetRandomValue(20, 35) / 100.0f;
                    p->phase = (float)GetRandomValue(0, 628) / 100.0f;
                    p->speed = (float)GetRandomValue(100, 500) / 100.0f;
                    p->amplitude = (float)GetRandomValue(2, 5);
                    p->size = (float)GetRandomValue(4, 7);
                    p->active = true;
                    break;
                }
            }
            e->spawnTimer = (float)GetRandomValue(1, 3) / 100.0f; // 0.01 - 0.03s
        }

        for (int k = 0; k < PARTICLES_PER_EMITTER; k++)
        {
            FlameParticle *p = &e->particles[k];
            if (!p->active)
                continue;

            p->age += dt;
            if (p->age >= p->maxLife)
            {
                p->active = false;
                continue;
            }

            float ageRatio = p->age / p->maxLife;

            Vector2 pos;
            pos.y = e->basePos.y - FLAME_RISE_HEIGHT * ageRatio;
            pos.x = e->basePos.x + sinf(p->age * p->speed + p->phase) * p->amplitude * ageRatio;

            float size = p->size * (1.0f - ageRatio * 0.75f);

            Color c;
            if (ageRatio < 0.5f)
                c = LerpColor(baseColor, midColor, ageRatio / 0.5f);
            else
                c = LerpColor(midColor, tipColor, (ageRatio - 0.5f) / 0.5f);

            DrawCircleV(pos, size, c);
        }
    }
}

bool NearBlackHole(Vector2 rocketPos, Vector2 holePos, float radiusCells)
{
    return Vector2Distance(rocketPos, holePos) < radiusCells;
}

bool CheckBlackHoleTeleport(Vector2 rocketPos, Vector2 *holes, Vector2 *outPos)
{
    if (teleportCooldown > 0.0f)
        return false;
    false;

    for (int i = 0; i < 2; i++)
    {
        if (NearBlackHole(rocketPos, holes[i], BLACKHOLE_RADIUS_CELLS))
        {
            *outPos = holes[1 - i];
            return true;
        }
    }
    return false;
}

void DrawBlackHole(Vector2 gridPos, float t)
{
    Vector2 center = {gridPos.x * CELL, gridPos.y * CELL};

    for (int i = 1; i <= 12; i++)
    {
        float angle = t * 3.2f + i * (2.0f * PI / 12.0f);
        float radiusT = fmodf(t * 0.7f + i * 0.083f, 1.0f);
        float r = Lerp(30.0f, 3.0f, radiusT);
        float px = center.x + cosf(angle) * r;
        float py = center.y + sinf(angle) * r;
        float size = Lerp(5.0f, 1.0f, radiusT);
        Color c = LerpColor((Color){170, 90, 255, 255}, (Color){15, 0, 30, 0}, radiusT);
        DrawCircleV((Vector2){px, py}, size, c);
    }

    DrawCircleV(center, 11, (Color){5, 0, 15, 255}); // event horizon core
    DrawCircleLines((int)center.x, (int)center.y, 15, Fade((Color){170, 90, 255, 255}, 0.6f));
}

void DrawWallThick(Wall w, Color color, int thickness)
{
    int x1 = w.x1 * CELL, y1 = w.y1 * CELL;
    int x2 = w.x2 * CELL, y2 = w.y2 * CELL;
    int half = thickness / 2;

    if (x1 == x2)
    {
        int top = (y1 < y2) ? y1 : y2;
        int height = abs(y2 - y1);
        DrawRectangle(x1 - half, top - half, thickness, height + thickness, color);
    }
    else
    {
        int left = (x1 < x2) ? x1 : x2;
        int width = abs(x2 - x1);
        DrawRectangle(left - half, y1 - half, width + thickness, thickness, color);
    }
}

void DrawWallColored(Wall w, Color color)
{
    DrawWallThick(w, color, THICK);
}

void DrawWall(Wall w)
{
    DrawWallColored(w, (Color){30, 50, 100, 255});
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

bool hitMeteor(Vector2 rocket_pos, Vector2 *meteors, int meteorCount)
{
    Rectangle rocketRec = {rocket_pos.x * CELL, rocket_pos.y * CELL, rocketSize, rocketSize};
    float offset = (CELL - meteorSize) / 2.0f;

    for (int i = 0; i < meteorCount; i++)
    {
        Rectangle meteorRec = {
            meteors[i].x * CELL + offset,
            meteors[i].y * CELL + offset,
            meteorSize, meteorSize};

        if (CheckCollisionRecs(rocketRec, meteorRec))
            return true;
    }
    return false;
}

void DrawPlanetCentered(Texture2D tex, Vector2 gridPos)
{
    float offset = (CELL - moonSize) / 2.0f;
    DrawTexturePro(tex,
                   (Rectangle){0, 0, tex.width, tex.height},
                   (Rectangle){gridPos.x * CELL + offset, gridPos.y * CELL + offset, moonSize, moonSize},
                   (Vector2){0, 0}, 0.0f, WHITE);
}
void DrawMeteorCentered(Texture2D tex, Vector2 gridPos)
{
    float offset = (CELL - meteorSize) / 2.0f;
    DrawTexturePro(tex,
                   (Rectangle){0, 0, tex.width, tex.height},
                   (Rectangle){gridPos.x * CELL + offset, gridPos.y * CELL + offset, meteorSize, meteorSize},
                   (Vector2){0, 0}, 0.0f, WHITE);
}

void DrawSpaceShipCentered(Texture2D tex, Vector2 gridPos)
{
    float offset = (CELL - meteorSize) / 2.0f;
    DrawTexturePro(tex,
                   (Rectangle){0, 0, tex.width, tex.height},
                   (Rectangle){gridPos.x * CELL + offset, gridPos.y * CELL + offset, meteorSize, meteorSize},
                   (Vector2){0, 0}, 0.0f, WHITE);
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

bool CheckAlienShipCollision(Vector2 rocket_pos, Vector2 ship_pos)
{
    float collision_distance = 0.5f;
    return Vector2Distance(rocket_pos, ship_pos) < collision_distance;
}

void updateSpaceShip(spaceShip *ship)
{
    if (ship == NULL || ship->path == NULL || ship->pathSize <= 1)
        return;

    float dt = GetFrameTime();
    int nextIndex = ship->currentIndex + ship->direction;

    if (nextIndex >= ship->pathSize || nextIndex < 0)
    {
        ship->direction *= -1;
        nextIndex = ship->currentIndex + ship->direction;
    }

    Vector2 target = ship->path[nextIndex];
    Vector2 delta = {target.x - ship->pos.x, target.y - ship->pos.y};
    float distance = sqrtf(delta.x * delta.x + delta.y * delta.y);

    if (distance <= 0.01f)
    {
        ship->pos = target;
        ship->currentIndex = nextIndex;
        return;
    }

    float step = ship->speed * dt;
    if (step >= distance)
    {
        ship->pos = target;
        ship->currentIndex = nextIndex;
    }
    else
    {
        ship->pos.x += (delta.x / distance) * step;
        ship->pos.y += (delta.y / distance) * step;
    }
}

bool CellFree(Vector2 gridPos, Wall *walls, int wallCount)
{
    return !hitWall(gridPos, walls, wallCount);
}

Vector2 FindOpenCellNear(Vector2 start, Wall *walls, int wallCount, Vector2 *avoid, int avoidCount, float minDist, int gridMaxX, int gridMaxY)
{
    for (int radius = 0; radius <= 30; radius++)
    {
        for (int dy = -radius; dy <= radius; dy++)
        {
            for (int dx = -radius; dx <= radius; dx++)
            {
                if (radius > 0 && abs(dx) != radius && abs(dy) != radius)
                    continue; // only walk the outer ring of this radius

                int gx = (int)start.x + dx;
                int gy = (int)start.y + dy;
                if (gx < 1 || gy < 1 || gx > gridMaxX || gy > gridMaxY)
                    continue;

                Vector2 candidate = {(float)gx, (float)gy};
                if (!CellFree(candidate, walls, wallCount))
                    continue;

                bool tooClose = false;
                for (int i = 0; i < avoidCount; i++)
                {
                    if (Vector2Distance(candidate, avoid[i]) < minDist)
                    {
                        tooClose = true;
                        break;
                    }
                }
                if (tooClose)
                    continue;

                return candidate;
            }
        }
    }
    return start;
}

void PlaceKeys(Key keys[KEY_TYPE_COUNT], Wall *walls, int wallCount, Vector2 rocketStart, Vector2 planetPos, Vector2 *blackholes, int gridMaxX, int gridMaxY)
{
    Vector2 avoid[8];
    int avoidCount = 0;
    avoid[avoidCount++] = rocketStart;
    avoid[avoidCount++] = planetPos;
    avoid[avoidCount++] = blackholes[0];
    avoid[avoidCount++] = blackholes[1];

    Vector2 seeds[KEY_TYPE_COUNT] = {
        {gridMaxX * 0.25f, gridMaxY * 0.3f},
        {gridMaxX * 0.75f, gridMaxY * 0.3f},
        {gridMaxX * 0.25f, gridMaxY * 0.75f},
        {gridMaxX * 0.75f, gridMaxY * 0.75f},
    };

    for (int i = 0; i < KEY_TYPE_COUNT; i++)
    {
        Vector2 pos = FindOpenCellNear(seeds[i], walls, wallCount, avoid, avoidCount, 1.6f, gridMaxX, gridMaxY);
        keys[i].pos = pos;
        keys[i].type = (KeyType)i;
        keys[i].collected = false;
        avoid[avoidCount++] = pos;
    }
}

void BuildPlanetRedWalls(Vector2 planetPos, Wall redWalls[4])
{
    int px = (int)planetPos.x;
    int py = (int)planetPos.y;
    redWalls[0] = (Wall){px, py, px + 1, py};
    redWalls[1] = (Wall){px, py + 1, px + 1, py + 1};
    redWalls[2] = (Wall){px, py, px, py + 1};
    redWalls[3] = (Wall){px + 1, py, px + 1, py + 1};
}

int CountKeysCollected(Key keys[KEY_TYPE_COUNT])
{
    int c = 0;
    for (int i = 0; i < KEY_TYPE_COUNT; i++)
        if (keys[i].collected)
            c++;
    return c;
}

bool AllKeysCollected(Key keys[KEY_TYPE_COUNT])
{
    return CountKeysCollected(keys) == KEY_TYPE_COUNT;
}

void UpdateKeyPickups(Key keys[KEY_TYPE_COUNT], Vector2 rocketPos)
{
    for (int i = 0; i < KEY_TYPE_COUNT; i++)
    {
        if (!keys[i].collected && Vector2Distance(rocketPos, keys[i].pos) < 0.55f)
        {
            keys[i].collected = true;
            PlaySound(clicksound);
        }
    }
}

Color GetKeyColor(KeyType t)
{
    switch (t)
    {
    case KEY_RED:
        return RED;
    case KEY_GREEN:
        return GREEN;
    case KEY_YELLOW:
        return YELLOW;
    case KEY_BLUE:
        return BLUE;
    default:
        return WHITE;
    }
}

void DrawKeys(Key keys[KEY_TYPE_COUNT], float t)
{
    for (int i = 0; i < KEY_TYPE_COUNT; i++)
    {
        if (keys[i].collected)
            continue;

        float cx = keys[i].pos.x * CELL + CELL / 2.0f;
        float cy = keys[i].pos.y * CELL + CELL / 2.0f;
        float bob = sinf(t * 3.0f + i * 1.7f) * 4.0f;
        cy += bob;
        Color c = GetKeyColor(keys[i].type);

        float bowOuterR = 8.0f;
        float bowInnerR = 4.5f;
        float shaftLen = 15.0f;
        float shaftThick = 3.0f;

        float bowCx = cx - shaftLen / 2.0f - bowOuterR + shaftThick;
        float bowCy = cy;

        DrawRing((Vector2){bowCx, bowCy}, bowInnerR, bowOuterR, 0, 360, 24, c);
        DrawCircleLines((int)bowCx, (int)bowCy, bowOuterR, BLACK);
        DrawCircleLines((int)bowCx, (int)bowCy, bowInnerR, BLACK);

        float shaftStartX = bowCx + bowOuterR - shaftThick;
        Rectangle shaftRec = {shaftStartX, cy - shaftThick / 2.0f, shaftLen, shaftThick};
        DrawRectangleRec(shaftRec, c);
        DrawRectangleLinesEx(shaftRec, 1, BLACK);

        float teethX = shaftStartX + shaftLen;
        Rectangle tooth1 = {teethX - 5, cy + shaftThick / 2.0f, 4, 5};
        Rectangle tooth2 = {teethX - 1, cy + shaftThick / 2.0f, 3, 8};
        DrawRectangleRec(tooth1, c);
        DrawRectangleRec(tooth2, c);
        DrawRectangleLinesEx(tooth1, 1, BLACK);
        DrawRectangleLinesEx(tooth2, 1, BLACK);
    }
}

void updateRocket(Player *rocket, Wall wall_level[], int wall_count, Wall redWalls[4], bool redWallsActive, Vector2 *meteors, int meteorCount)
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
                bool blockedUp = hitWall(next_pos, wall_level, wall_count) || (redWallsActive && hitWall(next_pos, redWalls, 4)) || hitMeteor(next_pos, meteors, meteorCount);
                if (!blockedUp)
                {
                    rocket->pos = next_pos;
                    if (rocket->pos.y < 0)
                    {
                        rocket->pos.y = 0;
                    }
                }
                else
                {
                    PlaySound(crashsound);
                }
                break;

            case DOWN:
                next_pos = rocket->pos;
                next_pos.y = rocket->pos.y + rocket->speed * dt;
                bool blockedDown = hitWall(next_pos, wall_level, wall_count) || (redWallsActive && hitWall(next_pos, redWalls, 4)) || hitMeteor(next_pos, meteors, meteorCount);
                if (!blockedDown)
                {
                    rocket->pos = next_pos;
                    if (rocket->pos.y > screen_height)
                    {
                        rocket->pos.y = screen_height;
                    }
                }
                else
                {
                    PlaySound(crashsound);
                }
                break;

            case RIGHT:
                next_pos = rocket->pos;
                next_pos.x = rocket->pos.x + rocket->speed * dt;
                bool blockedRight = hitWall(next_pos, wall_level, wall_count) || (redWallsActive && hitWall(next_pos, redWalls, 4)) || hitMeteor(next_pos, meteors, meteorCount);
                if (!blockedRight)
                {
                    rocket->pos = next_pos;
                    if (rocket->pos.x > screen_width)
                    {
                        rocket->pos.x = screen_width;
                    }
                }
                else
                {
                    PlaySound(crashsound);
                }
                break;

            case LEFT:
                next_pos = rocket->pos;
                next_pos.x = rocket->pos.x - rocket->speed * dt;
                bool blockedLeft = hitWall(next_pos, wall_level, wall_count) || (redWallsActive && hitWall(next_pos, redWalls, 4)) || hitMeteor(next_pos, meteors, meteorCount);
                if (!blockedLeft)
                {
                    rocket->pos = next_pos;
                    if (rocket->pos.x < 0)
                    {
                        rocket->pos.x = 0;
                    }
                }
                else
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

#include "Load-Data.h"
#include "Start_gameplay.h"

int main()
{
    InitWindow(screen_width, screen_height, "COSMIC MAZE");
    InitAudioDevice();
    SetMasterVolume(Master_volume_value);
    SetTargetFPS(60);
    load_data_0();
    load_data_pause_window();

    int levelChoice;
    printf("Enter window number (0-12): ");
    scanf("%d", &levelChoice);
    level = levelChoice;

    switch (level)
    {
    case LEVEL_1:
        rocket.pos = rocket_position[0];
        rocket.dir = DOWN;
        break;
    case LEVEL_2:
        rocket.pos = rocket_position[1];
        rocket.dir = DOWN;
        break;
    case LEVEL_3:
        rocket.pos = rocket_position[2];
        rocket.dir = RIGHT;
        break;
    case LEVEL4:
        rocket.pos = rocket_position[3];
        rocket.dir = LEFT;
        break;
    default:
        break;
    }

    LoadLeaderboard();

    BuildFlameEmitters(wall_level1, wallCount1, flameEmitters1, &flameEmitterCount1);
    BuildFlameEmitters(wall_level2, wallCount2, flameEmitters2, &flameEmitterCount2);
    BuildFlameEmitters(wall_level3, wallCount3, flameEmitters3, &flameEmitterCount3);
    BuildFlameEmitters(wall_level4, wallCount4, flameEmitters4, &flameEmitterCount4);

    {
        const int GRID_MAX_X = (screen_width / CELL) - 2;
        const int GRID_MAX_Y = (screen_height / CELL) - 2;

        BuildPlanetRedWalls(planet_position[0], redWalls_level1);
        BuildPlanetRedWalls(planet_position[1], redWalls_level2);
        BuildPlanetRedWalls(planet_position[2], redWalls_level3);
        BuildPlanetRedWalls(planet_position[3], redWalls_level4);

        PlaceKeys(keys_level1, wall_level1, wallCount1, rocket_position[0], planet_position[0], blackholes_level1, GRID_MAX_X, GRID_MAX_Y);
        PlaceKeys(keys_level2, wall_level2, wallCount2, rocket_position[1], planet_position[1], blackholes_level2, GRID_MAX_X, GRID_MAX_Y);
        PlaceKeys(keys_level3, wall_level3, wallCount3, rocket_position[2], planet_position[2], blackholes_level3, GRID_MAX_X, GRID_MAX_Y);
        PlaceKeys(keys_level4, wall_level4, wallCount4, rocket_position[3], planet_position[3], blackholes_level4, GRID_MAX_X, GRID_MAX_Y);
    }

    for (int i = 0; i < CNT; i++)
    {
        rocketTex[i] = LoadTexture(rocketPics[i]);
    }

    load_data_0();
    load_data_leaderboard_window();
    load_data_howto_window();
    load_data_1();
    load_credentials();
    load_data_name_window();
    load_data_rules_window();
    load_data_transition_window();
    load_data_transition_window2();
    load_data_transition_window3();
    load_data_total_window();

    backgrnd_music = LoadMusicStream("D:/Maze-explorer/Audio/background.ogg");
    clicksound = LoadSound("D:/Maze-explorer/Audio/click.wav");
    crashsound = LoadSound("D:/Maze-explorer/Audio/crash.wav");
    level_up_sound = LoadSound("D:/Maze-explorer/Audio/level_up.mp3");

    PlayMusicStream(backgrnd_music);

    while (!WindowShouldClose() && !exitGameRequested)
    {

        UpdateMusicStream(backgrnd_music);
        if (GetMusicTimePlayed(backgrnd_music) >= GetMusicTimeLength(backgrnd_music) - 1.0f)
            SeekMusicStream(backgrnd_music, 0.0f);

        BeginDrawing();
        ClearBackground(DARKBLUE);

        start_gameplay();

        EndDrawing();
    }

    UnloadMusicStream(backgrnd_music);
    UnloadSound(clicksound);
    CloseAudioDevice();

    UnloadTexture(planet);
    UnloadTexture(meteor);
    UnloadTexture(space_background);
    UnloadTexture(space_background2);
    UnloadTexture(space_background3);
    UnloadTexture(space_background4);
    UnloadTexture(space_background5);
    UnloadTexture(alien_spaceship);
    for (int i = 0; i < CNT; i++)
    {
        UnloadTexture(rocketTex[i]);
    }

    CloseWindow();
}