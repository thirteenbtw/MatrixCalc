#include <cstring>
#include <iostream>
#include "ConsoleApp.h"
#include "Console.h"
#include "GuiApp.h"

// запуск без параметров - окно, с параметром --console - меню в терминале
int main(int argc, char** argv) {
    for (int i = 1; i < argc; i++) {
        if (std::strcmp(argv[i], "--console") == 0 or std::strcmp(argv[i], "-c") == 0) {
            runConsole();
            return 0;
        }
        if (std::strcmp(argv[i], "--help") == 0 or std::strcmp(argv[i], "-h") == 0) {
            setupConsole();
            std::cout << "MatrixCalculator\n"
                      << "  (без параметров)  графический интерфейс\n"
                      << "  --console, -c     консольный режим\n";
            return 0;
        }
    }
    return runGui();
}
