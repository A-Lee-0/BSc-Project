//
// Created by Samuel Kirby and Andrew Lee on 04/02/2017.
//

#ifndef SGCB_STRATEGY_H
#define SGCB_STRATEGY_H

#include "Network.h"
#include <queue>
#include <boost/random.hpp>

Strategy * GetStrategyFromStrategyList(StrategyList name);

extern std::map<std::string, StrategyList> strategyStrings;
extern std::map<StrategyList, std::string> StrategyListStrings;
extern std::map<StrategyList, int> intStrategies;

class Strategy {
public:
    virtual Outcome play(Network::Node &);
    virtual void feedback(Network::Node &, Outcome);
    virtual void clearMemory();
    virtual StrategyList getName();

    // Functions to produce initial strategy arrays.
    static Strategy *centreT4T(std::pair<int, int> coord, std::pair<int, int> dimensions);
    static Strategy *centre(std::pair<int, int> coord, std::pair<int, int> dimensions);
    static Strategy *random(std::pair<int, int> coord, std::pair<int, int> dimensions);
    static Strategy *random_NonST4T(std::pair<int, int> coord, std::pair<int, int> dimensions);
    static Strategy *rdT4T(std::pair<int, int> coord, std::pair<int, int> dimensions);

    static void SetRadius(double newRadius);
    static void SetCentreStrat(StrategyList newStrat);
    static void SetSurroundingsStrat(StrategyList newStrat);
    static void SetRNG(boost::random::mt19937 newRNG);
private:
    static double radius;
    static StrategyList centreStrat;
    static StrategyList surroundingsStrat;
    static boost::random::mt19937 gen;
};

class AlwaysDefect : public Strategy {
public:
    Outcome play(Network::Node &node) override;
    void feedback(Network::Node &, Outcome) override;
    void clearMemory() override;
    StrategyList getName() override;

private:
    const StrategyList name = StrategyList::AlwaysDefect;
};

class AlwaysCooperate : public Strategy {
public:
    Outcome play(Network::Node &node) override;
    void feedback(Network::Node &, Outcome) override;
    void clearMemory() override;
    StrategyList getName() override;

private:
    const StrategyList name = StrategyList::AlwaysCooperate;
};

class TitForTat : public Strategy {
public:
    Outcome play(Network::Node &node) override;
    void feedback(Network::Node &, Outcome) override;
    void clearMemory() override;
    StrategyList getName() override;


private:
    std::map<Network::Node*,Outcome> history;
    const StrategyList name = StrategyList::TitForTat;
};

class Titx2ForTat : public Strategy {
public:
    Outcome play(Network::Node &node) override;
    void feedback(Network::Node &, Outcome) override;
    void clearMemory() override;
    StrategyList getName() override;


private:
    std::map<Network::Node*,std::queue<Outcome>> history;
    const StrategyList name = StrategyList::Titx2ForTat;
};

class SuspiciousTitForTat : public  Strategy {
public:
    Outcome play(Network::Node &node) override;
    void feedback(Network::Node &node, Outcome outcomes) override;
    void clearMemory() override;
    StrategyList getName() override;


private:
    std::map<Network::Node*,Outcome> history;
    const StrategyList name = StrategyList::SuspiciousTitForTat;
};





#endif //SGCB_STRATEGY_H