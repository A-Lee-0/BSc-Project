//
// Created by Samuel Kirby and Andrew Lee on 2/2/17.
//

#include "Network.h"

#include <fstream>
#include <string>
#include <cstring>
#include <iostream>
#include <boost/filesystem.hpp>

//-----Network Methods-------------------------------------
//----------Constructors-----------------------------------
/*
 * Create a rectangular network with number of nodes along x and y axis defined by xDim and yDim respectively. Give them a
 * strategy defined by defaultStrategy. Use for networks where only a few nodes will differ from the default.
 */
Network::Network(int xDim, int yDim, StrategyList defaultStrategy) : xDim(xDim), yDim(yDim) {
    //create correct number of nodes, give coordinates for easy drawing and access
    for (int x = 0; x < xDim; x++)
    {
        for (int y = 0; y < yDim; y++) {
            switch (defaultStrategy) {
                case StrategyList::AlwaysDefect:
                    nodeMap.insert({{x,y},Node(x,y,new AlwaysDefect(),this)});
                    break;
                case StrategyList::AlwaysCooperate:
                    nodeMap.insert({{x,y},Node(x,y,new AlwaysCooperate(),this)});
                    break;
                case StrategyList::TitForTat:
                    nodeMap.insert({{x,y},Node(x,y,new TitForTat(),this)});
                    break;
                case StrategyList::Titx2ForTat:
                    nodeMap.insert({{x,y},Node(x,y,new Titx2ForTat(),this)});
                    break;
                case StrategyList::SuspiciousTitForTat:
                    nodeMap.insert({{x,y},Node(x,y,new SuspiciousTitForTat(),this)});
                    break;
            }
        }
    }
}

/*
 * Create a rectangular network with number of nodes along x and y axes defined by xDim and yDim respectively.
 * Each node has a strategy chosen by the function strategyFunc, which should take two pairs of integers as parameters
 * - the first is the current coordinate and the second is the dimension of the network. It should return a pointer to a
 * new strategy.
 */
Network::Network(int xDim, int yDim, StrategyMethodPnt strategyFunc) : xDim(xDim), yDim(yDim) {
    for (int x = 0; x < xDim; x++) {
        for (int y = 0; y < yDim; y++) {
            Node node = Node(x,y,strategyFunc({x,y},{xDim,yDim}),this);
            node.setUpdateType(UpdateList::L_Threshold);
            nodeMap.insert({{x,y},node});
        }
    }
}

Network::Network(int xDim, int yDim, StrategyMethodPnt strategyFunc, UpdateList updateType, std::string subDir) : xDim(xDim), yDim(yDim),subDir(subDir) {

    CreateDirectory(subDir);

    for (int x = 0; x < xDim; x++) {
        for (int y = 0; y < yDim; y++) {
            Node node = Node(x,y,strategyFunc({x,y},{xDim,yDim}),this);
            node.setUpdateType(updateType);
            nodeMap.insert({{x,y},node});
        }
    }
}

Network::Network(int xDim, int yDim, StrategyMethodPnt strategyFunc, UpdateMethodPnt updateFunc, std::string subDir, UpdateList defaultUpdateType) : xDim(xDim), yDim(yDim),subDir(subDir) {
    CreateDirectory(subDir);
    for (int x = 0; x < xDim; x++) {
        for (int y = 0; y < yDim; y++) {
            Node node = Node(x,y,strategyFunc({x,y},{xDim,yDim}),this);
            node.setUpdateType(updateFunc({x,y},{xDim,yDim},defaultUpdateType));
            nodeMap.insert({{x,y},node});
        }
    }
}

Network::Network(int xDim, int yDim, StrategyMethodPnt strategyFunc, UpdateMethodPnt updateFunc, std::string subDir, UpdateList defaultUpdateType, int blocksize) : xDim(xDim), yDim(yDim),subDir(subDir) {
    Network network2 = Network((xDim/blocksize) +1, (yDim/blocksize)+1, strategyFunc, updateFunc, subDir, defaultUpdateType);
    CreateDirectory(subDir);
    for (int x = 0; x < xDim; x++) {
        for (int y = 0; y < yDim; y++) {
            Strategy * strategy = GetStrategyFromStrategyList(network2.getNodeByCoord({x/blocksize,y/blocksize}).getCurrentStrategy());
            Node node = Node(x,y,strategy,this);
            node.setUpdateType(updateFunc({x,y},{xDim,yDim},defaultUpdateType));
            nodeMap.insert({{x,y},node});
        }
    }
}

Network::Network(std::string filename) {
    std::fstream inputFile;
    inputFile.open(filename, std::ios::in);
    if (!inputFile.is_open()){
        std::cout << "File Did Not Open." << std::endl;
        throw std::logic_error("File Did Not Open.");
    }

    std::string line;

    std::getline(inputFile,line);

    unsigned long oldLinePos = 0;
    unsigned long linePos = line.find(",");
    this->xDim = std::stoi(line.substr(oldLinePos,linePos-oldLinePos));
    this->yDim = std::stoi(line.substr(linePos+1));
    while(std::getline(inputFile,line)) {
        oldLinePos = 0;
        linePos = line.find(",");
        int x = std::stoi(line.substr(oldLinePos,linePos-oldLinePos));
        oldLinePos = linePos+1;
        linePos = line.find(",",oldLinePos+1);
        int y = std::stoi(line.substr(oldLinePos,linePos-oldLinePos));
        std::string strat = line.substr(linePos+1);
        strat.pop_back();
        StrategyList stratlist = strategyStrings[strat];
        Network::Node node = Node(GetStrategyFromStrategyList(stratlist));


        nodeMap.insert({{x,y},node});
    }
    inputFile.close();
    connectNearestNeighbours();
}

//----------Methods----------------------------------------
std::map<std::pair<int, int>, Network::Node> &Network::getNodeMap() {
    return nodeMap;
}

const std::vector<std::pair<Network::Node *, Network::Node *>> &Network::getEdges() const {
    return edges;
}

Network::Node & Network::getNodeByCoord(std::pair<int, int> coord) {
    return this->nodeMap.find(coord)->second;
}

const std::pair<int, int> Network::getDimensionality() const {
    return {this->xDim, this->yDim};
}

std::string Network::getSubDir(){
    return subDir;
}
void Network::setSubDir(std::string newSubDir) {
    subDir = newSubDir;
    std::string filePath = std::string(newSubDir);
    CreateDirectory(subDir);
}

void Network::connectNearestNeighbours() {
    /*
     * For every node, create 4 edges (this results in every node having eight edges).
     * o    o   x
     * o    O   x
     * o    x   x
     * For the node O, xs are connected in it's iteration. os are connected in previous iterations.
     * This system will have periodic boundary conditions.
     */
    for (auto it = this->nodeMap.begin(); it != this->nodeMap.end(); it++)
    {
        int x = it->first.first;
        int y = it->first.second;
        int xUpper = x!=this->xDim - 1  ?   x+1     :   0;
        int yLower = y!=0               ?   y-1     :   this->yDim - 1;
        int yUpper = y!=this->yDim - 1  ?   y+1     :   0;

        Network::Node* pNode = &(it->second);

        this->edges.push_back({pNode, &(this->getNodeByCoord({xUpper,yLower}))});   // /^
        this->edges.push_back({pNode, &(this->getNodeByCoord({xUpper,y}))});        // ->
        this->edges.push_back({pNode, &(this->getNodeByCoord({xUpper,yUpper}))});   // \.
        this->edges.push_back({pNode, &(this->getNodeByCoord({x, yUpper}))});       // \|/
    }
}

void Network::writeToFile(std::string filename, bool withEdges) {
    char * charFilePath = (char*) subDir.c_str();
    boost::filesystem::path dir(charFilePath);
    boost::filesystem::create_directory(dir);
    std::fstream outputFile;
    outputFile.open(subDir + "\\" + filename, std::ios::out | std::ios::trunc);
    for (auto it = this->nodeMap.begin(); it != this->nodeMap.end(); it++)
        // first = node position, second = node object.
        outputFile << it->first.first << "," << it->first.second << "\t" << it->second.getScore()  << ","
                   << (int)it->second.getCurrentStrategy() << "\n";
    outputFile << "\n\n\n";             //maybe nicer to do this another way? could have a character on each line saying whether its a node or an edge - would make it easier to read the file back in.
    if (withEdges)
        for (auto it = this->edges.begin(); it != this->edges.end(); it++)
            outputFile << it->first->getCoord().first << "," << it->first->getCoord().second << "\t"
                       << it->second->getCoord().first << "," << it->second->getCoord().second << std::endl;
    outputFile.close();
}

void Network::exportNetwork(std::string filename) {
    std::fstream outputFile;
    outputFile.open(filename, std::ios::out | std::ios::trunc);
    outputFile << this->xDim << "," << this->yDim << "\r" << std::endl;
    for (auto it = this->nodeMap.begin(); it != this->nodeMap.end(); it++)
        // first = node position, second = node object.
        outputFile << it->first.first << "," << it->first.second << "\t" << (int)it->second.getCurrentStrategy() << "\r" << std::endl;
    outputFile.close();
}

void Network::resetNodeScores(){
    for(auto node = this->getNodeMap().begin(); node != this->getNodeMap().end(); node++)
        node->second.resetScore();
}

void Network::generateGlobalStats() {
    globalStats.clear();
    globalPopulation.clear();
    globalMeanScorePerNode.clear();
    globalMeanScorePerEdge.clear();
    globalTotalScore.clear();

    std::map<StrategyList,uint> globalGames;
    for (auto iter = nodeMap.begin(); iter != nodeMap.end(); iter++) {
        Node node = iter->second;
        if (globalTotalScore.find(node.getCurrentStrategy()) == globalTotalScore.end()) {
            globalTotalScore.insert({node.getCurrentStrategy(), node.getScore()});
            globalPopulation.insert({node.getCurrentStrategy(), 1});
            globalGames.insert({node.getCurrentStrategy(), 0});
        }
        else {
            globalTotalScore[node.getCurrentStrategy()] += (double) node.getScore();
            globalPopulation[node.getCurrentStrategy()] += 1;
        }
    }
    for (auto iter = edges.begin(); iter != edges.end(); iter++) {
        globalGames[iter->first->getCurrentStrategy()] += 1;
        globalGames[iter->second->getCurrentStrategy()] += 1;
    }
    for (auto iter = globalTotalScore.begin(); iter != globalTotalScore.end(); iter++) {
        int score = iter->second;
        int pop = globalPopulation.find(iter->first)->second;
        int games = globalGames.find(iter->first)->second;

        globalMeanScorePerNode.insert({iter->first, (double)score / (double) pop});
        globalMeanScorePerEdge.insert({iter->first, (double)score / (double) games});

    }

}

std::map<StrategyList, double> & Network::getGlobalStats(){
    return globalStats;
}
std::map<StrategyList, int> &Network::getGlobalPopulation(){
    return globalPopulation;
}
std::map<StrategyList, double> &Network::getGlobalMeanScorePerNode(){
    return globalMeanScorePerNode;
}
std::map<StrategyList, double> &Network::getGlobalMeanScorePerEdge(){
    return globalMeanScorePerEdge;
}
std::map<StrategyList, int> &Network::getGlobalTotalScore(){
    return globalTotalScore;
}

//-----Network::Node Methods-------------------------------
//----------Constructors-----------------------------------
Network::Node::Node(int x, int y, Strategy * strategy, Network * network) : x(x), y(y), strategy(strategy), network(network) {}

Network::Node::Node(Strategy * strategy) : strategy(strategy) {}

//----------Methods----------------------------------------
const std::pair<int, int> Network::Node::getCoord() const {
    return {this->x,this->y};
}

Outcome Network::Node::play(Network::Node * node) {
    return this->strategy->play(*node);
}

void Network::Node::feedback(Network::Node *opNode, Outcome choice, Outcome opChoice, int score, int opScore) {
    this->strategy->feedback(*opNode, opChoice);
    this->score += score;
    if (this->opScore.find(opNode) == this->opScore.end())
        this->opScore.insert({opNode, opScore});
    else
        this->opScore[opNode] += opScore;

    if (choice == Outcome::Defect && opChoice == Outcome::Cooperate)
        this->wins += 1;
    if (this->opWins.find(opNode) == this->opWins.end())
        this->opWins.insert({opNode, 0});
    if (choice == Outcome::Cooperate && opChoice == Outcome::Defect)
        this->opWins[opNode] += 1;
}

void Network::Node::updateStrategy(StrategyList strategy) {
    switch (strategy) {
        case StrategyList::AlwaysDefect:
            this->strategy = new AlwaysDefect();
            break;
        case StrategyList::AlwaysCooperate:
            this->strategy = new AlwaysCooperate();
            break;
        case StrategyList::TitForTat:
            this->strategy = new TitForTat();
            break;
        case StrategyList::Titx2ForTat:
            this->strategy = new Titx2ForTat();
            break;
        case StrategyList::SuspiciousTitForTat:
            this->strategy = new SuspiciousTitForTat();
            break;
    }
}
void Network::Node::backupStrategy() {
    this->oldStrategy = this->strategy;
}
StrategyList Network::Node::getCurrentStrategy() {
    return this->strategy->getName();
}
StrategyList Network::Node::getOldStrategy() {
    return this->oldStrategy->getName();
}

Update * Network::Node::getUpdateType(){
    return this->updateType;
}
void Network::Node::setUpdateType(Update* update){
    this->updateType = update;
}
void Network::Node::setUpdateType(UpdateList updateType) {
    this->updateType = updateFromUpdateType(updateType);
}
void Network::Node::updateStrategy(){
    this->updateType->update(*this);
}

void Network::Node::clearMemory() const {
    this->strategy->clearMemory();
}

int Network::Node::getScore() const {
    return this->score;
}
std::map<Network::Node*,int> &Network::Node::getOpScore() {
    return this->opScore;
}
int Network::Node::getWins() const {
    return this->wins;
}
std::map<Network::Node*,int> &Network::Node::getOpWins() {
    return this->opWins;
}

Network* Network::Node::getNetwork(){
    return this->network;
}

void Network::Node::resetScore() {
    this->score = 0;
    this->opScore.clear();

    this->wins = 0;
    this->opWins.clear();
}

// Duplicated from Game.cpp to prevent dependence on 'core' lib code from 'network' lib code.
void Network::CreateDirectory(std::string path){
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