//
// Created by Samuel Kirby and Andrew Lee on 2/2/17.
//

#ifndef SGCB_NETWORK_H
#define SGCB_NETWORK_H

#include <tuple>
#include <vector>
#include <map>
#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem.hpp>
#undef BOOST_NO_CXX11_SCOPED_ENUMS

class Strategy;
class Update;

enum class StrategyList {AlwaysDefect, AlwaysCooperate, TitForTat, Titx2ForTat, SuspiciousTitForTat};
enum class UpdateList {L_Threshold, L_MostWins, SL_Interrogate, SL_ThresholdScoreTB, G_ThresholdGDPTB, G_ThresholdGPPTB, G_GDP};

enum class Outcome {Cooperate, Defect};

//Alias functions to produce grids of strategies and updateStrategies.
typedef Strategy* StrategyMethod(std::pair<int, int>, std::pair<int, int>);
typedef StrategyMethod* StrategyMethodPnt;
typedef Update* UpdateMethod(std::pair<int, int>, std::pair<int, int>,UpdateList);
typedef UpdateMethod* UpdateMethodPnt;


/*
 * Network class
 * The Network class stores a collection of nodes in a map of 2D integer cartesian coordinates
 * Edges are stored as a pair of pointers to the nodes at either end.
 * When retrieving a list of all nodes or all edges, note that it is read-only. Any modifications to nodes or edges
 * must be made through public member functions.
 */
class Network {
public:
    Network(int, int, StrategyList);

    Network(int, int, StrategyMethodPnt);

    Network(int, int, StrategyMethodPnt, UpdateList, std::string subDir);

    Network(int, int, StrategyMethodPnt, UpdateMethodPnt, std::string subDir, UpdateList defaultUpdateType);

    Network(int, int, StrategyMethodPnt, UpdateMethodPnt, std::string subDir, UpdateList defaultUpdateType, int blocksize);

    Network(std::string filepath);

    /*
     * Node class
     * The Node class is a wrapper class for the strategy of the agent at that coordinate. It allows the strategy to
     * be changed without requiring the edges to be reconstructed. It also contains the current score of that agent.
     * Strategy and score are private members and must be accessed and updated using public member functions.
     */
    class Node {
    public:
        Node(int,int,Strategy*,Network*);

        Node(Strategy*);

        const std::pair<int,int> getCoord() const;

        Outcome play(Node*);

        void feedback(Network::Node *opNode, Outcome choice, Outcome opChoice, int score, int opScore);

        void updateStrategy(StrategyList);

        void backupStrategy();

        StrategyList getCurrentStrategy();
        StrategyList getOldStrategy();

        Update* getUpdateType();
        void setUpdateType(Update*);
        void setUpdateType(UpdateList);
        void updateStrategy();

        void clearMemory() const;

        int getScore() const;
        std::map<Node*,int> &getOpScore();

        int getWins() const;
        std::map<Node*,int> &getOpWins();

        Network* getNetwork();

        void resetScore();
    private:
        int x;
        int y;
        Strategy* strategy;
        Strategy* oldStrategy;
        int score = 0;
        int wins = 0;
        std::map<Node*,int> opScore;
        std::map<Node*,int> opWins;
        Update* updateType;
        Network* network;
    };


    std::map<std::pair<int, int>, Node> &getNodeMap();

    const std::vector<std::pair<Node *, Node *>> &getEdges() const;

    Network::Node &getNodeByCoord(std::pair<int, int>);

    const std::pair<int,int> getDimensionality() const;

    std::string getSubDir();

    void setSubDir(std::string newSubDir);

    void connectNearestNeighbours();

    void writeToFile(std::string filename, bool withEdges = false);

    void exportNetwork(std::string filename);

    void resetNodeScores();

    void generateGlobalStats();

    std::map<StrategyList, double> &getGlobalStats();
    std::map<StrategyList, int> &getGlobalPopulation();
    std::map<StrategyList, double> &getGlobalMeanScorePerNode();
    std::map<StrategyList, double> &getGlobalMeanScorePerEdge();
    std::map<StrategyList, int> &getGlobalTotalScore();

private:
    std::map<std::pair<int,int>, Node> nodeMap;
    std::vector<std::pair<Node*, Node*>> edges;
    std::map<StrategyList,double> globalStats;
    std::map<StrategyList,int> globalPopulation;
    std::map<StrategyList,double> globalMeanScorePerNode;
    std::map<StrategyList,double> globalMeanScorePerEdge;
    std::map<StrategyList,int> globalTotalScore;
    int xDim;
    int yDim;
    std::string subDir;
};


#endif //SGCB_NETWORK_H
