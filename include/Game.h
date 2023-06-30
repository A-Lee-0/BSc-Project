//
// Created by Samuel Kirby and Andrew Lee on 22/2/17.
//

#ifndef SGCB_GAME_H
#define SGCB_GAME_H

#include <map>

#include <Network.h>
#include "Drawing.h"

enum class StrategyList;

extern std::map<std::pair<Outcome, Outcome>, std::pair<int, int>> payoff;

void playNRounds(Network& network, int N, int gamesPerRound, bool saveBitmaps = false, bool returnGlobalData = false);
void iterate(Network & network, int gamesPerRound = 5);
void updateNetwork(Network& network, double threshold = 0.5);
void updateNetworkWithGDPTieBreak(Network& network, double threshold = 0.5);
void updateNetworkByInterrogation(Network & network);
void updateNetworkByNode(Network & network);
void prepareNetwork(Network& network);

void saveIntArray(char* filename, int intArray);

void createDirectory(std::string path);

#endif //SGCB_GAME_H
