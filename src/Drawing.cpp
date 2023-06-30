//
// Created by Samuel Kirby and Andrew Lee on 22/2/17.
//
#include <boost/filesystem.hpp>

#include "Drawing.h"
#include "Strategy.h"

std::map<StrategyList, rgb_t> strategyColours = {{StrategyList::AlwaysDefect, rgb_t {255,0,0}},             //red
                                                 {StrategyList::AlwaysCooperate, rgb_t {0,0,255}},          //blue
                                                 {StrategyList::TitForTat, rgb_t {0,255,0}},                //green
                                                 {StrategyList::Titx2ForTat, rgb_t {0,255,255}},            //cyan
                                                 {StrategyList::SuspiciousTitForTat, rgb_t {255,0,255}}};   //magenta

void SaveBitmap(Network &network, std::string filename) {
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

void CreateGif(std::string dir,std::string filename){
    CreateGif(dir, filename, 30);
}
/// Creates a gif from the output bmps of an experiment.
/// \param dir - the directory the bmps are stored. Can be relative to the built executable
/// \param filename - the name to give the new .gif, e.g. "anim.gif"
/// \param frametime - the time to display each frame in the animation, in 1/100 of a second.
void CreateGif(std::string dir,std::string filename, short frametime){
    //Determine how many bmps we're dealing with.

    int maxFile = -1;
    std::string bmpName;
    bool searching = true;

    while (searching){
        bmpName = "round_" + std::to_string(maxFile + 1) + ".bmp";
        if (FileExists(dir + "\\" + bmpName)){
            maxFile += 1;
        }
        else{
            searching = false;
        }
    }
    if (maxFile == -1){
        std::cout << "No images were found to construct a gif!" << std::endl;
        return;
    }
    //std::cout << "Found "<< maxFile << " files to construct a gif!" << std::endl;

    Magick::Image image;
    std::list<Magick::Image> imageList;
    try {
        // load bmps into imageList
        for (int i = 0; i <= maxFile; i++) {
            bmpName = "round_" + std::to_string(i) + ".bmp";
            image.read(dir + "\\" + bmpName);
            image.animationDelay(frametime);
            imageList.push_back(image);
        }

        //write imageList out as a new .gif
        //Magick::Image bmpStack;
        //appendImages( &bmpStack, imageList.begin(), imageList.end() );
        //bmpStack.write( dir + "\\" + filename );

        Magick::writeImages( imageList.begin(), imageList.end(), dir + "\\" + filename );
    }
    catch( Magick::Exception &error_ )
    {
        std::cout << "Caught exception: " << error_.what() << std::endl;
        return;
    }
    return ;


};

inline bool FileExists (const std::string& name) {
    std::ifstream f(name.c_str());
    return f.good();
}
