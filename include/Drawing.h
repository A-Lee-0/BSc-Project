//
// Created by Samuel Kirby and Andrew Lee on 22/2/17.
//

#ifndef SGCB_DRAWING_H
#define SGCB_DRAWING_H

#include <Magick++.h>
#include "Strategy.h"
#include "bitmap_image.hpp"

extern std::map<StrategyList, rgb_t> strategyColours;

void SaveBitmap(Network &network, std::string filename);

void CreateGif(std::string dir,std::string filename);
void CreateGif(std::string dir,std::string filename, short frametime);

inline bool FileExists (const std::string& name);

#endif //SGCB_DRAWING_H
