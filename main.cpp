#include <iostream>
#include <vector>
#include <map>
#include <random>
#include <ctime>

#include "Console.h"
#include "Input.h"
#include "SGCB_Config.h"
//#include "Experiment.h"



std::time_t startTime_main;
boost::random::mt19937 gen(static_cast<unsigned int>(std::time(&startTime_main)));

extern void MinimumRawModeTest();
extern void MinimumRawModeTermiosTest();
extern void MinimumRawModeTest2();

int main() {
    // Try set initial console dimensions. Works on Mintty and WindowsConsole, but not supported in Windows Terminal.
    Console::SetSize(40,100);

    //MinimumRawModeTest();
    //MinimumRawModeTermiosTest();
    //MinimumRawModeTest2();
    // Conclusion of these tests - the only way I can find to make it work, it to just use termios on windows, and wrap
    // the code in blocks to store and reset the windows console modes.

    std::stringstream printString;
    printString << "From Simple Games to Complex Behaviour\n"
                << "Written by Samuel Kirby and Andrew Lee for BSc Project supervised by Dr Dave Clements\n"
                << "Version: " << VERSION_MAJOR << "." << VERSION_MINOR << std::endl;
    printCP(printString.str());

    //Console::DEBUG_PRINT_INPUT_HEX = true;


    time_t t = time(0);   // get time now
    struct tm * now = localtime( & t );
    char datetime[20];
    strftime(datetime,20,"%Y-%m-%d %H:%M",now);
    printCP("Time: " + std::string(datetime) + "\n");

    //init RNGs
    Strategy::SetRNG(gen);
    Update::SetRNG(gen);

    Experiment * experiment = new RepeatExperiment();
    //Experiment * experiment = new IncrementGamesPerRound();
    experiment->SetParameter("SaveBitmaps","1");
    experiment->SetParameter("RepeatExperiments","10");
    //experiment->SetParameter("MaxGPR","3");

    Input::TEST_ANSI = true;

    bool inputting = true;
    while (inputting){
        std::cout << "Enter 'run' to start the experiment." << std::endl;
        bool result;
        result = Input::ProcessInput(experiment);

        if (result){
            inputting = false;
        }

        //TODO: allow displaying 'special' param permitted values, i.e. for InitialStrategiesMethod and UpdateType
        //TODO: allow setting StrategyGenerator parameters, e.g. centreStrat, surroundingStrat
    }


/*
//Run Single Case

    UpdateList updateType = UpdateList::L_MostWins;
    int GPR = 1;
    int rounds = 50;
    std::string updateString = UpdateListStrings[updateType];
    std::string filepath = "DotTest_" + updateString + "\\" + std::to_string(GPR) + "gpr";
    Network network = Network(10, 10, random, Default, filepath, updateType, 5);
    network.connectNearestNeighbours();
    playNRounds(network, rounds, GPR, true, true);
*/

    //centreStrat = StrategyList::AlwaysDefect;
    //surroundingsStrat = StrategyList::AlwaysCooperate;
    //UpdateList updateType = UpdateList::SL_Interrogate;
    //std::string updateString = UpdateListStrings[updateType];
    /*for (int i = 0; i != 200; i++){
        std::cout << updateString << i << std::endl;

        std::string filepath = "IteratedDataSet3\\" + updateString + "\\15gpr\\iter" + std::to_string(i);
        Network network = Network(100,100,random, updateType, filepath);
        network.connectNearestNeighbours();
        playNRounds(network, 50, 15, false, true);
    }

    updateType = UpdateList::L_Threshold;
    updateString = UpdateListStrings[updateType];
    for (int i = 0; i != 200; i++){
        std::cout << updateString << i << std::endl;

        std::string filepath = "IteratedDataSet3\\" + updateString + "\\15gpr\\iter" + std::to_string(i);
        Network network = Network(100,100,random, updateType, filepath);
        network.connectNearestNeighbours();
        playNRounds(network, 50, 15, false, true);
    }*/


    /*
    updateType = UpdateList::SL_ThresholdScoreTB;
    updateString = UpdateListStrings[updateType];
    for (int i = 0; i != 200; i++){
        std::cout << updateString << i << std::endl;

        std::string filepath = "IteratedDataSet3\\" + updateString + "\\15gpr\\iter" + std::to_string(i);
        Network network = Network(100,100,Strategy::random,updateType, filepath);
        network.connectNearestNeighbours();
        playNRounds(network, 50, 15, false, true);
    }*/


    //iteratedRunAllUpdatesForAllGPRs(15,50,150,random);
    //runAllUpdatesForAllGPRs(15,50, random);
    //runAllUpdatesForAllGPRs_BlockNetwork(15,50,random, 5);
    //runAllUpdatesForAllGPRs_CentreNetwork(15,50,centre, 0.5);
    return 0;
}


/*
void runAllUpdatesForAllGPRs(int maxGPR,int rounds, StrategyMethod strategyFunc){
    unsigned long noUpdateTypes = (int) UpdateList::G_GDP;
//    boost::progress_display showUpdateProg(noUpdateTypes);
//    boost::progress_display showGPRProg((unsigned long) maxGPR);
    std::cout << maxGPR << ", " << rounds << std::endl;

    for (int j = (int)UpdateList::L_Threshold; j != (int) UpdateList::G_GDP+1; j++) {
        UpdateList updateType = static_cast<UpdateList>(j);
        std::string updateString = UpdateListStrings[updateType];
        std::cout << updateString << std::endl;

        for (int i = 1; i != maxGPR; i++) {
            std::cout << i << std::endl;
            std::string filepath = "NewDataSet\\gprTest_GDPSubPop0-1" + updateString + "\\random_" + std::to_string(i) + "gpr";
            //Network network = Network(100, 100, random, updateType, filepath);
            Network network = Network(100, 100, strategyFunc, Update::DefaultWithPureGDP, filepath, updateType);
            network.connectNearestNeighbours();
            playNRounds(network, rounds, i, true, true);
//            ++showGPRProg;
        }
//        ++showUpdateProg;
    }
}

 */


/*
void iteratedRunAllUpdatesForAllGPRs(int maxGPR,int rounds, int iterations, Strategy* strategyFunc(std::pair<int, int>, std::pair<int, int>)){
    unsigned long noUpdateTypes = (int) UpdateList::G_GDP;
    std::cout << maxGPR << ", " << rounds << std::endl;

    //for (int j = (int) UpdateList::L_Threshold; j != (int) UpdateList::G_GDP + 1; j++) {
    for (int j = (int) UpdateList::L_Threshold+1; j != (int) UpdateList::G_GDP + 1; j++) {
    //for (int j = (int) UpdateList::L_Threshold; j != (int) UpdateList::L_Threshold + 1; j++) {
        UpdateList updateType = static_cast<UpdateList>(j);
        std::string updateString = UpdateListStrings[updateType];
        std::cout << updateString << std::endl;

        for (int i = 15; i != maxGPR + 1; i++) {
            for (int k = 0; k!= iterations; k++) {
                std::cout << i << std::endl;
                std::string filepath = "IteratedDataSet\\" + updateString + "\\" + std::to_string(i) + "gpr\\iter" + std::to_string(k+50);
                Network network = Network(100, 100, strategyFunc, Update::DefaultWithPureGDP, filepath, updateType);
                network.connectNearestNeighbours();
                playNRounds(network, rounds, i, false, true);
            }
        }
    }
}
*/


/*
void runAllUpdatesForAllGPRs_BlockNetwork(int maxGPR,int rounds, Strategy* strategyFunc(std::pair<int, int>, std::pair<int, int>),int blockSize){
    unsigned long noUpdateTypes = (int) UpdateList::G_GDP;
//    boost::progress_display showUpdateProg(noUpdateTypes);
//    boost::progress_display showGPRProg((unsigned long) maxGPR);
    std::cout << maxGPR << ", " << rounds << std::endl;

    for (int j = (int)UpdateList::L_Threshold; j != (int) UpdateList::G_GDP+1; j++) {
        UpdateList updateType = static_cast<UpdateList>(j);
        std::string updateString = UpdateListStrings[updateType];
        std::cout << updateString << std::endl;

        for (int i = 1; i != maxGPR; i++) {
            std::cout << i << std::endl;
            std::string filepath = "BlockTest_noSubPops\\" + updateString + "\\random_" + std::to_string(i) + "gpr";
            //Network network = Network(100, 100, random, updateType, filepath);
            Network network = Network(100, 100, strategyFunc, Update::Default, filepath, updateType, blockSize);
            network.connectNearestNeighbours();
            playNRounds(network, rounds, i, true, true);
//            ++showGPRProg;
        }
//        ++showUpdateProg;
    }
}
*/

/*

//StrategyList centreStrat;
//StrategyList surroundingsStrat;
void runAllUpdatesForAllGPRs_CentreNetwork(int maxGPR,int rounds, Strategy* strategyFunc(std::pair<int, int>, std::pair<int, int>),double centreradius){
    std::cout << maxGPR << ", " << rounds << std::endl;
    double radius = centreradius;
    //for (int l = (int)StrategyList::AlwaysDefect; l != (int) StrategyList::SuspiciousTitForTat+1; l++) {
    for (int l = (int)StrategyList::TitForTat; l != (int) StrategyList::TitForTat+1; l++) {
        StrategyList surroundingsStrat = static_cast<StrategyList>(l);
        std::string surroundsString = StrategyListStrings[surroundingsStrat];
        std::cout << "surrounds:" << surroundsString << std::endl;
        //for (int k = (int)StrategyList::AlwaysDefect; k != (int) StrategyList::SuspiciousTitForTat+1; k++) {
        for (int k = (int)StrategyList::SuspiciousTitForTat; k != (int) StrategyList::SuspiciousTitForTat+1; k++) {
            StrategyList centreStrat = static_cast<StrategyList>(k);
            std::string centreString = StrategyListStrings[centreStrat];
            std::cout << "centre:" << centreString << std::endl;
            if (k!= l){
                for (int j = (int)UpdateList::L_Threshold; j != (int) UpdateList::G_GDP+1; j++) {
                    UpdateList updateType = static_cast<UpdateList>(j);
                    std::string updateString = UpdateListStrings[updateType];
                    std::cout << updateString << std::endl;
                    for (int i = 1; i != maxGPR+1; i++) {
                        std::cout << i << std::endl;
                        std::string filepath =
                                "centreDot_r0-5\\surr_" + surroundsString + "\\cent_" + centreString + "\\" + updateString +
                                "\\" + std::to_string(i) + "gpr";
                        Network network = Network(15, 15, strategyFunc, Update::Default, filepath, updateType);
                        network.connectNearestNeighbours();
                        playNRounds(network, rounds, i, true, true);
                    }
                }
            }
        }
    }
}

*/




inline void hexPrint(std::string printString){
    char hexStr[5];
    for (char ch : printString) {
        sprintf(hexStr,"0x%2X ",ch);
        printCP(hexStr);
    }
    printCP("\n");
}

#include <windows.h>
void MinimumRawModeTest(){
    std::cout << "Entering raw mode test." << std::endl;
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD inputMode;
    GetConsoleMode(hStdin, &inputMode);
    DWORD origInputMode = inputMode;

    inputMode &= ~ENABLE_LINE_INPUT;
    inputMode &= ~ENABLE_ECHO_INPUT;
    inputMode &= ~ENABLE_PROCESSED_INPUT;

    int retVal = SetConsoleMode(hStdin,inputMode);
    if (retVal == 0){
        DWORD errorCode = GetLastError();
        std::cout << "Error during SetConsoleMode. Mode: " << retVal << ":  Error Code: " << errorCode << std::endl;
    }

    // now, can't use ReadConsole, as it prefilters the ANSI escape codes out.
    char readChar;
    uint charsRead;
    std::stringstream inputString;
    bool reading = true;
    while (reading) {
        //std::cin.get(readChar);     // doesn't act in a 'raw' manner - the char is still printed to the terminal, and need to press return to progress the proram
        ReadConsole(hStdin,&readChar,1,&charsRead,NULL);    // does work with ReadConsole, but then don't get ANSI codes.

        if (readChar == 'z'){
            break;
        }
        inputString << readChar;
    }

    retVal = SetConsoleMode(hStdin,origInputMode);
    if (retVal == 0){
        DWORD errorCode = GetLastError();
        std::cout << "Error during RestoreConsoleMode. Mode: " << retVal << ":  Error Code: " << errorCode << std::endl;
    }
    hexPrint(inputString.str());
    //std::cout << inputString.str() << std::endl;
    return;
}

#include <termios.h>
void MinimumRawModeTermiosTest() {
    std::cout << "Entering raw mode termios test." << std::endl;

    //save windows console values.
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD origInputMode;
    GetConsoleMode(hStdin, &origInputMode);
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD origOutputMode;
    GetConsoleMode(hStdout, &origOutputMode);


    termios attr;
    tcgetattr(STDIN_FILENO, &attr);
    termios origAttr = attr;

    attr.c_lflag &= ~ICANON;    //clear Canonical Mode flag
    attr.c_lflag &= ~ECHO;      //clear Echo flag
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &attr);

    DWORD midInputMode;
    GetConsoleMode(hStdin, &midInputMode);
    DWORD midOutputMode;
    GetConsoleMode(hStdout, &midOutputMode);


    // now, can't use ReadConsole, as it prefilters the ANSI escape codes out.
    char readChar;
    std::stringstream inputString;
    bool reading = true;
    while (reading) {
        //read(STDIN_FILENO, &readChar, 1);           // Finally! it works!
        //readChar = fgetc(stdin);                    // Also works, and is a little easier to parse.
        std::cin.get(readChar);                     // And also works. Basically, pick whichever I find easiest to read.

        if (readChar == 'z'){
            printCP("\n");
            break;
        }
        printCP(std::string(1,readChar));
        inputString << readChar;
    }

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &origAttr);

    DWORD newInputMode;
    GetConsoleMode(hStdin, &newInputMode);
    DWORD newOutputMode;
    GetConsoleMode(hStdout, &newOutputMode);

    std::stringstream debugString;
    debugString <<std::hex;
    debugString << "The orig inputMode was 0x" << origInputMode << ", then 0x" << midInputMode << ", but is now 0x" << newInputMode << std::endl;
    debugString << "The orig outputMode was 0x" << origOutputMode << ", then 0x" << midOutputMode << ", but is now 0x" << newOutputMode << std::endl;
    printCP(debugString.str());
    // The orig inputMode was 0x1a, then 0x18, but is now 0x1e
    // The orig outputMode was 0x17, then 0x3, but is now 0x3

    int retVal = SetConsoleMode(hStdin,origInputMode);
    if (retVal == 0){
        DWORD errorCode = GetLastError();
        std::cout << "Error during RestoreConsoleMode. Input mode: " << retVal << ":  Error Code: " << errorCode << std::endl;
    }
    retVal = SetConsoleMode(hStdout,origOutputMode);
    if (retVal == 0){
        DWORD errorCode = GetLastError();
        std::cout << "Error during RestoreConsoleMode. Output mode: " << retVal << ":  Error Code: " << errorCode << std::endl;
    }

    hexPrint(inputString.str());
    return;
}


#  ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#    define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#  endif
#  ifndef ENABLE_LVB_GRID_WORLDWIDE
#    define ENABLE_LVB_GRID_WORLDWIDE 0x0010
#  endif

// Using what I learned from MinimumRawModeTermiosTest
void MinimumRawModeTest2(){
    std::cout << "Entering raw mode test 2." << std::endl;
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD inputMode;
    GetConsoleMode(hStdin, &inputMode);
    DWORD origInputMode = inputMode;

    inputMode &= ~ENABLE_LINE_INPUT;        // clear
    inputMode &= ~ENABLE_ECHO_INPUT;        // clear
    inputMode &= ~ENABLE_PROCESSED_INPUT;   //clear

    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD outputMode;
    GetConsoleMode(hStdout, &outputMode);
    DWORD origOutputMode = outputMode;

    outputMode &= ~ENABLE_VIRTUAL_TERMINAL_PROCESSING;  //clear
    outputMode &= ~ENABLE_LVB_GRID_WORLDWIDE;           //clear

    int retVal = SetConsoleMode(hStdin,inputMode);
    if (retVal == 0){
        DWORD errorCode = GetLastError();
        std::cout << "Error during SetConsoleMode. Input Mode: " << retVal << ":  Error Code: " << errorCode << std::endl;
    }
    retVal = SetConsoleMode(hStdout,outputMode);
    if (retVal == 0){
        DWORD errorCode = GetLastError();
        std::cout << "Error during SetConsoleMode. Output Mode: " << retVal << ":  Error Code: " << errorCode << std::endl;
    }


    // now, can't use ReadConsole, as it prefilters the ANSI escape codes out.
    char readChar;
    uint charsRead;
    std::stringstream inputString;
    bool reading = true;
    while (reading) {
        //ReadConsole(hStdin,&readChar,1,&charsRead,NULL);    // Still no ANSI codes, as expected
        //std::cin.get(readChar);                             // No better.
        read(STDIN_FILENO, &readChar, 1);                   // Same as cin.get(). Still echos, and still requires return.

        if (readChar == 'z'){
            break;
        }
        inputString << readChar;
    }

    retVal = SetConsoleMode(hStdin,origInputMode);
    if (retVal == 0){
        DWORD errorCode = GetLastError();
        std::cout << "Error during RestoreConsoleMode. Input Mode: " << retVal << ":  Error Code: " << errorCode << std::endl;
    }
    retVal = SetConsoleMode(hStdout,origOutputMode);
    if (retVal == 0){
        DWORD errorCode = GetLastError();
        std::cout << "Error during RestoreConsoleMode. Output Mode: " << retVal << ":  Error Code: " << errorCode << std::endl;
    }

    hexPrint(inputString.str());
    //std::cout << inputString.str() << std::endl;
    return;
}