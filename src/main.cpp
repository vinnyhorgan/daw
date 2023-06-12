#include <raylib.h>
#include <rlImGui.h>

int main()
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 600, "Hello World");
    SetTargetFPS(60);

    rlImGuiSetup(true);

    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(BLACK);

        rlImGuiBegin();

        ImGui::DockSpaceOverViewport();

        ImGui::ShowDemoWindow();

        rlImGuiEnd();

        EndDrawing();
    }

    rlImGuiShutdown();

    CloseWindow();

    return 0;
}
