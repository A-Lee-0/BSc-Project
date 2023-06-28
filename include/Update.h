//
// Created by AndrewLee on 19-Mar-17.
//

#ifndef SGCB_UPDATE_H
#define SGCB_UPDATE_H

#include "Network.h"
#include <queue>


extern std::map<UpdateList, std::string> UpdateListStrings;

Update * updateFromUpdateType(UpdateList updateType);

class Update {
public:
    virtual void update(Network::Node &);
    virtual UpdateList getName();
};

class L_Threshold : public Update {
public:
    void update(Network::Node &) override;
    UpdateList getName() override;

private:
    const UpdateList name = UpdateList::L_Threshold;
    const double threshold = 0.5;
};

class L_MostWins : public Update {
public:
    void update(Network::Node &) override;
    UpdateList getName() override;

private:
    const UpdateList name = UpdateList::L_MostWins;
    const double threshold = 0.5;
};

class SL_Interrogate : public Update {
public:
    void update(Network::Node &) override;
    UpdateList getName() override;

private:
    const UpdateList name = UpdateList::SL_Interrogate;
};

class SL_ThresholdScoreTB : public Update {
public:
    void update(Network::Node &) override;
    UpdateList getName() override;

private:
    const UpdateList name = UpdateList::SL_Interrogate;
    const double threshold = 0.5;
};

class G_ThresholdGDPTB : public Update {
public:
    void update(Network::Node &) override;
    UpdateList getName() override;

private:
    const UpdateList name = UpdateList::G_ThresholdGDPTB;
    const double threshold = 0.5;
};

class G_ThresholdGPPTB : public Update {
public:
    void update(Network::Node &) override;
    UpdateList getName() override;

private:
    const UpdateList name = UpdateList::G_ThresholdGPPTB;
    const double threshold = 0.5;
};

class G_GDP : public Update {
public:
    void update(Network::Node &) override;
    UpdateList getName() override;

private:
    const UpdateList name = UpdateList::G_GDP;
};

#endif //SGCB_UPDATE_H
