#include "raylib.h"

int main()
{
    int WIDTH = 1200;
    int HEIGHT = 800;

    InitWindow(WIDTH, HEIGHT, "ball bouncing");
    int currentFPS = 200;
    SetTargetFPS(currentFPS);

    float speed = 10.0;
    float radius = 35.0;

    Vector2 circle_1 = {radius, (float)HEIGHT / 4.0};
    Vector2 circle_2 = {radius, (float)HEIGHT * 2.0 / 4.0};

    float circle_1_dir = 1.0;
    float circle_2_dir = 1.0;

    while (!WindowShouldClose())
    {

        circle_1.x += GetFrameTime() * speed * 10 * circle_1_dir;
        circle_2.x += GetFrameTime() * speed * 10 * circle_2_dir;

        if (circle_1.x + radius >= WIDTH)
        {
            circle_1.x = WIDTH - radius;
            circle_1_dir = -1.0;
        }

        if (circle_2.x + radius >= WIDTH)
        {
            circle_2.x = WIDTH - radius;
            circle_2_dir = -1.0;
        }

        if (circle_1.x - radius <= 0)
        {
            circle_1.x = radius;
            circle_1_dir = 1.0f;
        }
        if (circle_2.x - radius <= 0)
        {
            circle_2.x = radius;
            circle_2_dir = 1.0;
        }

        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_LEFT))
        {
            speed = 20.0;
        }
        else
        {
            speed = 10;
        }

        BeginDrawing();
        ClearBackground(BLACK);

        const char *fpsTEXT = TextFormat("FPS: %d (target: %d)", GetFPS(), currentFPS);
        DrawText(fpsTEXT, 10, 10, 25, WHITE);
        DrawText(TextFormat("Frame time: %02.02f ms", GetFrameTime()), 10, 60, 25, WHITE);

        DrawCircleV(circle_1, radius, RED);
        DrawCircleV(circle_2, radius, PINK);

        EndDrawing();
    }

    CloseWindow();
}