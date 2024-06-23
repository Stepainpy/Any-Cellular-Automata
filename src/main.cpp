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

#define ESC_ASCII 27

using namespace std::chrono_literals;

void useConsole(ConsoleWorld& world) {
    std::cout
    << "\e[H\e[2J"  // clear screen
    << "\e[?25l";   // set cursor invisible

    auto begin = std::chrono::high_resolution_clock::now();
    auto end   = std::chrono::high_resolution_clock::now();

    auto tracker = std::async(std::launch::async, []() {
        while (true)
            if (getch() == ESC_ASCII)
                break;
    });
    bool run = true;
    while (run) {
        begin = std::chrono::high_resolution_clock::now();
        world.display();
        world.update();
        end   = std::chrono::high_resolution_clock::now();
        
        // exit from app
        run = !(tracker.wait_for(0s) == std::future_status::ready);

        auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
        if (dur.count() > 20)
            continue;
        std::this_thread::sleep_for(20ms - dur);  // 50 FPS
    }

    std::cout
    << "\e[?25h";  // set cursor visible
}

void useGui(GuiWorld& world) {
    auto [width, height] = world.getWindowSize();
    InitWindow(width, height, "ACA");
    SetTargetFPS(50);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        world.display();
        EndDrawing();

        world.update();
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