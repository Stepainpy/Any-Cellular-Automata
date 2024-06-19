#include <windows.h>
#include <iostream>
#include <thread>
#include <chrono>

#include "str_prefixs.hpp"
#include "world.hpp"
#include "lexer.hpp"

using namespace std::chrono_literals;

int main(int argc, char** argv) {
    srand(time(NULL));
    if (argc != 2) {
        std::cerr << ErrorPrefix << "Must be specificate one argument (world file path)\n";
        return 1;
    }

    Lexer lexer(argv[1]);
    auto l = lexer.parse();
    World world = buildWorld(l);

    // Game loop

    std::cout
    << "\e[H\e[2J"  // clear screen
    << "\e[?25l";   // set cursor invisible

    auto begin = std::chrono::high_resolution_clock::now();
    auto end   = std::chrono::high_resolution_clock::now();

    bool isRun = true;
    while (isRun) {
        begin = std::chrono::high_resolution_clock::now();
        world.display();
        world.update();
        end   = std::chrono::high_resolution_clock::now();
        
        // exit from app
        isRun = !(GetAsyncKeyState('Q') & 0x8000);

        auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
        if (dur.count() > 20)
            continue;
        std::this_thread::sleep_for(20ms - dur);  // 50 FPS
    }

    std::cout
    << "\e[?25h";  // set cursor visible
}