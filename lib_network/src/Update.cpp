//
// Created by AndrewLee on 19-Mar-17.
//

#include "Update.h"
#include <iostream>

std::map<UpdateList, std::string> UpdateListStrings = {{UpdateList::L_Threshold,"L_Threshold"},
                                                       {UpdateList::L_MostWins,"L_MostWins"},
                                                       {UpdateList::SL_Interrogate,"SL_Interrogate"},
                                                       {UpdateList::SL_ThresholdScoreTB,"SL_ThresholdScoreTB"},
                                                       {UpdateList::G_ThresholdGDPTB,"G_ThresholdGDPTB"},
                                                       {UpdateList::G_ThresholdGPPTB,"G_ThresholdGPPTB"},
                                                       {UpdateList::G_GDP,"G_GDP"}};

Update* updateFromUpdateType(UpdateList updateType) {
    switch (updateType) {
        case UpdateList::L_Threshold:
            return new L_Threshold();
        case UpdateList::L_MostWins:
            return new L_MostWins();
        case UpdateList::SL_Interrogate:
            return new SL_Interrogate();
        case UpdateList::SL_ThresholdScoreTB:
            return new SL_ThresholdScoreTB();
        case UpdateList::G_ThresholdGDPTB:
            return new G_ThresholdGDPTB();
        case UpdateList::G_ThresholdGPPTB:
            return new G_ThresholdGPPTB();
        case UpdateList::G_GDP:
            return new G_GDP();
    }
}

//parent class stub methods - should never be called.
void Update::update(Network::Node &) {
    throw std::logic_error("using stub strategy");
}
UpdateList Update::getName() {
    throw std::logic_error("using stub strategy");
}

//L_Threshold methods
void L_Threshold::update(Network::Node &node) {
    Network::Node * thisNode = &node;
    //Determine if the strategy will be updated
    auto opScores = thisNode->getOpScore();     //opponent scores
    double scoreAvg = thisNode->getScore() / (double)opScores.size();

    Network::Node* highestScoring = nullptr;
    double highestScore = scoreAvg;
    int count = 0;

    for (auto iter = opScores.begin(); iter != opScores.end(); iter++) {
        //determine if highest scoring
        if(iter->second > highestScore) {
            highestScoring = iter->first;   //if higher than previous high score, set as highest scoring neighbour
            highestScore = iter->second;
        }
        if (iter->second > scoreAvg)    //if node
            count++;
    }

    if(((double)count)/opScores.size() > threshold) {
        if(highestScoring->getOldStrategy() != thisNode->getOldStrategy())  //only update if strategy is different.
            thisNode->updateStrategy(highestScoring->getOldStrategy());
    }
}

UpdateList L_Threshold::getName() {
    return this->name;
}

//L_MostWins methods
void L_MostWins::update(Network::Node &node) {
    Network::Node * thisNode = &node;
    //Determine if the strategy will be updated
    auto opWins = thisNode->getOpWins();     //opponent scores
    double winAvg = thisNode->getWins() / (double)opWins.size();

    Network::Node* mostWinsNode = nullptr;
    double highestWins = winAvg;
    int count = 0;

    for (auto iter = opWins.begin(); iter != opWins.end(); iter++) {
        //determine if highest scoring
        if(iter->second > highestWins) {
            mostWinsNode = iter->first;   //if higher than previous high score, set as highest scoring neighbour
            highestWins = iter->second;
        }
        if (iter->second > winAvg)    //if node
            count++;
    }

    if(((double)count)/opWins.size() > threshold) {
        if(mostWinsNode->getOldStrategy() != thisNode->getOldStrategy())  //only update if strategy is different.
            thisNode->updateStrategy(mostWinsNode->getOldStrategy());
    }
}

UpdateList L_MostWins::getName() {
    return this->name;
}

//SL_Interrogate methods
void SL_Interrogate::update(Network::Node &node) {
    //int x = node->first.first;
    //int y = node->first.second;
    //int xLower = x!=0 ? x-1 : dims.first - 1;
    //int xUpper = x!=dims.first - 1 ? x+1 : 0;
    //int yLower = y!=0 ? y-1 : dims.second - 1;
    //int yUpper = y!=dims.second - 1 ? y+1 : 0;

    int lowestScore = node.getScore();
    int highestScore = node.getScore();
    Network::Node * thisNode = &node;
    Network::Node * testNode = nullptr;
    Network::Node * bestNode = &node;
    auto opScore = thisNode->getOpScore();
    for (auto iter = opScore.begin(); iter != opScore.end(); iter++) {
        testNode = iter->first;
        if (testNode->getScore() < lowestScore)
            return; //if there is a node with a worse score than the current node, do not change.
        if (testNode->getScore() > highestScore){
            bestNode = testNode;
            highestScore = testNode->getScore();
        }
    }
    //if reach this point, have lowest score
    if(bestNode->getOldStrategy() != thisNode->getOldStrategy())
        thisNode->updateStrategy(bestNode->getOldStrategy());

}

UpdateList SL_Interrogate::getName() {
    return this->name;
}

//SL_ThresholdScoreTB methods
void SL_ThresholdScoreTB::update(Network::Node &node) {
    Network::Node * thisNode = &node;
    //Determine if the strategy will be updated
    auto opScores = thisNode->getOpScore();     //opponent scores
    double scoreAvg = thisNode->getScore() / (double)opScores.size();

    Network::Node* highestScoring = nullptr;
    double highestScore = scoreAvg;
    int count = 0;

    std::map<Network::Node*,int> winningNeighbours;

    for (auto iter = opScores.begin(); iter != opScores.end(); iter++) {
        //determine if highest scoring
        if(iter->second > highestScore) {
            highestScoring = iter->first;   //if higher than previous high score, set as highest scoring neighbour
            highestScore = iter->second;
            winningNeighbours.clear();
            winningNeighbours.insert({highestScoring, highestScoring->getScore()});
        }
        if(iter->second == highestScore) {
            winningNeighbours.insert({iter->first, iter->first->getScore()});
        }
        if (iter->second > scoreAvg)    //if node
            count++;
    }

    if(((double)count)/opScores.size() > threshold) {
        Network::Node* bestScoring = winningNeighbours.begin()->first;
        int bestTotalScore = winningNeighbours.begin()->second;

        for (auto iter = winningNeighbours.begin(); iter != winningNeighbours.end(); iter++) {
            if (iter->second > bestTotalScore){
                bestScoring = iter->first;
                bestTotalScore = iter->second;
            }
        }
        if (bestScoring->getOldStrategy() != thisNode->getOldStrategy())
            thisNode->updateStrategy(bestScoring->getOldStrategy()); //Cannot be null.
    }
}

UpdateList SL_ThresholdScoreTB::getName() {
    return this->name;
}

//G_ThresholdGDPTB methods
void G_ThresholdGDPTB::update(Network::Node &node) {
    Network::Node * thisNode = &node;
    //Determine if the strategy will be updated
    auto opScores = thisNode->getOpScore();     //opponent scores
    double scoreAvg = thisNode->getScore() / (double)opScores.size();

    Network::Node* highestScoring = nullptr;
    double highestScore = scoreAvg;
    int count = 0;

    std::map<Network::Node*,int> winningNeighbours;

    for (auto iter = opScores.begin(); iter != opScores.end(); iter++) {
        //determine if highest scoring
        if(iter->second > highestScore) {
            highestScoring = iter->first;   //if higher than previous high score, set as highest scoring neighbour
            highestScore = iter->second;
            winningNeighbours.clear();
            winningNeighbours.insert({highestScoring, highestScoring->getScore()});
        }
        if(iter->second == highestScore) {
            winningNeighbours.insert({iter->first, iter->first->getScore()});
        }
        if (iter->second > scoreAvg)    //if node
            count++;
    }

    if(((double)count)/opScores.size() > threshold) {
        Network * network = thisNode->getNetwork();
        auto gdpMap = network->getGlobalTotalScore();

        Network::Node* bestScoring = winningNeighbours.begin()->first;
        int bestGDP = gdpMap.find(bestScoring->getOldStrategy())->second;

        for (auto iter = winningNeighbours.begin(); iter != winningNeighbours.end(); iter++) {
            int gdp = gdpMap.find(iter->first->getOldStrategy())->second;
            if (gdp > bestGDP){
                bestScoring = iter->first;
                bestGDP = gdp;
            }
        }
        if (bestScoring->getOldStrategy() != thisNode->getOldStrategy())
            thisNode->updateStrategy(bestScoring->getOldStrategy()); //Cannot be null.
    }
}

UpdateList G_ThresholdGDPTB::getName() {
    return this->name;
}

//G_ThresholdGPPTB methods
void G_ThresholdGPPTB::update(Network::Node &node) {
    Network::Node * thisNode = &node;
    //Determine if the strategy will be updated
    auto opScores = thisNode->getOpScore();     //opponent scores
    double scoreAvg = thisNode->getScore() / (double)opScores.size();

    Network::Node* highestScoring = nullptr;
    double highestScore = scoreAvg;
    int count = 0;

    std::map<Network::Node*,int> winningNeighbours;

    for (auto iter = opScores.begin(); iter != opScores.end(); iter++) {
        //determine if highest scoring
        if(iter->second > highestScore) {
            highestScoring = iter->first;   //if higher than previous high score, set as highest scoring neighbour
            highestScore = iter->second;
            winningNeighbours.clear();
            winningNeighbours.insert({highestScoring, highestScoring->getScore()});
        }
        if(iter->second == highestScore) {
            winningNeighbours.insert({iter->first, iter->first->getScore()});
        }
        if (iter->second > scoreAvg)    //if node
            count++;
    }

    if(((double)count)/opScores.size() > threshold) {
        Network * network = thisNode->getNetwork();
        auto gppMap = network->getGlobalMeanScorePerEdge();

        Network::Node* bestScoring = winningNeighbours.begin()->first;
        double bestGPP = gppMap.find(bestScoring->getOldStrategy())->second;

        for (auto iter = winningNeighbours.begin(); iter != winningNeighbours.end(); iter++) {
            double gpp = gppMap.find(iter->first->getOldStrategy())->second;
            if (gpp > bestGPP){
                bestScoring = iter->first;
                bestGPP = gpp;
            }
        }
        if (bestScoring->getOldStrategy() != thisNode->getOldStrategy())
            thisNode->updateStrategy(bestScoring->getOldStrategy()); //Cannot be null.
    }
}

UpdateList G_ThresholdGPPTB::getName() {
    return this->name;
}

//G_GDP methods
void G_GDP::update(Network::Node &node) {
    Network::Node * thisNode = &node;
    Network * network = thisNode->getNetwork();
    auto gdpMap = network->getGlobalTotalScore();
    StrategyList bestStrategy = gdpMap.begin()->first;
    int bestGDP = gdpMap.begin()->second;

    for (auto iter = gdpMap.begin(); iter != gdpMap.end(); iter++) {
        int gdp = iter->second;
        if (gdp > bestGDP){
            bestStrategy = iter->first;
            bestGDP = gdp;
        }
    }
    if (bestStrategy != thisNode->getOldStrategy())
        thisNode->updateStrategy(bestStrategy); //Cannot be null.
}

UpdateList G_GDP::getName() {
    return this->name;
}



// Functions to produce initial Update arrays.
boost::random::mt19937 Update::gen;
void Update::SetRNG(boost::random::mt19937 newRNG){ gen = newRNG; }

Update* Update::DefaultWithPureGDP(std::pair<int, int> position, std::pair<int, int> dimensions, UpdateList defaultUpdateType){
    double GDPfraction = 0.1;
    boost::random::uniform_01 <boost::random::mt19937> dist(gen);
    if (dist()< GDPfraction)
        return new G_GDP;
    return updateFromUpdateType(defaultUpdateType);
}

Update* Update::Default(std::pair<int, int> position, std::pair<int, int> dimensions, UpdateList defaultUpdateType){
    return updateFromUpdateType(defaultUpdateType);
}