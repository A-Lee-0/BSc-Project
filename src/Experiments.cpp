//
// Created by Andrew Lee on 28/06/2023.
//

#include "Experiments.h"
#include <iostream>
#include <boost/progress.hpp>
#include "Game.h"



//parent class stub methods - should never be called.

std::string Experiment::getName() {
    return name;
}
void Experiment::run() {
    throw std::logic_error("using stub experiment!");
}


//LoopOneUpdateType Methods
void LoopOneUpdateType::run() {
    boost::progress_display GPRProg(maxGPR);

    std::string updateString = UpdateListStrings[updateType];
    for (int i = 1; i != maxGPR; i++) {
        std::string filepath = name + "gprTest_" + updateString + "_CM_auto\\random_" + std::to_string(i) + "gpr";
        Network network = Network(networkSize, networkSize, Strategy::random, updateType, filepath);
        network.connectNearestNeighbours();
        playNRounds(network, 50, i, true, true);
        ++GPRProg;
    }
}