#include <iostream>
#include <conio.h>
#include <thread>
#include <chrono>
#include <future>

#include <raylib/raylib.h>
#include "str_prefixs.hpp"
#include "conworld.hpp"
#include "guiworld.hpp"
#include "lexer.hpp"

const int ESC_ASCII    = 27;
const int GETCH_KEY_F3 = 61;

using namespace std::chrono_literals;

void useConsole(ConsoleWorld& world) {
    std::cout
    << "\e[H\e[2J"  // clear screen
    << "\e[?25l";   // set cursor invisible

    auto begin = std::chrono::high_resolution_clock::now();
    auto end   = std::chrono::high_resolution_clock::now();
    std::chrono::milliseconds delay = 20ms, fastDelay = 2ms;
    size_t iterations = 0;
    bool viewIter = false;
    bool pauseSim = false;

    auto keyHandler = std::async(std::launch::async, [&]() {
        while (true) {
            int c = getch();
            if (c == ESC_ASCII) break;                       // Esc   = exit from app
            else if (c == ' ') pauseSim = !pauseSim;         // Space = pause simulation
            else if (c == 'b') std::swap(delay, fastDelay);  // b     = toggle simulation speed
            else if (c == 'i') viewIter = !viewIter;         // i     = view count of iterations
            else if (c == 'n' && pauseSim) {                 // n     = next step (if now is pause)
                world.update();
                iterations++;
            }
        }
    });

    while (keyHandler.wait_for(0s) != std::future_status::ready) {
        begin = std::chrono::high_resolution_clock::now();
        world.display();
        if (viewIter)
            std::cout << "\r(Esc - exit) Iter count: " << iterations;
        if (!pauseSim) {
            world.update();
            iterations++;
        }
        end = std::chrono::high_resolution_clock::now();

        auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
        if (dur.count() < delay.count())
            std::this_thread::sleep_for(delay - dur);  // 50 or 500 FPS
    }

    std::cout
    << "\e[?25h";  // set cursor visible
}

void useGui(GuiWorld& world) {
    auto [width, height] = world.getWindowSize();
    size_t iterations = 0;
    bool viewIter = false;
    bool pauseSim = false;
    int fps = 50, fastFps = 500;

    InitWindow(width, height, "ACA");
    SetTargetFPS(fps);

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_B)) {
            std::swap(fps, fastFps);
            SetTargetFPS(fps);
        }
        if (IsKeyPressed(KEY_N) && pauseSim) {
            world.update();
            iterations++;
        }
        if (IsKeyPressed(KEY_SPACE))
            pauseSim = !pauseSim;
        if (IsKeyPressed(KEY_I))
            viewIter = !viewIter;

        BeginDrawing();
        ClearBackground(BLACK);

        world.display();
        if (viewIter)
            DrawText(TextFormat("Iter count: %u", iterations), 3, 3, 20, GREEN);

        EndDrawing();

        if (!pauseSim) {
            world.update();
            iterations++;
        }
    }

    CloseWindow();
}

int main(int argc, char** argv) {
    srand(time(NULL));
    if (!(argc == 2 || argc == 3)) {
        std::cerr << ErrorPrefix << "Must be specificate one (world file path) or two (world file and gui setting json) argument\n";
        std::cerr << "[Usage]: ./aca <world file> [<guis setting json>]\n";
        return 1;
    }

    if (std::string(argv[1]) == "help") {
        std::cout << "[Usage]: ./aca <world file> [<guis setting json>]\n";
        return 0;
    }

    Lexer lexer(argv[1]);
    auto l = lexer.parse();

    if (argc == 3) {
        GuiWorld world = buildGuiWorld(l, argv[2]);
        useGui(world);
    } else {
        ConsoleWorld world = buildConsoleWorld(l);
        useConsole(world);
    }
}