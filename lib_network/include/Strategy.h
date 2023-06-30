//
// Created by Samuel Kirby and Andrew Lee on 04/02/2017.
//

#ifndef SGCB_STRATEGY_H
#define SGCB_STRATEGY_H

#include "Network.h"
#include <queue>
#include <boost/random.hpp>
#include <map>

class Strategy;
enum class Outcome {Cooperate, Defect};

//Alias functions to produce grids of strategies and updateStrategies.
typedef Strategy* StrategyMethod(std::pair<int, int>, std::pair<int, int>);
typedef StrategyMethod* StrategyMethodPnt;

enum class StrategyList {AlwaysDefect, AlwaysCooperate, TitForTat, Titx2ForTat, SuspiciousTitForTat};
Strategy * GetStrategyFromStrategyList(StrategyList name);

extern std::map<std::string, StrategyList> strategyStrings;
extern std::map<StrategyList, std::string> StrategyListStrings;
extern std::map<StrategyList, int> intStrategies;

extern std::map<StrategyMethodPnt, std::string> StrategyGeneratorNames;

class Strategy {
public:
    virtual Outcome play(Network::Node &);
    virtual void feedback(Network::Node &, Outcome);
    virtual void clearMemory();
    virtual StrategyList getName();

    // Functions to produce initial strategy arrays.
    static StrategyMethod centreT4T;
    static StrategyMethod centre;
    static StrategyMethod random;
//    static StrategyMethod randomBlocks;
    static StrategyMethod random_NonST4T;
    static StrategyMethod random_threshold_DefT4T;

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