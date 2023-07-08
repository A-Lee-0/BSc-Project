//
// Created by Andrew Lee on 07/07/2023.
//
//#define WINBUILD 1

#include "Console.h"
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <termios.h>



#ifdef WINBUILD
#  include <windows.h>
#  ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#    define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#  endif
#  ifndef ENABLE_LVB_GRID_WORLDWIDE
#    define ENABLE_LVB_GRID_WORLDWIDE 0x0010
#  endif
#endif

inline unsigned int to_uint(char ch)
{
    return static_cast<unsigned int>(static_cast<unsigned char>(ch));
}

inline void hexPrint(std::string printString){
    char hexStr[5];
    for (char ch : printString) {
        sprintf(hexStr,"0x%2X ",ch);
        printCP(hexStr);
    }
    printCP("\n");
}

// Internal variables --------------------------------------------------------------------------------------------------

// Variables to store cursor and command history
std::list<std::string> commandHistory; // TODO: implement this using up/down arrow keys to navigate.
uint currentCursorRow;
uint currentCursorCol;

// Variables to store previous terminal state.
#ifdef WINBUILD
DWORD prevInputModeRaw;
DWORD prevOutputModeRaw;
#else
termios prevAttr;
#endif
termios prevAttrRaw;
static bool MODE_IS_RAW = false;


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

    void ClearLine(){
        printCP("\e[2K");
    }

    void ResetLine(){
        ClearLine();
        printCP("\e[999D");
    }

    bool SetSize(int rows, int columns){
        consoleRows = rows;
        consoleCols = columns;
        //printCP("\e[8;" + std::to_string(rows) + ";" + std::to_string(columns) + "t");
        return _SetSize();
    }

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
    }


    std::string GetInputANSI(){
        SaveCursorPos();
        std::string currentLine;
        std::string debugString = "";
        char readChar;
        bool lineEnded = false;
        uint strIndex = 0;
        while (!lineEnded){

            SetConsoleModeRaw();
            std::cin.get(readChar);
            RestoreConsoleModeRaw();

            //printCP(std::string(1,readChar));
            if (readChar == '\e'){
                std::string returnedString = ProcessANSICodes(&currentLine,&strIndex); // I think the string should always be blank, but I'm not 100% on what all the ANSI codes will do! E.g. maybe past inputs are in there.
            }
            else if (readChar == '\n'){     // endline
                lineEnded = true;
            }
            else if (readChar == '\x7f'){   // backspace
                if(strIndex > 0){
                    currentLine.erase(currentLine.begin()+strIndex-1);
                    strIndex -= 1;
                    currentCursorCol -= 1;
                }
            }
            else if ( (readChar>0x1f) && (readChar<0x7f)) { // Regular printable ascii character!
                currentLine.insert(strIndex,std::string(1,readChar));
                currentCursorCol +=1;
                strIndex+=1;
            }
            else{           // Unimplemented character received!
                char hexStr[5];
                sprintf(hexStr,"0x%2X ",readChar);
                debugString += "Unimplemented char: " + std::string(hexStr);
            }

            ResetLine();
            printCP(currentLine);
            printCP("    " + debugString);  // print debug string at the end of current line
            RefreshCursorPos();

            debugString = "";   // clear debug string
        }
        return currentLine;
    }

    std::string GetInput(){
        std::string rawInput;
        std::cin.clear();
        rawInput = GetInputANSI();

        if(Console::CLEAR_ON_INPUT) { Console::Clear(); }

        if (Console::DEBUG_PRINT_INPUT_HEX) {
            // output raw input hex for debug
            // default here is ANSI, not unicode.
            hexPrint(rawInput);
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

        // Temporarily put terminal in 'raw' mode, reading every character as they come.
        SetConsoleModeRaw();

        char readChar;
        std::cin.get(readChar);
        if (readChar != '\e'){
            throw std::runtime_error("Incorrect return character during Console::GetCursorPos");
        }
        int maxChars = 15;  // the minimum chars for this would be 6. To hit would need >10000x10000 char-sized console!
        for(int i=0; i<maxChars; i++) {
            std::cin.get(readChar);
            returnString << readChar;
            if (readChar == 'R'){
                i=16;
            }
        }

        // Restore terminal to previous state.
        RestoreConsoleModeRaw();

        rawInput = returnString.str();
        ulong start = rawInput.find("[")+1;
        ulong mid = rawInput.find(";");
        ulong end = rawInput.find("R");
        std::string rowStr = rawInput.substr(start,mid-start);
        std::string colStr = rawInput.substr(mid+1,end-mid-1);
        //printCP("Returned row: " + rowStr + "  col: " + colStr + "\n" );

        std::pair<int,int> returnValue = {std::stoi(rowStr),std::stoi(colStr)};
        return returnValue;
    }
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
        HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
        DWORD inputMode;
        GetConsoleMode(hStdin, &inputMode);
        inputMode &= ~ENABLE_PROCESSED_INPUT;
        inputMode &= ~ENABLE_ECHO_INPUT;
        SetConsoleMode(hStdin,inputMode);
      #else

      #endif
        CONSOLE_INIT_RUN = true;
    }


    int SetConsoleModeRaw(){
        if(MODE_IS_RAW){
            std::string errorMsg = "SetConsoleModeRaw in Console.cpp was called, when the console was already in \'raw\' mode.";
            printCP(errorMsg + "\n");
            throw std::invalid_argument(errorMsg);
        }
        int returnValue = 0;
      #ifdef WINBUILD
        //save windows console values.
        HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
        GetConsoleMode(hStdin, &prevInputModeRaw);
        HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
        GetConsoleMode(hStdout, &prevOutputModeRaw);
      #endif
        // based off https://stackoverflow.com/questions/6698161/getting-raw-input-from-console-using-c-or-c
        termios attr;
        tcgetattr(0,&attr);     // Read current attributes
        prevAttrRaw = attr;     // Save current attributes

        MODE_IS_RAW = true;     // Set this now, as all of the initial state has now been saved. Even if something goes wrong, and the function fails to actually put the console into 'raw' mode, the RestoreConsoleModeRaw() function will now have populated variables.

        attr.c_lflag &= ~ICANON;    //clear Canonical Mode flag
        attr.c_lflag &= ~ECHO;      //clear Echo flag

        returnValue = tcsetattr(0, TCSANOW, &attr);   // Immediately set the new attributes to the terminal
        if (returnValue != 0){
            int errorCode = errno;
            printCP("Error during SetConsoleModeBlocking. Attr.c_lflag: " + std::to_string(attr.c_lflag) + ":  Error Code: " + std::to_string(errorCode) + "\n");
        }
        return returnValue;
    }

    int RestoreConsoleModeRaw(){
        if(!MODE_IS_RAW){
            std::string errorMsg = "RestoreConsoleModeRaw in Console.cpp was called, when the console was not in \'raw\' mode.";
            printCP(errorMsg + "\n");
            throw std::invalid_argument(errorMsg);
        }
        int returnValue = 0;

        returnValue = tcsetattr(0, TCSANOW, &prevAttrRaw);
        if (returnValue != 0){
            int errorCode = errno;
            printCP("Error during RestoreConsoleModeRaw. Attr.c_lflag: " + std::to_string(prevAttrRaw.c_lflag) + ":  Error Code: " + std::to_string(errorCode) + "\n");
        }
      #ifdef WINBUILD
        HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
        int errVal = SetConsoleMode(hStdin,prevInputModeRaw);
        if (errVal == 0){
            DWORD errorCode = GetLastError();
            std::cout << "Error during RestoreConsoleModeRaw. Input mode: " << prevInputModeRaw << ":  Error Code: " << errorCode << std::endl;
        }
        HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
        errVal = SetConsoleMode(hStdout,prevOutputModeRaw);
        if (errVal == 0){
            DWORD errorCode = GetLastError();
            std::cout << "Error during RestoreConsoleModeRaw. Output mode: " << prevOutputModeRaw << ":  Error Code: " << errorCode << std::endl;
        }
      #endif
        MODE_IS_RAW = false;
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
    }

    std::pair<uint,uint> _SetCursorPos(int row, int column){
        _SetCursorPosNoRet(row,column); // reuse NoReturn version of func, to avoid reduce duplication
        return Console::GetCursorPos();
    }

    void _SetCursorPosNoRet(int row, int column){
        printCP("\e[" + std::to_string(row) + ";" + std::to_string(column) + "H");
        return;
    }

    void SaveCursorPos(){
        std::pair<uint,uint> currentPos = Console::GetCursorPos();
        currentCursorRow = currentPos.first;
        currentCursorCol = currentPos.second;
    }

    void RefreshCursorPos(){
        _SetCursorPosNoRet(currentCursorRow, currentCursorCol);
    }

    std::string ProcessANSICodes(std::string* currentLine, uint* strIndex){
        //printCP("ANSI Code!");

        std::stringstream codeType;
        std::stringstream code;
        char lastChar = '\e';
        char newChar;
        codeType << '\e';

        bool buildingCode = true;
        functionPtr ansiFunc;
        while (buildingCode){
            SetConsoleModeRaw();
            std::cin.get(newChar);
            RestoreConsoleModeRaw();

            code << newChar;
            //hexPrint(code.str());

            // if newChar is escape code, then something has gone wrong! Print the code hex to terminal and restart processing of ANSI code.
            if(newChar == '\e'){
                printCP("Unexpected <ESC> token when parsing an ANSI escape code. The recorded code was: 0x1b ");
                hexPrint(code.str());
                return ProcessANSICodes(currentLine,strIndex);
            }

            // if newChar is new line, then something has gone wrong! Print the code hex to terminal and return.
            if(newChar == '\n'){
                printCP("Unexpected new line token when parsing an ANSI escape code. The recorded code was: 0x1b ");
                hexPrint(code.str());
                return "";
            }

            // if newChar is number, might need to add '\n' to codeType.
            if (isdigit(newChar)){
                //only add '\n' if previous char was NOT digit. Otherwise it's just a bigger number.
                if (!isdigit(lastChar)){
                    codeType << '\n';
                }
            }
            // otherwise add the char to the codeType.
            else {
                codeType << newChar;
            }

            //if code is now a recognised ANSI code, call it!
            // check if instruction string is valid
            try {
                ansiFunc = ANSICodeFunctions.at(codeType.str());
                buildingCode = false;
            }
            catch (const std::out_of_range& oor) {
                // Catch error from non-existant ANSI code.
            }
        }

        return (*ansiFunc)(codeType.str(),currentLine,strIndex);
    }



    // ANSI Code Functions ---------------------------------------------------------------------------------------------
    std::string _ProcessUpArrow(std::string ansiCode, std::string* currentLine, uint* strIndex){
        // \e[ n A
        printCP("UpArrow!");
        //fflush(stdout);
        //printf("UpArrow!");
        //TODO: Implement up/down arrows storing command history.
        return "";
    }
    std::string _ProcessDownArrow(std::string ansiCode, std::string* currentLine, uint* strIndex){
        // \e[ n B
        printCP("DownArrow!");
        //fflush(stdout);
        //printf("DownArrow!");
        return "";
    }
    std::string _ProcessRightArrow(std::string ansiCode, std::string* currentLine, uint* strIndex){
        // \e[ n C
        //printCP("RightArrow!");
        //TODO: Implement the 'n' parameter in the left/right arrow codes, to move n spaces rather than just 1.
        if ( (*strIndex) < (*currentLine).length() ){
            *strIndex += 1;
            currentCursorCol += 1;
            RefreshCursorPos();
        }
        return "";
    }
    std::string _ProcessLeftArrow(std::string ansiCode, std::string* currentLine, uint* strIndex){
        // \e[ n D
        //printCP("LeftArrow!");
        if ( (*strIndex)>0 ){
            *strIndex -= 1;
            currentCursorCol -= 1;
            RefreshCursorPos();
        }
        return "";
    }

    // Map from ANSI code string, to function to call to process them.
    std::map<std::string,functionPtr> ANSICodeFunctions {{"\e[A",&_ProcessUpArrow},
                                                                {"\e[B",&_ProcessDownArrow},
                                                                {"\e[C",&_ProcessRightArrow},
                                                                {"\e[D",&_ProcessLeftArrow},
                                                                {"\e[\nA",&_ProcessUpArrow},
                                                                {"\e[\nB",&_ProcessDownArrow},
                                                                {"\e[\nC",&_ProcessRightArrow},
                                                                {"\e[\nD",&_ProcessLeftArrow}};
}

