#include <iostream>
#include <vector>
#include <map>
#include <random>
#include <string>
#include <boost/random.hpp>
#include <ctime>
#include <boost/filesystem.hpp>
#include <boost/progress.hpp>


#include "Network.h"
#include "Strategy.h"
#include "Update.h"
#include "Game.h"
#include "bitmap_image.hpp"
#include "Drawing.h"
#include "SGCB_Config.h"
#include <ctime>

Strategy * centreT4T(std::pair<int,int>,std::pair<int,int>);
Strategy * centre(std::pair<int,int>,std::pair<int,int>);
Strategy * random(std::pair<int,int>, std::pair<int,int>);
Strategy * random_NonST4T(std::pair<int,int>, std::pair<int,int>);
Update * DefaultWithPureGDP(std::pair<int, int> position, std::pair<int, int> dimensions, UpdateList defaultUpdateType);
Update * Default(std::pair<int, int> position, std::pair<int, int> dimensions, UpdateList defaultUpdateType);
void iteratedRunAllUpdatesForAllGPRs(int maxGPR,int rounds, int iterations, Strategy* strategyFunc(std::pair<int, int>, std::pair<int, int>));
void runAllUpdatesForAllGPRs(int maxGPR,int rounds, Strategy* strategyFunc(std::pair<int, int>, std::pair<int, int>));
void runAllUpdatesForAllGPRs_BlockNetwork(int maxGPR,int rounds, Strategy* strategyFunc(std::pair<int, int>, std::pair<int, int>),int blockSize);
void runAllUpdatesForAllGPRs_CentreNetwork(int maxGPR,int rounds, Strategy* strategyFunc(std::pair<int, int>, std::pair<int, int>),double centreradius);
//boost::random::mt19937 gen;
std::time_t startTime;
boost::random::mt19937 gen(static_cast<unsigned int>(std::time(&startTime)));

StrategyList centreStrat;
StrategyList surroundingsStrat;
double radius = 1;


int main() {
    std::cout << "From Simple Games to Complex Behaviour\n"
              << "Written by Samuel Kirby and Andrew Lee for BSc Project supervised by Dr Dave Clements\n"
              << "Version: " << VERSION_MAJOR << "." << VERSION_MINOR << std::endl;


    time_t t = time(0);   // get time now
    struct tm * now = localtime( & t );
    std::cout << "Time: " << (now->tm_year + 1900) << '-' << (now->tm_mon + 1) << '-' << now->tm_mday << ' '
              << now->tm_hour << ':' << now->tm_min << ':' << now->tm_sec << std::endl;
//Loop through one updateType
/*
    UpdateList updateType = UpdateList::G_GDP;
    int maxGPR = 15;
    boost::progress_display GPRProg(maxGPR);
    for (int i = 1; i != maxGPR; i++) {
        std::string updateString = UpdateListStrings[updateType];
        std::string filepath = "gprTest_" + updateString + "_CM_auto\\random_" + std::to_string(i) + "gpr";
        Network network = Network(100, 100, random, updateType, filepath);
        network.connectNearestNeighbours();
        playNRounds(network, 50, i, true, true);
        ++GPRProg;
    }
*/
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
    UpdateList updateType = UpdateList::SL_Interrogate;
    std::string updateString = UpdateListStrings[updateType];
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

    updateType = UpdateList::SL_ThresholdScoreTB;
    updateString = UpdateListStrings[updateType];
    for (int i = 0; i != 200; i++){
        std::cout << updateString << i << std::endl;

        std::string filepath = "IteratedDataSet3\\" + updateString + "\\15gpr\\iter" + std::to_string(i);
        Network network = Network(100,100,random, updateType, filepath);
        network.connectNearestNeighbours();
        playNRounds(network, 50, 15, false, true);
    }


    //iteratedRunAllUpdatesForAllGPRs(15,50,150,random);
    //runAllUpdatesForAllGPRs(15,50, random);
    //runAllUpdatesForAllGPRs_BlockNetwork(15,50,random, 5);
    //runAllUpdatesForAllGPRs_CentreNetwork(15,50,centre, 0.5);

    t = time(0);   // get time now
    now = localtime( & t );
    std::cout << "Time: " << (now->tm_year + 1900) << '-' << (now->tm_mon + 1) << '-' << now->tm_mday << ' '
              << now->tm_hour << ':' << now->tm_min << ':' << now->tm_sec << std::endl;
    return 0;
}

void runAllUpdatesForAllGPRs(int maxGPR,int rounds, Strategy* strategyFunc(std::pair<int, int>, std::pair<int, int>)){
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
            Network network = Network(100, 100, strategyFunc, DefaultWithPureGDP, filepath, updateType);
            network.connectNearestNeighbours();
            playNRounds(network, rounds, i, true, true);
//            ++showGPRProg;
        }
//        ++showUpdateProg;
    }
}

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
                Network network = Network(100, 100, strategyFunc, DefaultWithPureGDP, filepath, updateType);
                network.connectNearestNeighbours();
                playNRounds(network, rounds, i, false, true);
            }
        }
    }
}
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
            Network network = Network(100, 100, strategyFunc, Default, filepath, updateType, blockSize);
            network.connectNearestNeighbours();
            playNRounds(network, rounds, i, true, true);
//            ++showGPRProg;
        }
//        ++showUpdateProg;
    }
}
void runAllUpdatesForAllGPRs_CentreNetwork(int maxGPR,int rounds, Strategy* strategyFunc(std::pair<int, int>, std::pair<int, int>),double centreradius){
    std::cout << maxGPR << ", " << rounds << std::endl;
    radius = centreradius;
    //for (int l = (int)StrategyList::AlwaysDefect; l != (int) StrategyList::SuspiciousTitForTat+1; l++) {
    for (int l = (int)StrategyList::TitForTat; l != (int) StrategyList::TitForTat+1; l++) {
        surroundingsStrat = static_cast<StrategyList>(l);
        std::string surroundsString = StrategyListStrings[surroundingsStrat];
        std::cout << "surrounds:" << surroundsString << std::endl;
        //for (int k = (int)StrategyList::AlwaysDefect; k != (int) StrategyList::SuspiciousTitForTat+1; k++) {
        for (int k = (int)StrategyList::SuspiciousTitForTat; k != (int) StrategyList::SuspiciousTitForTat+1; k++) {
            centreStrat = static_cast<StrategyList>(k);
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
                        Network network = Network(15, 15, strategyFunc, Default, filepath, updateType);
                        network.connectNearestNeighbours();
                        playNRounds(network, rounds, i, true, true);
                    }
                }
            }
        }
    }
}

Strategy * centreT4T(std::pair<int,int> coord, std::pair<int,int>) {
    int radius2 = 3;
    int x = coord.first;
    int y = coord.second;

    if(x*x<radius2 && y*y<radius2)
        return new AlwaysDefect;
    else
        return new TitForTat;
}

Strategy * centre(std::pair<int,int> coord, std::pair<int,int> dimensions) {
    double radius2 = radius;
    double x = ((double) coord.first) + 0.5 - ((double) dimensions.first)/2;
    double y = ((double) coord.second) + 0.5 - ((double) dimensions.second)/2;

    if((double) (x*x + y*y)<radius2 * radius2)
        return GetStrategyFromStrategyList(centreStrat);
    return GetStrategyFromStrategyList(surroundingsStrat);
}

Strategy * random(std::pair<int,int>, std::pair<int,int>) {
    boost::random::uniform_int_distribution<> dist(0,4);
    switch (StrategyList(dist(gen))) {
        case StrategyList::AlwaysCooperate:
            return new AlwaysCooperate;
        case StrategyList::AlwaysDefect:
            return new AlwaysDefect;
        case StrategyList::TitForTat:
            return new TitForTat;
        case StrategyList::Titx2ForTat:
            return new Titx2ForTat;
        case StrategyList::SuspiciousTitForTat:
            return new SuspiciousTitForTat;
    }
}

/*
Strategy * randomBlocks(std::pair<int,int> position, std::pair<int,int> dimensions){
    int blocksize = 5;
    int blockArray[dimensions.first/blocksize +1][dimensions.second/blocksize +1] = {0};
    for (int i = 0;i != dimensions.first/blocksize +1; i++){
        for (int j = 0;i != dimensions.second/blocksize +1; i++){
        }
    }
    return new AlwaysCooperate;
}
*/

Strategy * random_NonST4T(std::pair<int,int>, std::pair<int,int>) {
    boost::random::uniform_int_distribution<> dist(0,3);
    switch (StrategyList(dist(gen))) {
        case StrategyList::AlwaysCooperate:
            return new AlwaysCooperate;
        case StrategyList::AlwaysDefect:
            return new AlwaysDefect;
        case StrategyList::TitForTat:
            return new TitForTat;
        case StrategyList::Titx2ForTat:
            return new Titx2ForTat;
    }
}

Strategy * rdT4T (std::pair<int,int>, std::pair<int,int>) {
    boost::random::uniform_01 <boost::random::mt19937> dist(gen);
    if (dist() < 0.3)
        return new TitForTat;
    else
        return new AlwaysDefect;
}

Update * DefaultWithPureGDP(std::pair<int, int> position, std::pair<int, int> dimensions, UpdateList defaultUpdateType){
    double GDPfraction = 0.1;
    boost::random::uniform_01 <boost::random::mt19937> dist(gen);
    if (dist()< GDPfraction)
        return new G_GDP;
    return updateFromUpdateType(defaultUpdateType);
}
Update * Default(std::pair<int, int> position, std::pair<int, int> dimensions, UpdateList defaultUpdateType){
    return updateFromUpdateType(defaultUpdateType);
}
