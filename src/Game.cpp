//
// Created by Samuel Kirby and Andrew Lee on 22/2/17.
//

#include <iostream>
#include <string>
#include <boost/filesystem.hpp>
#include <boost/progress.hpp>

#include "Game.h"

std::map<std::pair<Outcome, Outcome>, std::pair<int, int>> payoff = {{{Outcome::Cooperate, Outcome::Cooperate}, {3, 3}},
         {{Outcome::Cooperate, Outcome::Defect},    {0, 5}},
         {{Outcome::Defect,    Outcome::Cooperate}, {5, 0}},
         {{Outcome::Defect,    Outcome::Defect},    {1, 1}}};

void playNRounds(Network& network, int N, int gamesPerRound, bool saveBitmaps, bool returnGlobalData){
    int globalPopulation[N+1][5] = {0};
    double globalMeanScorePerNode[N+1][5] = {0};
    double globalMeanScorePerEdge[N+1][5] = {0};
    int globalTotalScore[N+1][5] = {0};

    std::string path = network.getSubDir() + "\\data";
    std::string name = "data\\round_0.txt";
    createDirectory(path);
    //network.writeToFile(name);

    network.generateGlobalStats();
    if (returnGlobalData) {
        for (int i = 0; i != 5; i++){
            StrategyList strategy = static_cast<StrategyList>(i);
            globalPopulation[0][i] = network.getGlobalPopulation()[strategy];
            globalMeanScorePerNode[0][i] = network.getGlobalMeanScorePerNode()[strategy];
            globalMeanScorePerEdge[0][i] = network.getGlobalMeanScorePerEdge()[strategy];
            globalTotalScore[0][i] = network.getGlobalTotalScore()[strategy];
        }
    }
    if (saveBitmaps) {
        name = "\\images\\round_0.bmp";
        path = network.getSubDir() + "\\images";
        createDirectory(path);
        saveBitmap(network, name);
    }

    for (int n = 0; n!=N;++n){
        //std::cout << "round " << n << std::endl;


        //play
        prepareNetwork(network); //clear memory from last round, and reset scores to zero.
        iterate(network,gamesPerRound);
        //network.updateStrategiesIfLowest();
        //updateNetworkByInterrogation(network);

        network.generateGlobalStats();
        updateNetworkByNode(network);

        //save data file
        name = "\\data\\round_" + std::to_string(n+1) + ".txt";
        //network.writeToFile(name);

        if (returnGlobalData) {
            /*auto nodeMap = &network.getNodeMap();
            for (auto node = nodeMap->begin(); node != nodeMap->end(); node++) {
                globalPopulation[n+1][intStrategies[node->second.getCurrentStrategy()]] += 1;
            }*/
            for (int i = 0; i != 5; i++){
                StrategyList strategy = static_cast<StrategyList>(i);
                globalPopulation[n+1][i] = network.getGlobalPopulation()[strategy];
                globalMeanScorePerNode[n+1][i] = network.getGlobalMeanScorePerNode()[strategy];
                globalMeanScorePerEdge[n+1][i] = network.getGlobalMeanScorePerEdge()[strategy];
                globalTotalScore[n+1][i] = network.getGlobalTotalScore()[strategy];
            }
        }
        if (saveBitmaps) {
            //save bmp file
            name = "images\\round_" + std::to_string(n + 1) + ".bmp";
            saveBitmap(network, name);
        }
    }
    if (returnGlobalData){
        name =  "data\\StrategyFractions.txt";
        std::fstream outputFile;
        outputFile.open(network.getSubDir() + "\\" + name, std::ios::out | std::ios::trunc);
        for (int i = 0; i !=N+1; ++i){
            outputFile << globalPopulation[i][0] << "," << globalPopulation[i][1] << "," << globalPopulation[i][2] << "," << globalPopulation[i][3] << "," << globalPopulation[i][4] << "\n";
        }
        outputFile.close();

        name =  "data\\globalMeanScorePerNode.txt";
        outputFile.open(network.getSubDir() + "\\" + name, std::ios::out | std::ios::trunc);
        for (int i = 0; i !=N+1; ++i)
            outputFile << globalMeanScorePerNode[i][0] << "," << globalMeanScorePerNode[i][1] << "," << globalMeanScorePerNode[i][2] << "," << globalMeanScorePerNode[i][3] << "," << globalMeanScorePerNode[i][4] << "\n";
        outputFile.close();

        name =  "data\\globalMeanScorePerEdge.txt";
        outputFile.open(network.getSubDir() + "\\" + name, std::ios::out | std::ios::trunc);
        for (int i = 0; i !=N+1; ++i)
            outputFile << globalMeanScorePerEdge[i][0] << "," << globalMeanScorePerEdge[i][1] << "," << globalMeanScorePerEdge[i][2] << "," << globalMeanScorePerEdge[i][3] << "," << globalMeanScorePerEdge[i][4] << "\n";
        outputFile.close();

        name =  "data\\globalTotalScore.txt";
        outputFile.open(network.getSubDir() + "\\" + name, std::ios::out | std::ios::trunc);
        for (int i = 0; i !=N+1; ++i)
            outputFile << globalTotalScore[i][0] << "," << globalTotalScore[i][1] << "," << globalTotalScore[i][2] << "," << globalTotalScore[i][3] << "," << globalTotalScore[i][4] << "\n";
        outputFile.close();
    }
}

void iterate(Network &network, int gamesPerRound) {
    for (int i = 0; i < gamesPerRound; i++) {
        auto edgeList = &network.getEdges();
        for (auto edge = edgeList->begin(); edge != edgeList->end(); edge++) {
            //Play
            std::pair<Outcome, Outcome> result = {edge->first->play(edge->second), edge->second->play(edge->first)};
            //Determine score
            auto score = payoff[result];
            //Feedback
            edge->first->feedback(edge->second, result.first, result.second, score.first, score.second);
            edge->second->feedback(edge->first, result.second, result.first, score.second, score.first);
        }
    }
}



//semi-local
void updateNetworkByInterrogation(Network &network){
    auto nodeList = &network.getNodeMap();
    std::pair<int,int> dims = network.getDimensionality();

    //Save current strategy to oldStrategy
    for (auto node = nodeList->begin(); node != nodeList->end(); node++) {

        int x = node->first.first;
        int y = node->first.second;
        network.getNodeByCoord({x,y}).backupStrategy();
    }

    for (auto node = nodeList->begin(); node != nodeList->end(); node++)  //for each node in the network...
    {
        int x = node->first.first;
        int y = node->first.second;
        int xLower = x!=0 ? x-1 : dims.first - 1;
        int xUpper = x!=dims.first - 1 ? x+1 : 0;
        int yLower = y!=0 ? y-1 : dims.second - 1;
        int yUpper = y!=dims.second - 1 ? y+1 : 0;

        int lowestScore = node->second.getScore();
        Network::Node *bestScore = &network.getNodeByCoord({x,y});

        Network::Node *testNode = &network.getNodeByCoord({xLower,yLower});
        if (testNode->getScore() < lowestScore) continue; if (testNode->getScore() > bestScore->getScore()) bestScore = testNode;
        testNode = &network.getNodeByCoord({xLower,y});
        if (testNode->getScore() < lowestScore) continue; if (testNode->getScore() > bestScore->getScore()) bestScore = testNode;
        testNode = &network.getNodeByCoord({xLower,yUpper});
        if (testNode->getScore() < lowestScore) continue; if (testNode->getScore() > bestScore->getScore()) bestScore = testNode;
        testNode = &network.getNodeByCoord({x,yLower});
        if (testNode->getScore() < lowestScore) continue; if (testNode->getScore() > bestScore->getScore()) bestScore = testNode;
        testNode = &network.getNodeByCoord({x,yUpper});
        if (testNode->getScore() < lowestScore) continue; if (testNode->getScore() > bestScore->getScore()) bestScore = testNode;
        testNode = &network.getNodeByCoord({xUpper,yLower});
        if (testNode->getScore() < lowestScore) continue; if (testNode->getScore() > bestScore->getScore()) bestScore = testNode;
        testNode = &network.getNodeByCoord({xUpper,y});
        if (testNode->getScore() < lowestScore) continue; if (testNode->getScore() > bestScore->getScore()) bestScore = testNode;
        testNode = &network.getNodeByCoord({xUpper,yUpper});
        if (testNode->getScore() < lowestScore) continue; if (testNode->getScore() > bestScore->getScore()) bestScore = testNode;

        //if reach this point, have lowest score
        if(network.getNodeByCoord({x,y}).getCurrentStrategy() != bestScore->getCurrentStrategy()){
            network.getNodeByCoord({x,y}).updateStrategy(bestScore->getCurrentStrategy());
        }
    }
}

//purely local
void updateNetwork(Network &network, double threshold) {
    auto nodeList = &network.getNodeMap();
    for (auto it = nodeList->begin(); it != nodeList->end(); it++)
    {
        Network::Node * node = &it->second;
        //Determine if the strategy will be updated
        auto opScores = node->getOpScore();     //opponent scores
        double scoreAvg = node->getScore() / (double)opScores.size();

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

        if(((double)count)/opScores.size() > threshold)
            node->updateStrategy(highestScoring->getCurrentStrategy()); //Cannot be null.
    }
}

//node method dependent
void updateNetworkByNode(Network &network){
    auto nodeList = &network.getNodeMap();

    //Save current strategy to oldStrategy
    for (auto node = nodeList->begin(); node != nodeList->end(); node++) {
        node->second.backupStrategy();
    }

    //update each node using its own strategy changing method.
    for (auto node = nodeList->begin(); node != nodeList->end(); node++) {
        node->second.getUpdateType()->update(node->second);
    }
}

void prepareNetwork(Network &network) {
    auto nodeList = &network.getNodeMap();
    for(auto it = nodeList->begin(); it != nodeList->end(); it++) {
        it->second.clearMemory();
        it->second.resetScore();
    }
}

void createDirectory(std::string path){
    std::size_t oldLinePos = 0;
    std::size_t linePos = 0;
    while(path.find("\\",oldLinePos) != std::string::npos){
        linePos = path.find("\\",oldLinePos);
        oldLinePos = linePos+1;
        std::string substring = path.substr(0,linePos);
        char* charPath = (char*) (substring).c_str();
        boost::filesystem::path dir(charPath);
        boost::filesystem::create_directory(dir);
    }
    char* charPath = (char*) (path).c_str();
    boost::filesystem::path dir(charPath);
    boost::filesystem::create_directory(dir);
}


//TODO: Fix fact that strategies can be changed THEN used to change another.