//
// Created by Andrew on 28/06/2023.
//

#ifndef SGCB_EXPERIMENTS_H
#define SGCB_EXPERIMENTS_H

#endif //SGCB_EXPERIMENTS_H

#include "Update.h"

//enum class ExperimentList {LoopOneUpdateType};

class Experiment {
public:
    virtual void run();
    virtual std::string getName();
private:
    std::string name = "Stub Experiment";
};


//Loop through one updateType
class LoopOneUpdateType : public Experiment {
public:
    void run() override;
private:
    std::string name = "LoopOneUpdateType";

    UpdateList updateType = UpdateList::G_GDP;
    int maxGPR = 15;
    int networkSize = 100;
};