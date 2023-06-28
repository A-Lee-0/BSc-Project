//
// Created by Samuel Kirby and Andrew Lee on 22/2/17.
//
#include <boost/filesystem.hpp>

#include "Drawing.h"
#include "bitmap_image.hpp"
#include "Strategy.h"

std::map<StrategyList, rgb_t> strategyColours = {{StrategyList::AlwaysDefect, rgb_t {255,0,0}},             //red
                                                 {StrategyList::AlwaysCooperate, rgb_t {0,0,255}},          //blue
                                                 {StrategyList::TitForTat, rgb_t {0,255,0}},                //green
                                                 {StrategyList::Titx2ForTat, rgb_t {0,255,255}},            //cyan
                                                 {StrategyList::SuspiciousTitForTat, rgb_t {255,0,255}}};   //magenta

void saveBitmap(Network &network, std::string filename) {
    char * charFilePath = (char*) network.getSubDir().c_str();
    char * charFilename = (char*) filename.c_str();
    boost::filesystem::path dir(charFilePath);
    boost::filesystem::create_directory(dir);
    bitmap_image networkImage(network.getDimensionality().first,network.getDimensionality().second);
    for (auto node = network.getNodeMap().begin(); node != network.getNodeMap().end(); node++) {
        networkImage.set_pixel(node->first.first,node->first.second,strategyColours[node->second.getCurrentStrategy()]);
    }
    networkImage.save_image((char*) (network.getSubDir() + "\\" + filename).c_str());
}

