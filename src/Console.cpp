//
// Created by Andrew Lee on 07/07/2023.
//
#include "Console.h"
#include <string>
#include <iostream>
#include <boost/algorithm/string.hpp>


//#include <windows.h>

#ifdef WINBUILD
#  include <windows.h>
#  ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#    define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#  endif
#  ifndef ENABLE_LVB_GRID_WORLDWIDE
#    define ENABLE_LVB_GRID_WORLDWIDE 0x0010
#  endif
#else
#  include <termios.h>
#endif

inline unsigned int to_uint(char ch)
{
    return static_cast<unsigned int>(static_cast<unsigned char>(ch));
}


// Public functions ----------------------------------------------------------------------------------------------------

/**Attempts to print to the console in a way that will utilise any ANSI flags.
 * This /could/ be done with runtime logic, maybe, but is simpler to sort at compile-time.
 * Prints on Windows Console if the "WINBUILD" (i.e. by running CMake with -DWINBUILD=1 ), using WriteConsole.
 * Otherwise prints using printf.
 *
 * @param printString - The string of characters to print to the console. May include ANSI codes.
 * @return - Returns an int value containing the number of characters printed.
 */
int printCP(std::string printString) {
    //TODO modify this to add a linebreak if the next word is too long to fit in the current line.
    if(!CONSOLE_INIT_RUN){
        InitialiseConsole();
    }
    int returnValue;
    const char* cString = printString.c_str();
#ifdef WINBUILD
    ulong strLen = printString.size();
    bool returnBool;
    DWORD charsPrinted;
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    returnBool = WriteConsole(hStdout,cString,strLen,&charsPrinted,NULL);
    returnValue = returnBool ? -1 : charsPrinted;
#else
    returnValue = printf(cString);
#endif
    return returnValue;
}



namespace Console{
    bool DEBUG_PRINT_INPUT_HEX = false;
    bool CLEAR_ON_INPUT = false;

    void Clear(){
        printCP("\e[2J");
        _SetCursorPos(1,1);
    }

    bool SetSize(int rows, int columns){
        consoleRows = rows;
        consoleCols = columns;
        //printCP("\e[8;" + std::to_string(rows) + ";" + std::to_string(columns) + "t");
        return _SetSize();
    };

    /**Returns the size of the current terminal in characters.
     *
     * @return - pair< numRows, numColumns >
     */
    std::pair<uint,uint> GetSize(){
        // Apparently the most cross-platform way to do this is to set the cursor position to a huge x,y coord, and then
        // see where it actually end up.
        std::pair<uint,uint>  currentCursorPos = Console::GetCursorPos();
        _SetCursorPos(999,999);
        std::pair<uint,uint>  maxCursorPos = Console::GetCursorPos();
        _SetCursorPos(currentCursorPos.first,currentCursorPos.second);
        return maxCursorPos;
    };


    std::string GetInput(){
        //Temporary solution. Want to redo this to handle ANSI codes (e.g. arrow keys) etc.
        std::string rawInput;
        std::getline(std::cin >> std::ws, rawInput);
        std::cin.clear();

        if(Console::CLEAR_ON_INPUT) { Console::Clear(); }

        if (Console::DEBUG_PRINT_INPUT_HEX) {
            // output raw input hex for debug
            // default here is ANSI, not unicode.
            std::cout << std::hex;
            for (char ch : rawInput) {
                std::cout << "0x" << to_uint(ch) << ' ';
            }
            std::cout << std::endl;
        }
        boost::trim(rawInput);

        return rawInput;
    }

    /**Returns the position of the cursor (in characters coordinates)
     *
     * @return pair< row, column >
     */
    std::pair<uint,uint> GetCursorPos(){
        std::cin.clear();
        printCP("\e[6n");

        // Return should be formatted as \e[<rows>;<columns>R
        std::string rawInput;

        std::stringstream returnString;

    #ifdef WINBUILD
        // Temporarily set the stdin ENABLE_LINE_INPUT flag to false using SetConsoleMode.
        SetConsoleModeBlocking(false);
        HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
        char readChar;
        uint charsRead;
        ReadConsole(hStdin,&readChar,1,&charsRead,NULL);
        if (readChar != '\e'){
            throw std::runtime_error("Incorrect return character during Console::GetCursorPos");
        }
        int maxChars = 15;  // the minimum chars for this would be 6. To hit would need >10000x10000 char-sized console!
        for(int i=0; i<maxChars; i++) {
            ReadConsole(hStdin,&readChar,1,&charsRead,NULL);;
            returnString << readChar;
            if (readChar == 'R'){
                i=16;
            }
        }
        // Reset the stdin ENABLE_LINE_INPUT flag.
        SetConsoleModeBlocking(true);
    #else

        // based off https://stackoverflow.com/questions/6698161/getting-raw-input-from-console-using-c-or-c
        termios oldAttr;
        termios newAttr;
        tcgetattr(0,&oldAttr);
        newAttr = oldAttr;
        newAttr.c_lflag &= ~ICANON; //clear ICANON flag
        tcsetattr(0, TCSANOW, &newAttr);

        char readChar;
        readChar = fgetc(stdin);
        if (readChar != '\e'){
            throw std::runtime_error("Incorrect return character during Console::GetCursorPos");
        }
        bool reading = true;
        while (reading) {
            readChar = fgetc(stdin);
            returnString << readChar;
            if (readChar == 'R'){
                reading = false;
            }
        }

        // Reset back to original attributes.
        tcsetattr(0, TCSANOW, &oldAttr);

    #endif
        rawInput = returnString.str();
        ulong start = rawInput.find("[")+1;
        ulong mid = rawInput.find(";");
        ulong end = rawInput.find("R");
        std::string rowStr = rawInput.substr(start,mid-start);
        std::string colStr = rawInput.substr(mid+1,end-mid-1);
        //printCP("Returned row: " + rowStr + "  col: " + colStr + "\n" );

        std::pair<int,int> returnValue = {std::stoi(rowStr),std::stoi(colStr)};
        return returnValue;
    };
}


// Internal Functions --------------------------------------------------------------------------------------------------
namespace {
    void InitialiseConsole() {
        //system("chcp.com 65001");
        //system("locale charmap");
        //system("setcons.com UTF-8");
        //system("set LC_CTYPE=C.UTF-8");

    #ifdef WINBUILD
        HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD outputMode;
        GetConsoleMode(hStdout, &outputMode);
        outputMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        outputMode |= ENABLE_PROCESSED_OUTPUT;
        outputMode |= ENABLE_LVB_GRID_WORLDWIDE;
        SetConsoleMode(hStdout,outputMode);
    #else

    #endif

        CONSOLE_INIT_RUN = true;
    };

    int SetConsoleModeBlocking(bool blocking){
        int returnValue = 0;
    #ifdef WINBUILD
        HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
        DWORD inputMode;
        GetConsoleMode(hStdin, &inputMode);
        //printCP("InputMode was " + std::to_string(inputMode) + "\n");
        if (blocking){
            inputMode |= ENABLE_LINE_INPUT;
            inputMode |= ENABLE_ECHO_INPUT;
        }
        else{
            inputMode &= ~ENABLE_LINE_INPUT;
            inputMode &= ~ENABLE_ECHO_INPUT;
        }
        returnValue = SetConsoleMode(hStdin,inputMode);
        //printCP("Tried to set InputMode to " + std::to_string(inputMode) + ". returning " + std::to_string(returnValue) + "\n");
        if (returnValue == 0){
            DWORD errorCode = GetLastError();
            printCP("Error code: " + std::to_string(errorCode) + "\n");
        }
    #else

    #endif

        return returnValue;
    }

    /**Function to try and set the size of the terminal to the values in consoleRows and consoleCols.
     * This appears to work for both Mintty and Windows Console, but Windows Terminal has not yet implemented this.
     * See https://github.com/microsoft/terminal/issues/5094 for details.
     *
     * @return - returns a bool indicating whether the change was successful.
     */
    bool _SetSize(){
        printCP("\e[8;" + std::to_string(consoleRows) + ";" + std::to_string(consoleCols) + "t");
        bool returnValue = false;
        return returnValue;
    };

    std::pair<uint,uint> _SetCursorPos(int row, int column){
        printCP("\e[" + std::to_string(row) + ";" + std::to_string(column) + "H");
        return Console::GetCursorPos();
    };

}

