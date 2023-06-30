//
// Created by Andrew on 28/06/2023.
//

#ifndef SGCB_EXPERIMENT_H
#define SGCB_EXPERIMENT_H

#include <boost/bimap.hpp>
#include <map>
#include <Network.h>

#define READ_MEMBER_VAR_MAP(object,objectMap,key)  (object->*(object->objectMap.at(key)))

enum class ExperimentList {IncrementGamesPerRound, RepeatExperiment};
static std::map<std::string, ExperimentList> experimentNameMap = {{"IncrementGamesPerRound", ExperimentList::IncrementGamesPerRound},
                                                           {"RepeatExperiment", ExperimentList::RepeatExperiment}};

class Experiment;

Experiment * CreateExperiment(ExperimentList);
void PrintParameterTable(Experiment *);

enum class ParameterName {
    NumRounds,
    NetworkSize,
    RepeatExperiments,
    UpdateType,
    InitialStrategiesMethod,
    SaveBitmaps,
    ReturnGlobalData,
    MaxGPR,
    GPR};

enum class ConsoleReturn {Success, UnknownFailure, UnknownParamString, NotApplicableParam, InvalidNewValue, InvalidType};

const std::map<ConsoleReturn,std::string> ConsoleReturnString {{ConsoleReturn::Success,"Success"},
                                                               {ConsoleReturn::UnknownFailure,"UnknownFailure"},
                                                               {ConsoleReturn::UnknownParamString,"UnknownParamString"},
                                                               {ConsoleReturn::NotApplicableParam,"NotApplicableParam"},
                                                               {ConsoleReturn::InvalidNewValue,"InvalidNewValue"},
                                                               {ConsoleReturn::InvalidType,"InvalidType"}};


const std::map<std::string, ParameterName> parameterEnumMap = {{"NumRounds",ParameterName::NumRounds},
                                                               {"NetworkSize",ParameterName::NetworkSize},
                                                               {"RepeatExperiments",ParameterName::RepeatExperiments},
                                                               {"UpdateType",ParameterName::UpdateType},
                                                               {"InitialStrategiesMethod",ParameterName::InitialStrategiesMethod},
                                                               {"SaveBitmaps",ParameterName::SaveBitmaps},
                                                               {"ReturnGlobalData",ParameterName::ReturnGlobalData},
                                                               {"MaxGPR",ParameterName::MaxGPR},
                                                               {"GPR",ParameterName::GPR}};

const std::map<ParameterName,std::string> ParameterType = {{ParameterName::NumRounds,"int"},
                                                           {ParameterName::NetworkSize,"int"},
                                                           {ParameterName::RepeatExperiments,"int"},
                                                           {ParameterName::UpdateType,"UpdateList"},
                                                           {ParameterName::InitialStrategiesMethod,"StrategyMethod"},
                                                           {ParameterName::SaveBitmaps,"bool"},
                                                           {ParameterName::ReturnGlobalData,"bool"},
                                                           {ParameterName::MaxGPR,"int"},
                                                           {ParameterName::GPR,"int"}};

typedef boost::bimap<std::string, ParameterName> paramBimap;


std::vector<std::string> ListParameterNames(Experiment,std::vector<ParameterName>);
std::vector<std::string> ListParameterNames(Experiment*);


class Experiment {
public:
    Experiment();
    virtual void Run();
    std::string getName();
    std::string getDescription();
    virtual std::string GetParameter(std::string paramName);
    virtual std::string GetParameter(ParameterName);
    std::string _GetFoundParameter(ParameterName);

    ConsoleReturn SetParameter(std::string, std::string);
    ConsoleReturn _SetFoundParameter(ParameterName, std::string);
    ConsoleReturn _SetFoundInt(ParameterName param, int newValue);
    ConsoleReturn _SetFoundBool(ParameterName param, bool newValue);

    std::map<ParameterName,int Experiment::*> intMap;
    std::map<ParameterName,bool Experiment::*> boolMap;

    virtual std::vector<ParameterName> ListParameters();

    paramBimap parameterEnumBimap;
protected:
    const std::vector<ParameterName> parameters = {};
    std::string name = "Stub Experiment";
    std::string description = "Stub Experiment Description";


    int maxGPR;
    int GPR;
    int numRounds;
    int networkSize;
    int repeatExperiments;
    bool saveBitmaps = false;
    bool returnGlobalData = true;
    StrategyMethodPnt initialStrategiesMethod;
    UpdateList updateType;
};



class IncrementGamesPerRound : public Experiment {
public:
    IncrementGamesPerRound();
    void Run() override;

    virtual std::string GetParameter(std::string paramName);
    virtual std::string GetParameter(ParameterName);
    std::vector<ParameterName> ListParameters();

protected:
    const std::vector<ParameterName> parameters = {ParameterName::MaxGPR,
                                                   ParameterName::NumRounds,
                                                   ParameterName::NetworkSize,
                                                   ParameterName::RepeatExperiments,
                                                   ParameterName::SaveBitmaps,
                                                   ParameterName::ReturnGlobalData,
                                                   ParameterName::InitialStrategiesMethod,
                                                   ParameterName::UpdateType};
};

class RepeatExperiment : public Experiment {
public:
    RepeatExperiment();
    void Run() override;

    virtual std::string GetParameter(std::string paramName);
    virtual std::string GetParameter(ParameterName);
    std::vector<ParameterName> ListParameters();

protected:
    const std::vector<ParameterName> parameters = {ParameterName::GPR,
                                                   ParameterName::NumRounds,
                                                   ParameterName::NetworkSize,
                                                   ParameterName::RepeatExperiments,
                                                   ParameterName::SaveBitmaps,
                                                   ParameterName::ReturnGlobalData,
                                                   ParameterName::InitialStrategiesMethod,
                                                   ParameterName::UpdateType};
};

#endif //SGCB_EXPERIMENT_H

