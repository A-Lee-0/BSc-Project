#include <iostream>
#include <vector>
#include <map>
#include <random>
#include <boost/random.hpp>
#include <ctime>
#include <boost/filesystem.hpp>

#include "SGCB_Config.h"
#include "Network.h"
#include "Strategy.h"
#include "Update.h"
#include "Game.h"
#include "Experiment.h"

std::time_t startTime_main;
boost::random::mt19937 gen(static_cast<unsigned int>(std::time(&startTime_main)));


int main() {
    std::cout << "From Simple Games to Complex Behaviour\n"
              << "Written by Samuel Kirby and Andrew Lee for BSc Project supervised by Dr Dave Clements\n"
              << "Version: " << VERSION_MAJOR << "." << VERSION_MINOR << std::endl;


    time_t t = time(0);   // get time now
    struct tm * now = localtime( & t );
    std::cout << "Time: " << (now->tm_year + 1900) << '-' << (now->tm_mon + 1) << '-' << now->tm_mday << ' '
              << now->tm_hour << ':' << now->tm_min << ':' << now->tm_sec << std::endl;

    //init RNGs
    Strategy::SetRNG(gen);
    Update::SetRNG(gen);

    Experiment * experiment = new RepeatExperiment();
    Experiment &experimentRef = *experiment;


    bool inputting = true;
    while (inputting){
        std::cout << "Enter 'run' to start the experiment." << std::endl;
        //std::cout << "Enter 'run' to start the experiment, or enter 'help' to see other options." << std::endl;
        char rawInput[50];
        std::cin.getline(rawInput,50);
        std::string input = std::string(rawInput);

        if ( input == "run") {
            std::cout << "Start the experiment!" << std::endl;
            inputting = false;
            experiment->Run();
        }
        else if ( input == "help") {
            //TODO: make 'help' actually helpful!
            std::cout << "Need to implement changing experiment parameters from console!" << std::endl;
        }
        else if ( input.substr(0,4) == "set ") {
            try {
                std::string instruction = input.substr(4, input.length() - 4);
                auto pos = instruction.find(" ");
                std::string paramName = instruction.substr(0, pos);
                std::string newValue = instruction.substr(pos + 1);
                //std::cout << "Attempting to set parameter '" << paramName << "' to the value '" << newValue << "'."
                //          << std::endl;
                //experiment->SetParameter(paramName, newValue);
                ConsoleReturn consRet = experiment->SetParameter(paramName, newValue);
                switch (consRet) {
                    case ConsoleReturn::Success:
                        std::cout << "Parameter name '" << paramName << "' successfully changed to '"
                                  << experiment->GetParameter(paramName) << "'." << std::endl;
                        break;
                    case ConsoleReturn::UnknownParamString:
                        std::cout << "Parameter name '" << paramName << "' was not recognised." << std::endl;
                        break;
                    case ConsoleReturn::NotApplicableParam:
                        std::cout << "Parameter name '" << paramName << "' is not used by experiment type '"
                                  << experiment->getName() << "'." << std::endl;
                        break;
                    default:
                        std::cout << "Something went wrong! Parameter '" << paramName << "' is currently '"
                                  << (experiment->GetParameter(paramName)) << "'." << std::endl;
                        break;
                }
            }
            catch (...){
                std::cout << "Something went wrong..." << std::endl;
            }
        }
        else if ( input.substr(0,4) == "get ") {
            try {
                std::string instruction = input.substr(4, input.length() - 4);
                auto pos = instruction.find(" ");   // only look upto first space, in case someone just flips 'set' to 'get'
                std::string paramName = instruction.substr(0, pos);
                //std::cout << "Attempting to get the current value of parameter '" << paramName << "'." << std::endl;
                std::vector<std::string> paramNames = ListParameterNames(experiment);
                if (std::find(paramNames.begin(), paramNames.end(), paramName) != paramNames.end()) {
                    ParameterName param = parameterEnumMap.at(paramName);
                    std::cout << "Parameter '" << paramName << "' is currently '" << (experiment->GetParameter(param))
                              << "'." << std::endl;
                }
            }
            catch (...) {
                std::cout << "Something went wrong..." << std::endl;
            }
        }

        else if ( input == "params") {
            try {
                PrintParameterTable(experiment);
            }
            catch (...) {
                std::cout << "Something went wrong..." << std::endl;
            }
        }

        else if ( input.substr(0,11) == "experiment ") {
            try {
                std::string instruction = input.substr(11, input.length() - 11);
                auto pos = instruction.find(" ");   // only look upto first space, in case someone just flips 'set' to 'get'
                std::string newExperimentName = instruction.substr(0, pos);

                if(experimentNameMap.count(newExperimentName)){
                    ExperimentList experimentType = experimentNameMap.at(newExperimentName);
                    delete experiment;
                    experiment = CreateExperiment(experimentType);
                    std::cout << "Created new experiment of type '" << experiment->getName() << "'." << std::endl;
                    PrintParameterTable(experiment);
                }
                else{
                    std::cout << "new Experiment '" << newExperimentName << "' is not a recognised Experiment type. Valid Experiments are: " << std::endl;
                    for(const auto &exPair : experimentNameMap){
                        std::cout << "    " << exPair.first << std::endl;
                    }
                }
            }
            catch (...) {
                std::cout << "Something went wrong..." << std::endl;
            }
        }

        else if ( input == "experiment") {
            try {
                std::cout << "The current Experiment is: " << experiment->getName() << std::endl;
                std::cout << "Desciption: " << experiment->getDescription() << std::endl;
                std::cout << "Valid Experiments are: " << std::endl;
                for(const auto &exPair : experimentNameMap){
                    std::cout << "    " << exPair.first << std::endl;
                }
            }
            catch (...) {
                std::cout << "Something went wrong..." << std::endl;
            }
        }

        //TODO: allow displying 'special' param permitted values, i.e. for InitialStrategiesMethod and UpdateType
        //TODO: allow setting StrategyGenerator parameters, e.g. centreStrat, surroundingStrat

        else{
            std::cout << "No intruction recognised... try 'help' to see your options." << std::endl;
        }
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

    t = time(0);   // get time now
    now = localtime( & t );
    char datetime[20];
    strftime(datetime,20,"%Y-%m-%d %H:%M",now);
    std::cout << std::endl;
    std::cout << "Time Finished: " << datetime << std::endl;
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
            Network network = Network(100, 100, strategyFunc, Update::DefaultWithPureGDP, filepath, updateType);
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
                Network network = Network(100, 100, strategyFunc, Update::DefaultWithPureGDP, filepath, updateType);
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
            Network network = Network(100, 100, strategyFunc, Update::Default, filepath, updateType, blockSize);
            network.connectNearestNeighbours();
            playNRounds(network, rounds, i, true, true);
//            ++showGPRProg;
        }
//        ++showUpdateProg;
    }
}


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