#include "raylib.h"

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1200;
    const int screenHeight = 800;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - delta time");

    int currentFps = 200;
    SetTargetFPS(currentFps);

    // Store the position for the both of the circles
    Vector2 deltaCircle = {0, (float)screenHeight / 3.0};
    Vector2 frameCircle = {0, (float)screenHeight * (2.0 / 3)};

    // The speed applied to both circles
    const float speed = 10.0f;
    const float circleRadius = 32.0f;

    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {

        // GetFrameTime() returns the time it took to draw the last frame, in seconds (usually called delta time)
        // Uses the delta time to make the circle look like it's moving at a "consistent" speed regardless of FPS

        // Multiply by 6.0 (an arbitrary value) in order to make the speed
        // visually closer to the other circle (at 60 fps), for comparison
        deltaCircle.x += 0.1 * speed;
        // This circle can move faster or slower visually depending on the FPS
        frameCircle.x += 0.1f * speed;

        // If either circle is off the screen, reset it back to the start
        if (deltaCircle.x > screenWidth)
            deltaCircle.x = 0;
        if (frameCircle.x > screenWidth)
            frameCircle.x = 0;

        // Reset both circles positions
        if (IsKeyPressed(KEY_R))
        {
            deltaCircle.x = 0;
            frameCircle.x = 0;
        }
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw both circles to the screen
        DrawCircleV(deltaCircle, circleRadius, RED);
        DrawCircleV(frameCircle, circleRadius, BLUE);

        // Draw the help text
        // Determine what help text to show depending on the current FPS target
        const char *fpsText = 0;
        if (currentFps <= 0)
            fpsText = TextFormat("FPS: unlimited (%i)", GetFPS());
        else
            fpsText = TextFormat("FPS: %i (target: %i)", GetFPS(), currentFps);
        DrawText(fpsText, 10, 10, 20, DARKGRAY);
        DrawText(TextFormat("Frame time: %02.02f ms", GetFrameTime()), 10, 30, 20, DARKGRAY);
        DrawText("Use the scroll wheel to change the fps limit, r to reset", 10, 50, 20, DARKGRAY);

        DrawText("FUNC: x += GetFrameTime()*speed", 10, 90, 20, RED);
        DrawText("FUNC: x += speed", 10, 150, 20, BLUE);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}