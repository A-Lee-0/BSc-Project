//
// Created by Andrew Lee on 07/07/2023.
//
#ifndef SGCB_CONSOLE_H
#define SGCB_CONSOLE_H

#include <string>
#include <sstream>
#include <utility>

// Public functions
int printCP(std::string);
namespace Console {
    extern bool DEBUG_PRINT_INPUT_HEX;
    extern bool CLEAR_ON_INPUT;
    extern void Clear();
    extern bool SetSize(int rows, int columns);
    extern std::pair<uint,uint> GetSize();
    extern std::string GetInput();
    extern std::pair<uint,uint> GetCursorPos();
}

// Internal functions
namespace {
    static void InitialiseConsole();
    static int SetConsoleModeBlocking(bool blocking);
    static int RestoreConsoleMode();
    static bool _SetSize();
    static std::pair<uint, uint> _SetCursorPos(int row, int column);
    static char rawReadCharCP();
}

// Internal variables
static bool CONSOLE_INIT_RUN = false;
static int consoleCols = 100;
static int consoleRows = 20;


#endif //SGCB_CONSOLE_H