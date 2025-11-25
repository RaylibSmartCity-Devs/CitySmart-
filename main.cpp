#include <iostream>
#include "raylib.h"

int main() {
    InitWindow(960, 540, "City Map Test");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // =======================
        // Roads
        // =======================
        DrawRectangle(0, 220, 960, 100, DARKGRAY);     // Main horizontal road
        DrawRectangle(430, 0, 100, 540, DARKGRAY);     // Main vertical road

        // Road lanes (white lines)
        for (int x = 0; x < 960; x += 40)
            DrawRectangle(x, 265, 20, 5, RAYWHITE);

        for (int y = 0; y < 540; y += 40)
            DrawRectangle(475, y, 5, 20, RAYWHITE);

        // =======================
        // Buildings
        // =======================
        DrawRectangle(50, 50, 200, 150, LIGHTGRAY);
        DrawRectangleLines(50, 50, 200, 150, BLACK);

        DrawRectangle(250, 350, 150, 150, LIGHTGRAY);
        DrawRectangleLines(250, 350, 150, 150, BLACK);

        DrawRectangle(600, 50, 200, 150, LIGHTGRAY);
        DrawRectangleLines(600, 50, 200, 150, BLACK);

        DrawRectangle(600, 350, 250, 150, LIGHTGRAY);
        DrawRectangleLines(600, 350, 250, 150, BLACK);

        // =======================
        // Park area
        // =======================
        DrawRectangle(100, 350, 120, 120, GREEN);
        DrawText("Park", 130, 400, 20, DARKGREEN);

        // =======================
        // Roundabout
        // =======================
        DrawCircle(480, 270, 60, GRAY);
        DrawCircleLines(480, 270, 60, BLACK);

        DrawCircle(480, 270, 25, DARKGREEN);

        // Lane separator circles
        for (int i = 0; i < 360; i += 30) {
            float rad = i * DEG2RAD;
            DrawCircle(
                480 + cosf(rad) * 45,
                270 + sinf(rad) * 45,
                4,
                RAYWHITE
            );
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
