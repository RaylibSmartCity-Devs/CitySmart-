#include <iostream>
#include "raylib.h"

int main(int, char**){
    InitWindow(800, 600, "citysmart window");
    SetTargetFPS(60);
    while(!WindowShouldClose()){
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("hello world", 10, 10, 20, BLACK);
        DrawFPS(10, 40);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
