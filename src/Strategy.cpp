//
// Created by Samuel Kirby and Andrew Lee on 04/02/2017.
//
#include <iostream>
#include "Strategy.h"


Strategy * GetStrategyFromStrategyList(StrategyList name) {
    switch (name) {
        case StrategyList::AlwaysDefect:
            return new AlwaysDefect();
        case StrategyList::AlwaysCooperate:
            return new AlwaysCooperate();
        case StrategyList::TitForTat:
            return new TitForTat();
        case StrategyList::Titx2ForTat:
            return new Titx2ForTat();
        case StrategyList::SuspiciousTitForTat:
            return new SuspiciousTitForTat();
    }
}



std::map<std::string, StrategyList> strategyStrings = {{std::string("D"), StrategyList::AlwaysDefect},
                                                       {std::string("C"), StrategyList::AlwaysCooperate},
                                                       {std::string("TT"), StrategyList::TitForTat},
                                                       {std::string("TTT"), StrategyList::Titx2ForTat},
                                                       {std::string("STT"), StrategyList::SuspiciousTitForTat}};

std::map<StrategyList, std::string> StrategyListStrings = {{StrategyList::AlwaysDefect,"AlwaysDefect"},
                                                       {StrategyList::AlwaysCooperate,"AlwaysCoop"},
                                                       {StrategyList::TitForTat,"TitforTat"},
                                                       {StrategyList::Titx2ForTat,"Titfor2Tat"},
                                                       {StrategyList::SuspiciousTitForTat,"SuspTitforTat"}};

std::map<StrategyList, int> intStrategies = {{StrategyList::AlwaysDefect,0},
                                             {StrategyList::AlwaysCooperate,1},
                                             {StrategyList::TitForTat,2},
                                             {StrategyList::Titx2ForTat,3},
                                             {StrategyList::SuspiciousTitForTat,4}};


Outcome Strategy::play(Network::Node &) {
    throw std::logic_error("using stub strategy");
}

void Strategy::feedback(Network::Node &, Outcome) {
    throw std::logic_error("using stub strategy");
}

StrategyList Strategy::getName() {
    throw std::logic_error("using stub strategy");
}

void Strategy::clearMemory() {
    throw std::logic_error("using stub strategy");
}

// AlwaysDefect Strategy Methods: -----------------------------------------------------------------
Outcome AlwaysDefect::play(Network::Node &node) {
    return Outcome::Defect;
}

void AlwaysDefect::feedback(Network::Node &, Outcome) {
    return;
}

void AlwaysDefect::clearMemory() {
    return;
}

StrategyList AlwaysDefect::getName() {
    return this->name;
}


// AlwaysCooperate Strategy Methods: -----------------------------------------------------------------
Outcome AlwaysCooperate::play(Network::Node &node) {
    return Outcome::Cooperate;
}

void AlwaysCooperate::feedback(Network::Node &, Outcome) {
    return;
}

void AlwaysCooperate::clearMemory() {
    return;
}

StrategyList AlwaysCooperate::getName() {
    return this->name;
}

// TitForTat Strategy Methods: -----------------------------------------------------------------
Outcome TitForTat::play(Network::Node &node) {
    std::map<Network::Node*,Outcome>::iterator past = this->history.find(&node);
    if (past == history.end())
        return Outcome::Cooperate; // if no history with opponent, then cooperate
    switch (past->second) {
        case Outcome::Cooperate:
            return Outcome::Cooperate; // if cooperated last game, continue cooperation
        case Outcome::Defect:
            return Outcome::Defect; // if defected last game, then return fire
    }
}

void TitForTat::feedback(Network::Node &node, Outcome last) {
    history[&node] = last;
}

void TitForTat::clearMemory() {
    history.clear();
}

StrategyList TitForTat::getName() {
    return this->name;
}

// Titx2ForTat Strategy Methods: -----------------------------------------------------------------
Outcome Titx2ForTat::play(Network::Node &node) {
    std::map<Network::Node*, std::queue<Outcome>>::iterator past = history.find(&node);
    if (past == history.end())
        return Outcome::Cooperate; // have no history with opponent
    else if (past->second.size() < 2)
        return Outcome::Cooperate; // not yet played two games with opponent
    Outcome ltm = past->second.front(); // look at what was played two games ago
    Outcome result;
    if (ltm == Outcome::Defect) // in case it was defect, we need to look at last game as well
        switch (past->second.front())
        {
            case Outcome::Defect:
                result = Outcome::Defect;
                break;
            case Outcome::Cooperate:
                result = Outcome::Cooperate;
                break;
        }
    else
        result = Outcome::Cooperate; // if it was cooperate, we know we will cooperate this game
    past->second.pop();
    return result;
}

void Titx2ForTat::feedback(Network::Node &node, Outcome last) {
    history[&node].push(last);
}

void Titx2ForTat::clearMemory() {
    history.clear();
}

StrategyList Titx2ForTat::getName() {
    return this->name;
}

// SuspiciousTitForTat Strategy Methods: -----------------------------------------------------------------
Outcome SuspiciousTitForTat::play(Network::Node &node) {
    std::map<Network::Node*,Outcome>::iterator past = history.find(&node);
    if (past == history.end())
        return Outcome::Defect; // if no history with opponent, then initially defect
    switch (past->second) {
        case Outcome::Cooperate:
            return Outcome::Cooperate; // if cooperated last game, continue cooperation
        case Outcome::Defect:
            return Outcome::Defect; // if defected last game, then return fire
    }
}

void SuspiciousTitForTat::feedback(Network::Node &node, Outcome last) {
    history[&node] = last;
}

void SuspiciousTitForTat::clearMemory() {
    history.clear();
}

StrategyList SuspiciousTitForTat::getName() {
    return this->name;
}






// Functions to produce initial strategy arrays.
std::map<StrategyMethodPnt, std::string> StrategyGeneratorNames = {{Strategy::centreT4T,"CentreT4T"},
                                                                {Strategy::centre,"centre"},
                                                                {Strategy::random,"random"},
//                                                                {Strategy::randomBlocks,"randomBlocks"},
                                                                {Strategy::random_NonST4T,"random_NonSusT4T"},
                                                                {Strategy::random_threshold_DefT4T,"random_threshold_DefT4T"}};
double Strategy::radius = 1;
StrategyList Strategy::centreStrat;
StrategyList Strategy::surroundingsStrat;
boost::random::mt19937 Strategy::gen;
void Strategy::SetRadius(double newRadius){ radius = newRadius; }
void Strategy::SetCentreStrat(StrategyList newStrat){ centreStrat = newStrat; }
void Strategy::SetSurroundingsStrat(StrategyList newStrat){ surroundingsStrat = newStrat; }
void Strategy::SetRNG(boost::random::mt19937 newRNG){ gen = newRNG; }

Strategy* Strategy::centreT4T(std::pair<int,int> coord, std::pair<int,int> dimensions) {
    int radius2 = 3;
    int x = coord.first;
    int y = coord.second;

    if(x*x<radius2 && y*y<radius2)
        return new AlwaysDefect;
    else
        return new TitForTat;
}

Strategy* Strategy::centre(std::pair<int,int> coord, std::pair<int,int> dimensions) {
    double radius2 = radius;
    double x = ((double) coord.first) + 0.5 - ((double) dimensions.first)/2;
    double y = ((double) coord.second) + 0.5 - ((double) dimensions.second)/2;

    if((double) (x*x + y*y)<radius2 * radius2)
        return GetStrategyFromStrategyList(centreStrat);
    return GetStrategyFromStrategyList(surroundingsStrat);
}

Strategy* Strategy::random(std::pair<int,int>, std::pair<int,int>) {
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
Strategy* Strategy::randomBlocks(std::pair<int,int> position, std::pair<int,int> dimensions){
    int blocksize = 5;
    int blockArray[dimensions.first/blocksize +1][dimensions.second/blocksize +1] = {0};
    for (int i = 0;i != dimensions.first/blocksize +1; i++){
        for (int j = 0;i != dimensions.second/blocksize +1; i++){
        }
    }
    return new AlwaysCooperate;
}
*/

Strategy* Strategy::random_NonST4T(std::pair<int,int>, std::pair<int,int>) {
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

Strategy* Strategy::random_threshold_DefT4T (std::pair<int,int>, std::pair<int,int>) {
    boost::random::uniform_01 <boost::random::mt19937> dist(gen);
    if (dist() < 0.3)
        return new TitForTat;
    else
        return new AlwaysDefect;
}