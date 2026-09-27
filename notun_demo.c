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
#define rocketSpeed 3
#define rocketSize 30
#define moonSize 42
#define meteorSize 34
#define BLACKHOLE_RADIUS_CELLS 0.45f // how close (in grid cells) counts as "entered"
#define BLACKHOLE_COOLDOWN 0.6f
#define PARTICLES_PER_EMITTER 1

#define FLAME_SPACING 90.0f     // px between emitters along a wall
#define FLAME_RISE_HEIGHT 22.0f // how tall each flame lick grows
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
    Vector2 pos; // grid position (same units as rocket_position / planet_position)
    KeyType type;
    bool collected;
} Key;

typedef struct
{
    float age;
    float maxLife;
    float phase;     // sine offset so flames don't all wobble in sync
    float speed;     // wobble speed
    float amplitude; // how far it licks side to side
    float size;
    bool active;
} FlameParticle;

typedef struct
{
    Vector2 basePos;                                // fixed point on the wall - never moves
    float spawnTimer;                               // counts down to next particle spawn
    FlameParticle particles[PARTICLES_PER_EMITTER]; // this emitter's OWN slots, shared with no one
} FlameEmitter;

// rocket pics directories
const char *rocketPics[CNT] = {
    "D:/Maze-explorer/rocket/1.png",
    "D:/Maze-explorer/rocket/2.png",
    "D:/Maze-explorer/rocket/3.png",
    "D:/Maze-explorer/rocket/4.png"};

// textures
Texture2D rocketTex[CNT];
Texture2D space_background;
Texture2D space_background2;
Texture2D space_background3;
Texture2D space_background4;
Texture2D space_background5;
Texture2D planet;
Texture2D meteor;
Texture2D alien_spaceship;

// vector arrays
const Vector2 rocket_position[] = {{2, 9}, {2, 2}, {3, 4}, {2, 11}};
Vector2 planet_position[] = {{23, 8}, {22, 2}, {15, 13}, {22, 2}};

Vector2 meteors_level2[] = {{6, 12}, {24, 6}, {24, 10}, {22, 14}, {17, 9}, {21, 4}, {4, 14}};
int meteors_level2_size = sizeof(meteors_level2) / sizeof(Vector2);
Vector2 meteors_level3[] = {
    {18, 4}, {8, 8}, {1, 13}, {24, 9}, {16, 8}, {11, 12}, {7, 10}, {16, 6}, {17, 13}, {15, 11}, {20, 2}, {13, 2}, {21, 12}, {5, 12}, {19, 10}, {20, 24}, {23, 33}, {12, 10}, {11, 6}};
int meteors_level3_size = sizeof(meteors_level3) / sizeof(Vector2);
Vector2 meteors_level4[] = {{12, 8}, {5, 5}};

// ===================== BLACK HOLES =====================
// One linked pair per level. Both points below were checked against every
// wall rectangle in that level's own wall array (same collision test as
// hitWall) to confirm the rocket's 30x30 box sits cleanly in open space
// there, and each pair is placed far apart from each other and from that
// level's rocket start / planet so they read as two distinct portals.
Vector2 blackholes_level1[2] = {{6, 10}, {22, 5}};
Vector2 blackholes_level2[2] = {{5, 12}, {19, 5}};
Vector2 blackholes_level3[2] = {{2, 4}, {21, 10}};
Vector2 blackholes_level4[2] = {{5, 11}, {22, 5}};

// seconds of immunity right after a teleport

float teleportCooldown = 0.0f; // counts down after every teleport so the rocket
                               // doesn't instantly re-trigger the exit hole
float blackholeAnimTime = 0.0f;
// =================== END BLACK HOLES ===================

// ===================== COLLECTIBLE KEYS + RED GUARD WALLS =====================
// Each level has 4 keys (red, green, yellow, blue) scattered through the maze.
// The planet in every level sits inside a small 1-cell box made of 4 RED walls
// (built automatically from that level's planet position - see
// BuildPlanetRedWalls) so the rocket physically cannot reach the planet until
// every key has been picked up. Once the 4th key is collected, the red walls
// for that level stop being solid and stop being drawn.

Key keys_level1[KEY_TYPE_COUNT];
Key keys_level2[KEY_TYPE_COUNT];
Key keys_level3[KEY_TYPE_COUNT];
Key keys_level4[KEY_TYPE_COUNT];

Wall redWalls_level1[4];
Wall redWalls_level2[4];
Wall redWalls_level3[4];
Wall redWalls_level4[4];
// =================== END COLLECTIBLE KEYS + RED GUARD WALLS ===================

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

// game messages
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
char *rules_line2 = "Your space exploration starts here!";
char *rules_line3 = "You must travel across four planets.";
char *rules_line4 = "To complete each level, collect four keys";
char *rules_line5 = "and navigate the mysterious maze.";
char *rules_line6 = "Avoid crashing into meteors and walls.";
char *rules_line7 = "If you collide with an alien spaceship, you will";
char *rules_line8 = "be sent back to the starting point.";
char *rules_line9 = "Be careful, and good luck on your journey!";

char *start_button_message = "START";

char *supervisor_label = "SUPERVISOR-";
char *supervisor_name = "ABU BASHIR SHUAIB SIR";

Vector2 supervisor_label_pos;
Vector2 supervisor_name_pos;

Rectangle rules_box;
Vector2 rules_line_pos[9];
Vector2 start_button_pos;
Rectangle start_button_posRec;

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

Vector2 credential_name1_pos;
Vector2 credential_name2_pos;
Vector2 credential_back_pos;
Rectangle credential_back_posRec;
Vector2 credentialButton_pos;
Rectangle credentialButton_posRec;

// LEADERBOARD button (main menu) + LEADERBOARD screen layout
char *leaderboard_button_message = "LEADERBOARD";
Vector2 leaderboard_button_pos;
Rectangle leaderboard_button_posRec;

char *leaderboard_title = "LEADERBOARD";
Vector2 leaderboard_title_pos;

char *leaderboard_back_message = "BACK";
Vector2 leaderboard_back_pos;
Rectangle leaderboard_back_posRec;

Vector2 name_label_pos;
Vector2 name_input_text_pos;
Rectangle name_label_box;
Rectangle name_input_box;

// audio
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
Vector2 avg_time_pos; // position of the "Average Time" row on the score screen

char *exit_message = "EXIT";
char *play_again_message = "PLAY AGAIN";

Vector2 exit_button_pos;
Rectangle exit_button_posRec;

Vector2 playagain_button_pos;
Rectangle playagain_button_posRec;

bool exitGameRequested = false;

static Color GetButtonColor(Rectangle rect, Vector2 mouse, Color baseColor)
{
    return CheckCollisionPointRec(mouse, rect) ? WHITE : baseColor;
}

// ===================== HUD: LEVEL LABEL + STOPWATCH + LEADERBOARD =====================
// The very top row of every maze (y = 0 .. CELL) is outside the actual playable
// border of the maze (the real border wall sits at y = 1 cell down), so it's free
// screen real-estate. We use that strip to show which level is active and a live
// stopwatch, and we persist each level's best completion time to disk as a tiny
// leaderboard.

const char *leaderboardFilePath = "D:/Maze-explorer/leaderboard.txt";

double levelElapsedTime = 0.0; // seconds elapsed on the current level's stopwatch
bool timerRunning = false;
int currentLevelNumber = 1;              // 1-based, shown in the HUD
float levelTimes[LEVEL_COUNT] = {0};     // most recent completion time per level
float bestLevelTimes[LEVEL_COUNT] = {0}; // leaderboard: best (lowest) time per level, 0 = no record yet

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

// ===================== PLAYER LEADERBOARD (AVG TIME ACROSS ALL LEVELS) =====================
// Every time a player finishes all 4 levels, their name and the average of
// their 4 level times (their "score" - lower is better) is appended to a
// separate file. The main menu's LEADERBOARD button reads that whole file
// back in, sorts it fastest-first, and shows it as a simple scoreboard.

typedef struct
{
    char name[MAX_NAME_LEN + 1];
    float avgTime;
} PlayerScore;

#define MAX_LEADERBOARD_ENTRIES 200
#define LEADERBOARD_DISPLAY_COUNT 10 // how many rows to show on screen at once

const char *playersLeaderboardFilePath = "D:/Maze-explorer/players_leaderboard.txt";

PlayerScore playerScores[MAX_LEADERBOARD_ENTRIES];
int playerScoreCount = 0;

float avgLevelTime = 0.0f; // this run's average completion time across all 4 levels
bool scoreSaved = false;   // guards against writing the same completed run twice

// Averages this run's 4 (already-completed) level times into a single score.
float ComputeAverageTime(void)
{
    float sum = 0.0f;
    for (int i = 0; i < LEVEL_COUNT; i++)
        sum += levelTimes[i];
    return sum / LEVEL_COUNT;
}

// Appends one "<name> <avgTime>" line to the players leaderboard file. This
// is what makes a completed run show up on the LEADERBOARD screen.
void SavePlayerScore(const char *name, float avgTime)
{
    FILE *f = fopen(playersLeaderboardFilePath, "a");
    if (f == NULL)
        return;
    fprintf(f, "%s %.2f\n", name, avgTime);
    fclose(f);
}

// Reads every "<name> <avgTime>" line from disk into playerScores[], then
// sorts them ascending by avgTime, so the fastest average time (best score)
// ends up first.
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

    // simple ascending bubble sort - the file only ever holds a handful of
    // entries so this is more than fast enough.
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
// =================== END PLAYER LEADERBOARD (AVG TIME ACROSS ALL LEVELS) ===================

// Starts (or restarts) the stopwatch for whichever level is about to begin.
void StartLevelTimer(void)
{
    levelElapsedTime = 0.0;
    timerRunning = true;
}

// Stops the stopwatch, records the run, and updates the on-disk leaderboard
// if this run beat the previous best for that level.
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

// Formats seconds as mm:ss.xx
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

// Draws the level indicator + live stopwatch + best time inside the free strip
// above the maze border (0 <= y < CELL).
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
// =================== END HUD: LEVEL LABEL + STOPWATCH + LEADERBOARD ===================

// ===================== WALL-ANCHORED FLAME SYSTEM =====================
// Design: fixed emitter points are laid out along every wall segment once,
// at level-build time. Each emitter OWNS a tiny fixed number of its own
// particle slots (PARTICLES_PER_EMITTER) instead of pulling from one shared
// pool. This matters a lot: with a shared pool, the emitters near the START
// of the wall array (top of the maze) grab a free slot first every single
// frame, so once the pool fills up the emitters further down the array
// (bottom of the maze) can never find a free slot and never get to light -
// that was exactly why the lower part of the maze stayed dark. Giving every
// emitter its own private slots means every wall gets to flicker, no matter
// where it sits in the array or the maze.
//
// Each particle only rises straight up from its own emitter and wobbles
// sideways with a smooth sine wave (not random jitter), tapering and
// changing color as it climbs - which reads as a flame lick instead of
// scattered sparks.

void InitFireParticles(void)
{
    // no shared pool to clear anymore - every emitter's particles start
    // inactive automatically when the emitter is created (see AddEmitter).
    // kept as a no-op so main() doesn't need to change.
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

// Walks every wall segment and drops fixed emitter points along it, spaced
// roughly FLAME_SPACING pixels apart. IMPORTANT: each point is placed at the
// MIDPOINT of its own sub-segment (never at t=0 or t=1), so it never lands
// on a corner shared with a neighboring wall. That guarantees every single
// wall - even a short 1-cell one - gets its own dedicated flame, instead of
// only the shared corners lighting up.
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

        // this emitter (re)lights one of ITS OWN slots - it never has to
        // wait on any other wall's flame to free up a slot
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

            float ageRatio = p->age / p->maxLife; // 0 = at wall, 1 = flame tip

            // rises straight up from its own anchor, wobbling side to side
            Vector2 pos;
            pos.y = e->basePos.y - FLAME_RISE_HEIGHT * ageRatio;
            pos.x = e->basePos.x + sinf(p->age * p->speed + p->phase) * p->amplitude * ageRatio;

            float size = p->size * (1.0f - ageRatio * 0.75f); // tapers to a point

            Color c;
            if (ageRatio < 0.5f)
                c = LerpColor(baseColor, midColor, ageRatio / 0.5f);
            else
                c = LerpColor(midColor, tipColor, (ageRatio - 0.5f) / 0.5f);

            DrawCircleV(pos, size, c);
        }
    }
}
// =================== END WALL-ANCHORED FLAME SYSTEM ===================

// ===================== BLACK HOLE TELEPORT + VISUAL =====================
bool NearBlackHole(Vector2 rocketPos, Vector2 holePos, float radiusCells)
{
    return Vector2Distance(rocketPos, holePos) < radiusCells;
}

// Checks the rocket against both ends of a level's black hole pair. If it's
// inside one (and not still on cooldown from a previous jump), returns true
// and writes the OTHER hole's position into outPos so the caller can move
// the rocket there.
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

// Draws a small swirling portal: particles spiral inward and fade/shrink as
// they approach the core, animated purely from elapsed time (t) so every
// hole on screen can share one clock and still look alive.
void DrawBlackHole(Vector2 gridPos, float t)
{
    Vector2 center = {gridPos.x * CELL, gridPos.y * CELL};

    for (int i = 1; i <= 12; i++)
    {
        float angle = t * 3.2f + i * (2.0f * PI / 12.0f);
        float radiusT = fmodf(t * 0.7f + i * 0.083f, 1.0f); // 0 = outer rim, 1 = core
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
// =================== END BLACK HOLE TELEPORT + VISUAL ===================

// thinner than the maze walls (THICK = 15)

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

// Check if rocket collides with alien spaceship (using grid-based collision)
bool CheckAlienShipCollision(Vector2 rocket_pos, Vector2 ship_pos)
{
    float collision_distance = 0.8f; // collision radius in grid cells
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

// A grid cell is "free" if a rocket-sized box placed there does not collide
// with any wall in that level - reuses the exact same test hitWall() already
// uses for rocket movement, so a cell that passes this check is guaranteed
// to be a legal place to stand.
bool CellFree(Vector2 gridPos, Wall *walls, int wallCount)
{
    return !hitWall(gridPos, walls, wallCount);
}

// Spirals outward from `start` (in whole-cell rings) looking for the first
// free cell that is also at least `minDist` grid cells away from every point
// in `avoid`. Guarantees the returned cell never sits inside a wall, since it
// is only ever accepted after passing CellFree().
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
    return start; // fallback - should not happen on a real maze
}

// Places all 4 keys for one level. Seeds one search per quadrant of the maze
// so the keys end up spread out, then nudges each seed to the nearest free
// cell that isn't too close to the rocket start, the planet, the black
// holes, or any key already placed.
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

// Builds the 4 red guard walls that box in exactly the single grid cell the
// planet sits in (top / bottom / left / right edges of that cell). Computed
// automatically from the planet's own position, so it always seals the
// planet off correctly no matter which level it's called for.
void BuildPlanetRedWalls(Vector2 planetPos, Wall redWalls[4])
{
    int px = (int)planetPos.x;
    int py = (int)planetPos.y;
    redWalls[0] = (Wall){px, py, px + 1, py};         // top
    redWalls[1] = (Wall){px, py + 1, px + 1, py + 1}; // bottom
    redWalls[2] = (Wall){px, py, px, py + 1};         // left
    redWalls[3] = (Wall){px + 1, py, px + 1, py + 1}; // right
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

// Checks the rocket against every not-yet-collected key in this level and
// marks it collected if the rocket is close enough.
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

// Returns the color used to draw a given key type.
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

// Draws every key that hasn't been collected yet as a small realistic key
// silhouette: a ring-shaped bow (head), a shaft, and two teeth at the end -
// instead of a plain dot with a bump.
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

        // bow: a ring, so the middle reads as a hole like a real key head
        DrawRing((Vector2){bowCx, bowCy}, bowInnerR, bowOuterR, 0, 360, 24, c);
        DrawCircleLines((int)bowCx, (int)bowCy, bowOuterR, BLACK);
        DrawCircleLines((int)bowCx, (int)bowCy, bowInnerR, BLACK);

        // shaft, running from the bow to the teeth
        float shaftStartX = bowCx + bowOuterR - shaftThick;
        Rectangle shaftRec = {shaftStartX, cy - shaftThick / 2.0f, shaftLen, shaftThick};
        DrawRectangleRec(shaftRec, c);
        DrawRectangleLinesEx(shaftRec, 1, BLACK);

        // teeth: two notches of different length off the end of the shaft
        float teethX = shaftStartX + shaftLen;
        Rectangle tooth1 = {teethX - 5, cy + shaftThick / 2.0f, 4, 5};
        Rectangle tooth2 = {teethX - 1, cy + shaftThick / 2.0f, 3, 8};
        DrawRectangleRec(tooth1, c);
        DrawRectangleRec(tooth2, c);
        DrawRectangleLinesEx(tooth1, 1, BLACK);
        DrawRectangleLinesEx(tooth2, 1, BLACK);
    }
}
// =================== END KEY SYSTEM HELPERS ===================

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

void load_data_0()
{
    float paddingX = 30;
    float paddingY = 15;

    font_play = LoadFont("D:/Raylib project/Fonts/ALIEN CYBERNETICS.ttf");
    play_button_pos = (Vector2){screen_width / 2 - MeasureTextEx(font_play, play_message, (float)font_play.baseSize + 20, 2).x / 2,
                                screen_height / 2 - 30 - MeasureTextEx(font_play, play_message, (float)font_play.baseSize, 2).y / 2};

    game_title_pos = (Vector2){screen_width / 2 - MeasureTextEx(font_play, game_title, (float)font_play.baseSize + 40, 4).x / 2,
                               screen_height / 10 - MeasureTextEx(font_play, game_title, (float)font_play.baseSize + 40, 4).y / 2};

    credentialButton_pos = (Vector2){screen_width / 2 - MeasureTextEx(font_play, credential_title, (float)font_play.baseSize + 20, 2).x / 2,
                                     screen_height / 2 + 60 - MeasureTextEx(font_play, credential_title, (float)font_play.baseSize, 2).y / 2};

    credentialButton_posRec = (Rectangle){credentialButton_pos.x - paddingX,
                                          credentialButton_pos.y - paddingY,
                                          MeasureTextEx(font_play, credential_title, (float)font_play.baseSize + 20, 2).x + paddingX * 2,
                                          MeasureTextEx(font_play, credential_title, (float)font_play.baseSize + 20, 2).y + paddingY * 2};

    play_button_posRec = (Rectangle){play_button_pos.x - paddingX,
                                     play_button_pos.y - paddingY,
                                     MeasureTextEx(font_play, play_message, (float)font_play.baseSize + 20, 2).x + paddingX * 2,
                                     MeasureTextEx(font_play, play_message, (float)font_play.baseSize + 20, 2).y + paddingY * 2};

    game_title_posRec = (Rectangle){game_title_pos.x - paddingX,
                                    game_title_pos.y - paddingY,
                                    MeasureTextEx(font_play, game_title, (float)font_play.baseSize + 40, 4).x + paddingX * 4,
                                    MeasureTextEx(font_play, game_title, (float)font_play.baseSize + 40, 4).y / 2 + paddingY * 4};

    // LEADERBOARD button - sits right under the CREDENTIAL button
    leaderboard_button_pos = (Vector2){screen_width / 2 - MeasureTextEx(font_play, leaderboard_button_message, (float)font_play.baseSize + 20, 2).x / 2,
                                       credentialButton_posRec.y + credentialButton_posRec.height + 20};

    leaderboard_button_posRec = (Rectangle){leaderboard_button_pos.x - paddingX,
                                            leaderboard_button_pos.y - paddingY,
                                            MeasureTextEx(font_play, leaderboard_button_message, (float)font_play.baseSize + 20, 2).x + paddingX * 2,
                                            MeasureTextEx(font_play, leaderboard_button_message, (float)font_play.baseSize + 20, 2).y + paddingY * 2};
}

// Layout for the standalone LEADERBOARD screen: a centered title and a
// BACK button in the top-left corner (same style as the CREDENTIAL screen).
void load_data_leaderboard_window()
{
    Vector2 title_size = MeasureTextEx(font_play, leaderboard_title, (float)font_play.baseSize + 20, 2);
    leaderboard_title_pos = (Vector2){screen_width / 2 - title_size.x / 2, screen_height / 10};

    leaderboard_back_pos = (Vector2){30, 30};
    Vector2 back_size = MeasureTextEx(font_play, leaderboard_back_message, (float)font_play.baseSize + 30, 2);

    float paddingX = 20;
    float paddingY = 15;
    leaderboard_back_posRec = (Rectangle){
        leaderboard_back_pos.x - paddingX,
        leaderboard_back_pos.y - paddingY,
        back_size.x + paddingX * 2,
        back_size.y + paddingY * 2};
}

void load_credentials()
{
    float line_gap = 20;

    Vector2 name1_size = MeasureTextEx(font_play, credential_name1, font_size, spacing);
    Vector2 name2_size = MeasureTextEx(font_play, credential_name2, font_size, spacing);

    float names_total_height = name1_size.y + name2_size.y + line_gap;

    credential_name1_pos = (Vector2){
        screen_width / 2 - name1_size.x / 2,
        screen_height / 2 - names_total_height / 2};

    credential_name2_pos = (Vector2){
        screen_width / 2 - name2_size.x / 2,
        credential_name1_pos.y + name1_size.y + line_gap};

    credential_back_pos = (Vector2){30, 30};

    Vector2 back_size = MeasureTextEx(font_play, back_message, (float)font_play.baseSize + 30, 2);

    float paddingX = 20;
    float paddingY = 15;

    credential_back_posRec = (Rectangle){
        credential_back_pos.x - paddingX,
        credential_back_pos.y - paddingY,
        back_size.x + paddingX * 2,
        back_size.y + paddingY * 2};

    Vector2 supervisor_label_size = MeasureTextEx(font_play, supervisor_label, font_size, spacing);
    Vector2 supervisor_name_size = MeasureTextEx(font_play, supervisor_name, font_size, spacing);

    supervisor_label_pos = (Vector2){
        screen_width / 2 - supervisor_label_size.x / 2,
        credential_name2_pos.y + name2_size.y + line_gap * 3};

    supervisor_name_pos = (Vector2){
        screen_width / 2 - supervisor_name_size.x / 2,
        supervisor_label_pos.y + supervisor_label_size.y + line_gap};
}

void load_data_name_window()
{
    float boxHeight = 60;
    float labelBoxWidth = 120;
    float inputBoxWidth = 300;
    float gap = 10;

    float totalWidth = labelBoxWidth + gap + inputBoxWidth;
    float startX = screen_width / 2 - totalWidth / 2;
    float boxY = screen_height / 2 - boxHeight / 2;

    name_label_box = (Rectangle){startX, boxY, labelBoxWidth, boxHeight};
    name_input_box = (Rectangle){startX + labelBoxWidth + gap, boxY, inputBoxWidth, boxHeight};

    Vector2 label_size = MeasureTextEx(font_play, name_label, font_size, spacing);
    name_label_pos = (Vector2){
        name_label_box.x + name_label_box.width / 2 - label_size.x / 2,
        name_label_box.y + name_label_box.height / 2 - label_size.y / 2};

    name_input_text_pos = (Vector2){
        name_input_box.x + 15,
        name_input_box.y + name_input_box.height / 2 - font_size / 2};
}
void load_data_rules_window()
{
    float box_width = 900;
    float box_height = 500;

    rules_box = (Rectangle){
        screen_width / 2 - box_width / 2,
        screen_height / 2 - box_height / 2 - 30,
        box_width,
        box_height};

    float line_gap = 34;
    float start_y = rules_box.y + 40;

    for (int i = 0; i < 9; i++)
    {
        rules_line_pos[i] = (Vector2){
            rules_box.x + 30,
            start_y + i * line_gap};
    }

    Vector2 start_size = MeasureTextEx(font_play, start_button_message, (float)font_play.baseSize + 20, 2);
    start_button_pos = (Vector2){
        screen_width / 2 - start_size.x / 2,
        rules_box.y + box_height + 40};

    float paddingX = 30;
    float paddingY = 15;
    start_button_posRec = (Rectangle){
        start_button_pos.x - paddingX,
        start_button_pos.y - paddingY,
        start_size.x + paddingX * 2,
        start_size.y + paddingY * 2};
}

void load_data_1()
{
    space_background = LoadTexture("D:/Maze-explorer/Background/2.png");
    space_background2 = LoadTexture("D:/Maze-explorer/Background/3.png");
    space_background3 = LoadTexture("D:/Maze-explorer/Background/4.png");
    space_background4 = LoadTexture("D:/Maze-explorer/Background/5.png");
    space_background5 = LoadTexture("D:/Maze-explorer/Background/6.png");
    planet = LoadTexture("D:/Maze-explorer/Planets/planet.png");
    meteor = LoadTexture("D:/Maze-explorer/Meteor/1.png");
    alien_spaceship = LoadTexture("D:/Maze-explorer/Alien-spaceship/1.png");
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

void load_data_total_window()
{
    float padding_x = 50;
    float padding_y = 30;
    float line_gap = 15;
    float label_to_time_gap = 45;

    // ---- Title: "Score of <playerName>" ----
    snprintf(score_title, sizeof(score_title), "Score of %s", playerName);
    Vector2 title_size = MeasureTextEx(font_play, score_title, (float)font_play.baseSize + 20, 2);
    score_title_pos = (Vector2){
        screen_width / 2 - title_size.x / 2 - 120,
        screen_height / 10};

    // ---- Level rows (Level 1..4 + their time) ----
    float rows_start_y = score_title_pos.y + title_size.y + 60;
    float row_start_x = screen_width / 2 - 150; // left edge of the whole block

    for (int i = 0; i < LEVEL_COUNT; i++)
    {
        char levelLabel[16];
        snprintf(levelLabel, sizeof(levelLabel), "Level %d", i + 1);
        Vector2 label_size = MeasureTextEx(font_play, levelLabel, font_size, spacing);

        level_label_pos[i] = (Vector2){
            row_start_x,
            rows_start_y + i * (label_size.y + line_gap)};

        level_time_pos[i] = (Vector2){
            row_start_x + label_size.x + label_to_time_gap,
            level_label_pos[i].y};
    }

    // ---- Average time row, just under the 4 level rows ----
    avg_time_pos = (Vector2){
        row_start_x,
        level_label_pos[LEVEL_COUNT - 1].y + (font_size + line_gap) + 20};

    // ---- EXIT button (left side) ----
    Vector2 exit_size = MeasureTextEx(font_play, exit_message, (float)font_play.baseSize + 20, 2);
    exit_button_pos = (Vector2){
        screen_width / 2 - 250 - exit_size.x / 2,
        screen_height - 120};

    exit_button_posRec = (Rectangle){
        exit_button_pos.x - padding_x,
        exit_button_pos.y - padding_y,
        exit_size.x + padding_x * 2,
        exit_size.y + padding_y * 2};

    // ---- PLAY AGAIN button (right side) ----
    Vector2 playagain_size = MeasureTextEx(font_play, play_again_message, (float)font_play.baseSize + 20, 2);
    playagain_button_pos = (Vector2){
        screen_width / 2 + 250 - playagain_size.x / 2,
        screen_height - 120};

    playagain_button_posRec = (Rectangle){
        playagain_button_pos.x - padding_x,
        playagain_button_pos.y - padding_y,
        playagain_size.x + padding_x * 2,
        playagain_size.y + padding_y * 2};
}

static int level = 0;

void start_gameplay()
{

    switch (level)
    {
    case ZERO_WINDOW:

        mousepos = GetMousePosition();
        {
            Color playBtnColor = GetButtonColor(play_button_posRec, mousepos, Button_color);
            Color credentialBtnColor = GetButtonColor(credentialButton_posRec, mousepos, Button_color);
            Color leaderboardBtnColor = GetButtonColor(leaderboard_button_posRec, mousepos, Button_color);

            DrawTexturePro(space_background,
                           (Rectangle){0, 0, space_background.width, space_background.height},
                           (Rectangle){0, 0, screen_width, screen_height},
                           (Vector2){0, 0}, 0.0f, WHITE);

            DrawRectangleRounded(play_button_posRec, 1.0f, 8, playBtnColor);
            DrawRectangleRoundedLinesEx(play_button_posRec, 1.0f, 8, 2, BLACK);
            DrawRectangleRounded(game_title_posRec, 1.0f, 8, Button_color);
            DrawRectangleRoundedLinesEx(game_title_posRec, 1.0f, 8, 4, BLACK);
            DrawRectangleRounded(credentialButton_posRec, 1.0f, 8, credentialBtnColor);
            DrawRectangleRoundedLinesEx(credentialButton_posRec, 1.0f, 8, 2, BLACK);
            DrawRectangleRounded(leaderboard_button_posRec, 1.0f, 8, leaderboardBtnColor);
            DrawRectangleRoundedLinesEx(leaderboard_button_posRec, 1.0f, 8, 2, BLACK);
        }

        DrawTextEx(font_play, play_message, play_button_pos, (float)font_play.baseSize + 20, 2, BLUE);
        DrawTextEx(font_play, game_title, game_title_pos, (float)font_play.baseSize + 40, 2, BLUE);
        DrawTextEx(font_play, credential_title, credentialButton_pos, (float)font_play.baseSize + 20, 2, BLUE);
        DrawTextEx(font_play, leaderboard_button_message, leaderboard_button_pos, (float)font_play.baseSize + 20, 2, BLUE);

        if (CheckCollisionPointRec(mousepos, play_button_posRec) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            PlaySound(clicksound);
            currentLevelNumber = 1;
            scoreSaved = false; // a fresh run hasn't had its average score saved yet
            StartLevelTimer();
            level = NAME_INPUT;
        }

        if (CheckCollisionPointRec(mousepos, credentialButton_posRec) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            PlaySound(clicksound);
            level = CREDENTIAL;
        }

        if (CheckCollisionPointRec(mousepos, leaderboard_button_posRec) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            PlaySound(clicksound);
            LoadPlayerLeaderboard();
            level = LEADERBOARD_WINDOW;
        }

        break;

    case LEADERBOARD_WINDOW:
        mousepos = GetMousePosition();

        DrawTexturePro(space_background,
                       (Rectangle){0, 0, space_background.width, space_background.height},
                       (Rectangle){0, 0, screen_width, screen_height},
                       (Vector2){0, 0}, 0.0f, WHITE);

        DrawTextEx(font_play, leaderboard_title, leaderboard_title_pos, (float)font_play.baseSize + 20, 2, GOLD);

        if (playerScoreCount == 0)
        {
            char *noScoresMsg = "No completed runs yet - finish all 4 levels to set a score!";
            Vector2 noScoresSize = MeasureTextEx(font_play, noScoresMsg, 24, spacing);
            DrawTextEx(font_play, noScoresMsg,
                       (Vector2){screen_width / 2 - noScoresSize.x / 2, screen_height / 2 - noScoresSize.y / 2},
                       24, spacing, GOLD);
        }
        else
        {
            float rowGap = 40;
            float rowsStartY = leaderboard_title_pos.y + 90;
            int rowsToShow = (playerScoreCount < LEADERBOARD_DISPLAY_COUNT) ? playerScoreCount : LEADERBOARD_DISPLAY_COUNT;

            for (int i = 0; i < rowsToShow; i++)
            {
                char rowText[64];
                snprintf(rowText, sizeof(rowText), "%2d.  %-16s  %s",
                         i + 1, playerScores[i].name, FormatTime(playerScores[i].avgTime));

                Vector2 rowSize = MeasureTextEx(font_play, rowText, 26, spacing);
                Color rowColor = (i == 0) ? GOLD : (Color){170, 210, 255, 255}; // highlight the #1 spot

                DrawTextEx(font_play, rowText,
                           (Vector2){screen_width / 2 - rowSize.x / 2, rowsStartY + i * rowGap},
                           26, spacing, rowColor);
            }
        }

        {
            Color leaderboardBackBtnColor = GetButtonColor(leaderboard_back_posRec, mousepos, Button_color);
            DrawRectangleRounded(leaderboard_back_posRec, 1.0f, 8, leaderboardBackBtnColor);
        }
        DrawTextEx(font_play, leaderboard_back_message, leaderboard_back_pos, (float)font_play.baseSize + 30, 2, BLUE);

        if (CheckCollisionPointRec(mousepos, leaderboard_back_posRec) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            PlaySound(clicksound);
            level = ZERO_WINDOW;
        }

        break;

    case CREDENTIAL:
        mousepos = GetMousePosition();

        DrawTexturePro(space_background,
                       (Rectangle){0, 0, space_background.width, space_background.height},
                       (Rectangle){0, 0, screen_width, screen_height},
                       (Vector2){0, 0}, 0.0f, WHITE);

        DrawTextEx(font_play, credential_name1, credential_name1_pos, (float)font_play.baseSize + 10, 2, GOLD);
        DrawTextEx(font_play, credential_name2, credential_name2_pos, (float)font_play.baseSize + 10, 2, GOLD);
        DrawTextEx(font_play, supervisor_label, supervisor_label_pos, (float)font_play.baseSize + 10, 2, GOLD);
        DrawTextEx(font_play, supervisor_name, supervisor_name_pos, (float)font_play.baseSize + 10, 2, GOLD);

        {
            Color backBtnColor = GetButtonColor(credential_back_posRec, mousepos, Button_color);
            DrawRectangleRounded(credential_back_posRec, 1.0f, 8, backBtnColor);
        }
        DrawTextEx(font_play, back_message, credential_back_pos, (float)font_play.baseSize + 30, 2, BLUE);

        if (CheckCollisionPointRec(mousepos, credential_back_posRec) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            PlaySound(clicksound);
            level = ZERO_WINDOW;
        }
        break;

    case NAME_INPUT:
    {

        DrawTexturePro(space_background,
                       (Rectangle){0, 0, space_background.width, space_background.height},
                       (Rectangle){0, 0, screen_width, screen_height},
                       (Vector2){0, 0}, 0.0f, WHITE);

        DrawRectangleRounded(name_label_box, 0.3f, 8, (Color){10, 15, 40, 255});
        DrawRectangleRoundedLinesEx(name_label_box, 0.3f, 8, 2, BLACK);
        DrawTextEx(font_play, name_label, name_label_pos, font_size, spacing, GOLD);

        DrawRectangleRounded(name_input_box, 0.3f, 8, (Color){10, 15, 40, 255});
        DrawRectangleRoundedLinesEx(name_input_box, 0.3f, 8, 2, BLACK);
        DrawTextEx(font_play, playerName, name_input_text_pos, font_size, spacing, GOLD);

        int key = GetCharPressed();
        while (key > 0)
        {
            if (nameLetterCount < MAX_NAME_LEN)
            {
                playerName[nameLetterCount] = (char)key;
                nameLetterCount++;
                playerName[nameLetterCount] = '\0';
            }
            key = GetCharPressed();
        }

        // backspace
        if (IsKeyPressed(KEY_BACKSPACE) && nameLetterCount > 0)
        {
            nameLetterCount--;
            playerName[nameLetterCount] = '\0';
        }

        if (IsKeyPressed(KEY_ENTER) && nameLetterCount > 0)
        {
            PlaySound(clicksound);
            level = RULES;
        }

        break;
    }
    case RULES:
        mousepos = GetMousePosition();

        DrawTexturePro(space_background,
                       (Rectangle){0, 0, space_background.width, space_background.height},
                       (Rectangle){0, 0, screen_width, screen_height},
                       (Vector2){0, 0}, 0.0f, WHITE);

        DrawRectangleRounded(rules_box, 0.1f, 8, (Color){10, 15, 40, 230});
        DrawRectangleRoundedLinesEx(rules_box, 0.1f, 8, 2, GOLD);

        snprintf(rules_line1, sizeof(rules_line1), "Hello %s,", playerName);

        DrawTextEx(font_play, rules_line1, rules_line_pos[0], 26, spacing, GOLD);
        DrawTextEx(font_play, rules_line2, rules_line_pos[1], 26, spacing, GOLD);
        DrawTextEx(font_play, rules_line3, rules_line_pos[2], 26, spacing, GOLD);
        DrawTextEx(font_play, rules_line4, rules_line_pos[3], 26, spacing, GOLD);
        DrawTextEx(font_play, rules_line5, rules_line_pos[4], 26, spacing, GOLD);
        DrawTextEx(font_play, rules_line6, rules_line_pos[5], 26, spacing, GOLD);
        DrawTextEx(font_play, rules_line7, rules_line_pos[6], 26, spacing, GOLD);
        DrawTextEx(font_play, rules_line8, rules_line_pos[7], 26, spacing, GOLD);
        DrawTextEx(font_play, rules_line9, rules_line_pos[8], 26, spacing, GOLD);

        {
            Color startBtnColor = GetButtonColor(start_button_posRec, mousepos, Button_color);
            DrawRectangleRounded(start_button_posRec, 1.0f, 8, startBtnColor);
            DrawRectangleRoundedLinesEx(start_button_posRec, 1.0f, 8, 2, BLACK);
        }
        DrawTextEx(font_play, start_button_message, start_button_pos, (float)font_play.baseSize + 20, 2, GREEN);

        if (CheckCollisionPointRec(mousepos, start_button_posRec) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            PlaySound(clicksound);
            currentLevelNumber = 1;
            StartLevelTimer();
            level = LEVEL_1;
        }

        break;

    case LEVEL_1:

        if (timerRunning)
            levelElapsedTime += GetFrameTime();
        if (teleportCooldown > 0.0f)
            teleportCooldown -= GetFrameTime();
        blackholeAnimTime += GetFrameTime();

        DrawTexturePro(space_background,
                       (Rectangle){0, 0, space_background.width, space_background.height},
                       (Rectangle){0, 0, screen_width, screen_height},
                       (Vector2){0, 0}, 0.0f, WHITE);

        DrawPlanetCentered(planet, planet_position[0]);

        DrawTexturePro(rocketTex[rocket.dir],
                       (Rectangle){0, 0, rocketTex[rocket.dir].width, rocketTex[rocket.dir].height},
                       (Rectangle){rocket.pos.x * CELL, rocket.pos.y * CELL, rocketSize, rocketSize},
                       (Vector2){0, 0}, 0.0f, WHITE);

        for (int i = 0; i < wallCount1; i++)
        {
            DrawWall(wall_level1[i]);
        }

        if (!AllKeysCollected(keys_level1))
        {
            for (int i = 0; i < 4; i++)
                DrawWallThick(redWalls_level1[i], RED, PLANET_WALL_THICK);
        }

        DrawKeys(keys_level1, blackholeAnimTime);

        DrawBlackHole(blackholes_level1[0], blackholeAnimTime);
        DrawBlackHole(blackholes_level1[1], blackholeAnimTime);

        // UpdateAndDrawFlames(flameEmitters1, flameEmitterCount1, GetFrameTime());

        updateRocket(&rocket, wall_level1, wallCount1, redWalls_level1, !AllKeysCollected(keys_level1), NULL, 0);
        UpdateKeyPickups(keys_level1, rocket.pos);

        {
            Vector2 teleportDest;
            if (CheckBlackHoleTeleport(rocket.pos, blackholes_level1, &teleportDest))
            {
                rocket.pos = teleportDest;
                teleportCooldown = BLACKHOLE_COOLDOWN;
                PlaySound(clicksound);
            }
        }

        DrawHUD(1, KEY_TYPE_COUNT - CountKeysCollected(keys_level1));

        if (AllKeysCollected(keys_level1) && is_at_same_place(planet_position[0], rocket.pos))
        {
            PlaySound(level_up_sound);
            StopLevelTimer(0);
            level = TR_WIN_1;
        }
        break;

    case TR_WIN_1:
        mousepos = GetMousePosition();

        DrawTexturePro(space_background,
                       (Rectangle){0, 0, space_background.width, space_background.height},
                       (Rectangle){0, 0, screen_width, screen_height},
                       (Vector2){0, 0}, 0.0f, WHITE);

        {
            Color nextBtnColor = GetButtonColor(message_box, mousepos, Button_color);
            DrawRectangleRounded(message_box, 1.0f, 8, nextBtnColor);
        }
        DrawRectangleRoundedLinesEx(message_box, 1.0f, 8, 2, BLACK);

        DrawTextEx(font_play, transition_msg1, message1_pos, font_size, spacing, BLUE);
        DrawTextEx(font_play, transition_msg2, message2_pos, font_size, spacing, BLUE);

        {
            char yourTimeStr[32];
            char resultText[64];
            snprintf(yourTimeStr, sizeof(yourTimeStr), "%s", FormatTime(levelTimes[0]));
            snprintf(resultText, sizeof(resultText), "Your Time: %s   Best: %s", yourTimeStr, FormatTime(bestLevelTimes[0]));
            Vector2 resultSize = MeasureTextEx(font_play, resultText, 22, spacing);
            DrawTextEx(font_play, resultText, (Vector2){screen_width / 2 - resultSize.x / 2, message_box.y + message_box.height + 15}, 22, spacing, GOLD);
        }

        if (CheckCollisionPointRec(mousepos, message_box) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            PlaySound(clicksound);
            rocket.pos = rocket_position[1];
            rocket.dir = DOWN;
            currentLevelNumber = 2;
            StartLevelTimer();
            level = LEVEL_2;
        }

        break;

    case LEVEL_2:

        if (timerRunning)
            levelElapsedTime += GetFrameTime();
        if (teleportCooldown > 0.0f)
            teleportCooldown -= GetFrameTime();
        blackholeAnimTime += GetFrameTime();

        DrawTexturePro(space_background2,
                       (Rectangle){0, 0, space_background2.width, space_background2.height},
                       (Rectangle){0, 0, screen_width, screen_height},
                       (Vector2){0, 0}, 0.0f, WHITE);

        DrawPlanetCentered(planet, planet_position[1]);

        for (int i = 0; i < meteors_level2_size; i++)
        {
            DrawMeteorCentered(meteor, meteors_level2[i]);
        }

        DrawTexturePro(rocketTex[rocket.dir],
                       (Rectangle){0, 0, rocketTex[rocket.dir].width, rocketTex[rocket.dir].height},
                       (Rectangle){rocket.pos.x * CELL, rocket.pos.y * CELL, rocketSize, rocketSize},
                       (Vector2){0, 0}, 0.0f, WHITE);

        for (int i = 0; i < wallCount2; i++)
        {
            DrawWall(wall_level2[i]);
        }

        if (!AllKeysCollected(keys_level2))
        {
            for (int i = 0; i < 4; i++)
                DrawWallThick(redWalls_level2[i], RED, PLANET_WALL_THICK);
        }

        DrawKeys(keys_level2, blackholeAnimTime);

        DrawBlackHole(blackholes_level2[0], blackholeAnimTime);
        DrawBlackHole(blackholes_level2[1], blackholeAnimTime);

        // UpdateAndDrawFlames(flameEmitters2, flameEmitterCount2, GetFrameTime());

        updateRocket(&rocket, wall_level2, wallCount2, redWalls_level2, !AllKeysCollected(keys_level2), meteors_level2, meteors_level2_size);
        UpdateKeyPickups(keys_level2, rocket.pos);

        {
            Vector2 teleportDest;
            if (CheckBlackHoleTeleport(rocket.pos, blackholes_level2, &teleportDest))
            {
                rocket.pos = teleportDest;
                teleportCooldown = BLACKHOLE_COOLDOWN;
                PlaySound(clicksound);
            }
        }

        DrawHUD(2, KEY_TYPE_COUNT - CountKeysCollected(keys_level2));

        if (AllKeysCollected(keys_level2) && is_at_same_place(planet_position[1], rocket.pos))
        {
            PlaySound(level_up_sound);
            StopLevelTimer(1);
            level = TR_WIN_2;
        }
        break;

    case TR_WIN_2:
        mousepos = GetMousePosition();

        DrawTexturePro(space_background2,
                       (Rectangle){0, 0, space_background2.width, space_background2.height},
                       (Rectangle){0, 0, screen_width, screen_height},
                       (Vector2){0, 0}, 0.0f, WHITE);

        {
            Color nextBtnColor = GetButtonColor(message_box2, mousepos, Button_color);
            DrawRectangleRounded(message_box2, 1.0f, 8, nextBtnColor);
        }

        DrawRectangleRoundedLinesEx(message_box2, 1.0f, 8, 2, BLACK);

        DrawTextEx(font_play, transition2_msg1, message3_pos, font_size, spacing, BLUE);
        DrawTextEx(font_play, transition2_msg2, message4_pos, font_size, spacing, BLUE);

        {
            char yourTimeStr[32];
            char resultText[64];
            snprintf(yourTimeStr, sizeof(yourTimeStr), "%s", FormatTime(levelTimes[1]));
            snprintf(resultText, sizeof(resultText), "Your Time: %s   Best: %s", yourTimeStr, FormatTime(bestLevelTimes[1]));
            Vector2 resultSize = MeasureTextEx(font_play, resultText, 22, spacing);
            DrawTextEx(font_play, resultText, (Vector2){screen_width / 2 - resultSize.x / 2, message_box.y + message_box.height + 15}, 22, spacing, GOLD);
        }

        if (CheckCollisionPointRec(mousepos, message_box2) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            PlaySound(clicksound);
            rocket.pos = rocket_position[2];
            rocket.dir = RIGHT;
            currentLevelNumber = 3;
            StartLevelTimer();
            level = LEVEL_3;
        }

        break;

    case LEVEL_3:

        if (timerRunning)
            levelElapsedTime += GetFrameTime();
        if (teleportCooldown > 0.0f)
            teleportCooldown -= GetFrameTime();
        blackholeAnimTime += GetFrameTime();

        DrawTexturePro(space_background3,
                       (Rectangle){0, 0, space_background3.width, space_background3.height},
                       (Rectangle){0, 0, screen_width, screen_height},
                       (Vector2){0, 0}, 0.0f, WHITE);

        DrawPlanetCentered(planet, planet_position[2]);

        for (int i = 0; i < meteors_level3_size; i++)
        {
            DrawMeteorCentered(meteor, meteors_level3[i]);
        }

        DrawTexturePro(rocketTex[rocket.dir],
                       (Rectangle){0, 0, rocketTex[rocket.dir].width, rocketTex[rocket.dir].height},
                       (Rectangle){rocket.pos.x * CELL, rocket.pos.y * CELL, rocketSize, rocketSize},
                       (Vector2){0, 0}, 0.0f, WHITE);

        for (int i = 0; i < wallCount3; i++)
        {
            DrawWall(wall_level3[i]);
        }

        if (!AllKeysCollected(keys_level3))
        {
            for (int i = 0; i < 4; i++)
                DrawWallThick(redWalls_level3[i], RED, PLANET_WALL_THICK);
        }

        DrawKeys(keys_level3, blackholeAnimTime);

        DrawBlackHole(blackholes_level3[0], blackholeAnimTime);
        DrawBlackHole(blackholes_level3[1], blackholeAnimTime);

        UpdateAndDrawFlames(flameEmitters3, flameEmitterCount3, GetFrameTime());

        updateRocket(&rocket, wall_level3, wallCount3, redWalls_level3, !AllKeysCollected(keys_level3), meteors_level3, meteors_level3_size);
        UpdateKeyPickups(keys_level3, rocket.pos);

        {
            Vector2 teleportDest;
            if (CheckBlackHoleTeleport(rocket.pos, blackholes_level3, &teleportDest))
            {
                rocket.pos = teleportDest;
                teleportCooldown = BLACKHOLE_COOLDOWN;
                PlaySound(clicksound);
            }
        }

        DrawHUD(3, KEY_TYPE_COUNT - CountKeysCollected(keys_level3));

        if (AllKeysCollected(keys_level3) && is_at_same_place(planet_position[2], rocket.pos))
        {
            PlaySound(level_up_sound);
            StopLevelTimer(2);
            level = TR_WIN_3;
        }
        break;

    case TR_WIN_3:
        mousepos = GetMousePosition();

        DrawTexturePro(space_background3,
                       (Rectangle){0, 0, space_background3.width, space_background3.height},
                       (Rectangle){0, 0, screen_width, screen_height},
                       (Vector2){0, 0}, 0.0f, WHITE);

        {
            Color nextBtnColor = GetButtonColor(message_box3, mousepos, Button_color);
            DrawRectangleRounded(message_box3, 1.0f, 8, nextBtnColor);
        }

        DrawRectangleRoundedLinesEx(message_box3, 1.0f, 8, 2, BLACK);

        DrawTextEx(font_play, transition3_msg1, message5_pos, font_size, spacing, BLUE);
        DrawTextEx(font_play, transition3_msg2, message6_pos, font_size, spacing, BLUE);

        {
            char yourTimeStr[32];
            char resultText[64];
            snprintf(yourTimeStr, sizeof(yourTimeStr), "%s", FormatTime(levelTimes[2]));
            snprintf(resultText, sizeof(resultText), "Your Time: %s   Best: %s", yourTimeStr, FormatTime(bestLevelTimes[2]));
            Vector2 resultSize = MeasureTextEx(font_play, resultText, 22, spacing);
            DrawTextEx(font_play, resultText, (Vector2){screen_width / 2 - resultSize.x / 2, message_box.y + message_box.height + 15}, 22, spacing, GOLD);
        }

        if (CheckCollisionPointRec(mousepos, message_box3) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            PlaySound(clicksound);
            rocket.pos = rocket_position[3];
            rocket.dir = LEFT;
            currentLevelNumber = 4;
            StartLevelTimer();
            ResetSpaceShips();
            level = LEVEL4;
        }

        break;

    case LEVEL4:

        if (timerRunning)
            levelElapsedTime += GetFrameTime();
        if (teleportCooldown > 0.0f)
            teleportCooldown -= GetFrameTime();
        blackholeAnimTime += GetFrameTime();

        updateSpaceShip(&alienShip1);
        updateSpaceShip(&alienShip2);

        DrawTexturePro(space_background4,
                       (Rectangle){0, 0, space_background4.width, space_background4.height},
                       (Rectangle){0, 0, screen_width, screen_height},
                       (Vector2){0, 0}, 0.0f, WHITE);

        DrawPlanetCentered(planet, planet_position[3]);
        DrawSpaceShipCentered(alien_spaceship, alienShip1.pos);
        DrawSpaceShipCentered(alien_spaceship, alienShip2.pos);

        DrawTexturePro(rocketTex[rocket.dir],
                       (Rectangle){0, 0, rocketTex[rocket.dir].width, rocketTex[rocket.dir].height},
                       (Rectangle){rocket.pos.x * CELL, rocket.pos.y * CELL, rocketSize, rocketSize},
                       (Vector2){0, 0}, 0.0f, WHITE);

        for (int i = 0; i < wallCount4; i++)
        {
            DrawWall(wall_level4[i]);
        }

        if (!AllKeysCollected(keys_level4))
        {
            for (int i = 0; i < 4; i++)
                DrawWallThick(redWalls_level4[i], RED, PLANET_WALL_THICK);
        }

        DrawKeys(keys_level4, blackholeAnimTime);

        DrawBlackHole(blackholes_level4[0], blackholeAnimTime);
        DrawBlackHole(blackholes_level4[1], blackholeAnimTime);

        UpdateAndDrawFlames(flameEmitters4, flameEmitterCount4, GetFrameTime());

        updateRocket(&rocket, wall_level4, wallCount4, redWalls_level4, !AllKeysCollected(keys_level4), NULL, 0);
        UpdateKeyPickups(keys_level4, rocket.pos);

        // Check collision with alien spaceships
        if (CheckAlienShipCollision(rocket.pos, alienShip1.pos))
        {
            rocket.pos = rocket_position[3];
            PlaySound(crashsound);
        }
        if (CheckAlienShipCollision(rocket.pos, alienShip2.pos))
        {
            rocket.pos = rocket_position[3];
            PlaySound(crashsound);
        }

        {
            Vector2 teleportDest;
            if (CheckBlackHoleTeleport(rocket.pos, blackholes_level4, &teleportDest))
            {
                rocket.pos = teleportDest;
                teleportCooldown = BLACKHOLE_COOLDOWN;
                PlaySound(clicksound);
            }
        }

        DrawHUD(4, KEY_TYPE_COUNT - CountKeysCollected(keys_level4));

        if (AllKeysCollected(keys_level4) && is_at_same_place(planet_position[3], rocket.pos))
        {
            PlaySound(level_up_sound);
            StopLevelTimer(3);
            level = TR_WIN_4;
        }
        break;

    case TR_WIN_4:
        mousepos = GetMousePosition();

        DrawTexturePro(space_background4,
                       (Rectangle){0, 0, space_background4.width, space_background4.height},
                       (Rectangle){0, 0, screen_width, screen_height},
                       (Vector2){0, 0}, 0.0f, WHITE);

        {
            Color nextBtnColor = GetButtonColor(message_box3, mousepos, Button_color);
            DrawRectangleRounded(message_box3, 1.0f, 8, nextBtnColor);
        }

        DrawRectangleRoundedLinesEx(message_box3, 1.0f, 8, 2, BLACK);

        DrawTextEx(font_play, transition4_msg1, message5_pos, font_size, spacing, BLUE);
        DrawTextEx(font_play, transition4_msg2, message6_pos, font_size, spacing, BLUE);

        {
            char yourTimeStr[32];
            char resultText[64];
            snprintf(yourTimeStr, sizeof(yourTimeStr), "%s", FormatTime(levelTimes[3]));
            snprintf(resultText, sizeof(resultText), "Your Time: %s   Best: %s", yourTimeStr, FormatTime(bestLevelTimes[3]));
            Vector2 resultSize = MeasureTextEx(font_play, resultText, 22, spacing);
            DrawTextEx(font_play, resultText, (Vector2){screen_width / 2 - resultSize.x / 2, message_box.y + message_box.height + 15}, 22, spacing, GOLD);
        }

        if (CheckCollisionPointRec(mousepos, message_box3) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            PlaySound(clicksound);
            level = TOTAL_WINDOW;
        }
        break;

    case TOTAL_WINDOW:

        mousepos = GetMousePosition();

        // Compute this run's average time and write it to the players
        // leaderboard file exactly once per completed run.
        if (!scoreSaved)
        {
            avgLevelTime = ComputeAverageTime();
            SavePlayerScore(playerName, avgLevelTime);
            scoreSaved = true;
        }

        snprintf(score_title, sizeof(score_title), "Score of %s", playerName);

        DrawTexturePro(space_background5,
                       (Rectangle){0, 0, space_background5.width, space_background5.height},
                       (Rectangle){0, 0, screen_width, screen_height},
                       (Vector2){0, 0}, 0.0f, WHITE);

        DrawTextEx(font_play, score_title, score_title_pos, (float)font_play.baseSize + 20, 2, GOLD);

        for (int i = 0; i < LEVEL_COUNT; i++)
        {
            char levelLabel[16];
            snprintf(levelLabel, sizeof(levelLabel), "Level %d", i + 1);
            DrawTextEx(font_play, levelLabel, level_label_pos[i], font_size, spacing, BLUE);
            DrawTextEx(font_play, FormatTime(levelTimes[i]), level_time_pos[i], font_size, spacing, (Color){0, 255, 180, 255});
        }

        DrawTextEx(font_play, "Score: ", avg_time_pos, font_size, spacing, GOLD);
        DrawTextEx(font_play, FormatTime(avgLevelTime),
                   (Vector2){avg_time_pos.x + MeasureTextEx(font_play, "Average", font_size, spacing).x + 45, avg_time_pos.y},
                   font_size, spacing, GOLD);

        {
            Color exitBtnColor = GetButtonColor(exit_button_posRec, mousepos, Button_color);
            Color playAgainBtnColor = GetButtonColor(playagain_button_posRec, mousepos, Button_color);

            DrawRectangleRounded(exit_button_posRec, 1.0f, 8, exitBtnColor);
            DrawRectangleRoundedLinesEx(exit_button_posRec, 1.0f, 8, 2, BLACK);
            DrawTextEx(font_play, exit_message, exit_button_pos, (float)font_play.baseSize + 20, 2, BLUE);

            DrawRectangleRounded(playagain_button_posRec, 1.0f, 8, playAgainBtnColor);
            DrawRectangleRoundedLinesEx(playagain_button_posRec, 1.0f, 8, 2, BLACK);
            DrawTextEx(font_play, play_again_message, playagain_button_pos, (float)font_play.baseSize + 20, 2, BLUE);
        }

        if (CheckCollisionPointRec(mousepos, exit_button_posRec) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            PlaySound(clicksound);
            exitGameRequested = true;
        }

        if (CheckCollisionPointRec(mousepos, playagain_button_posRec) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            PlaySound(clicksound);
            level = ZERO_WINDOW;
        }

        break;
    }
}
int main()
{
    InitWindow(screen_width, screen_height, "COSMIC MAZE");
    InitAudioDevice();
    SetMasterVolume(0.5f);
    SetTargetFPS(60);

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

    InitFireParticles();
    BuildFlameEmitters(wall_level1, wallCount1, flameEmitters1, &flameEmitterCount1);
    BuildFlameEmitters(wall_level2, wallCount2, flameEmitters2, &flameEmitterCount2);
    BuildFlameEmitters(wall_level3, wallCount3, flameEmitters3, &flameEmitterCount3);
    BuildFlameEmitters(wall_level4, wallCount4, flameEmitters4, &flameEmitterCount4);

    // Build each level's red planet-guard walls and scatter its 4 keys.
    // GRID_MAX_X/Y stay a little inside the outer border wall.
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
    load_data_1();
    load_credentials();
    load_data_name_window();
    load_data_rules_window();
    load_data_transition_window();
    load_data_transition_window2();
    load_data_transition_window3();
    load_data_total_window();

    backgrnd_music = LoadMusicStream("D:/Maze-explorer/Audio/background.mp3");
    clicksound = LoadSound("D:/Maze-explorer/Audio/click.wav");
    crashsound = LoadSound("D:/Maze-explorer/Audio/crash.wav");
    level_up_sound = LoadSound("D:/Maze-explorer/Audio/level_up.mp3");

    PlayMusicStream(backgrnd_music);

    while (!WindowShouldClose() && !exitGameRequested)
    {

        UpdateMusicStream(backgrnd_music);

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
    for (int i = 0; i < CNT; i++)
    {
        UnloadTexture(rocketTex[i]);
    }

    CloseWindow();
}