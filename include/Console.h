//
// Created by Andrew Lee on 07/07/2023.
//
#ifndef SGCB_CONSOLE_H
#define SGCB_CONSOLE_H

#include <string>
#include <sstream>
#include <map>
#include <utility>

typedef std::string (*functionPtr)(std::string, std::string*, uint*); // function pointer type

// Public functions ----------------------------------------------------------------------------------------------------
int printCP(std::string);
namespace Console {
    extern bool DEBUG_PRINT_INPUT_HEX;
    extern bool CLEAR_ON_INPUT;
    extern void Clear();
    extern void ClearLine();
    extern void ResetLine();
    extern bool SetSize(int rows, int columns);
    extern std::pair<uint,uint> GetSize();
    extern std::string GetInputANSI();
    extern std::string GetInput();
    extern std::pair<uint,uint> GetCursorPos();
}

// Internal functions --------------------------------------------------------------------------------------------------
namespace {
    static void InitialiseConsole();
    static int SetConsoleModeRaw();
    static int RestoreConsoleModeRaw();
    static bool _SetSize();
    static std::pair<uint, uint> _SetCursorPos(int row, int column);
    static void _SetCursorPosNoRet(int row, int column);
    static void SaveCursorPos();
    static void RefreshCursorPos();
    static std::string ProcessANSICodes(std::string* currentLine,uint* strIndex);

    static std::string _ProcessUpArrow(std::string ansiCode, std::string* currentLine,uint* strIndex);
    static std::string _ProcessDownArrow(std::string ansiCode, std::string* currentLine,uint* strIndex);
    static std::string _ProcessLeftArrow(std::string ansiCode, std::string* currentLine,uint* strIndex);
    static std::string _ProcessRightArrow(std::string ansiCode, std::string* currentLine,uint* strIndex);
    //TODO: Add Pageup/down, ctrl arrows, home, end, del
    // and ctrl/shift variants of the above.
    extern std::map<std::string,functionPtr> ANSICodeFunctions;
}

// Internal variables --------------------------------------------------------------------------------------------------
static bool CONSOLE_INIT_RUN = false;
static int consoleCols = 100;
static int consoleRows = 20;






#endif //SGCB_CONSOLE_H