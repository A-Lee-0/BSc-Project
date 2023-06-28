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
