//
// Created by Andrew on 28/06/2023.
//

#ifndef SGCB_EXPERIMENT_H
#define SGCB_EXPERIMENT_H

#include <boost/any.hpp>
#include <boost/ref.hpp>
#include <boost/bimap.hpp>
#include <boost/assign.hpp>
#include <map>
#include "Update.h"
#include "Strategy.h"


#define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember))
#define READ_MEMBER_VAR_MAP(object,objectMap,key)  (object->*(object->objectMap.at(key)))

enum class ExperimentList {IncrementGamesPerRound, RepeatExperiment};

class Experiment;
typedef void (Experiment::*ExperimentMemberSetFn)(boost::any);
typedef std::string (Experiment::*ExperimentMemberGetFn)();
typedef std::string (Experiment::*ExperimentMemberGetParamFn)(std::string);

typedef int (Experiment::*pInt);
typedef bool (Experiment::*pBool);


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
    virtual std::string getDescription();
    void AddParameter(std::string newParamName, boost::any newParam);
    void SetParameter(std::string paramName, boost::any newParamValue);
    virtual std::string GetParameter(std::string paramName);
    virtual std::string GetParameter(ParameterName);
    std::string _GetFoundParameter(ParameterName);
    int _SetFoundParameter(ParameterName, std::string);
    int _SetFoundInt(ParameterName param, int newValue);
    int _SetFoundBool(ParameterName param, bool newValue);

    std::map<ParameterName,pInt> intMap;
    std::map<ParameterName,bool Experiment::*> boolMap;

    virtual std::vector<ParameterName> ListParameters();

    std::string dummyGetFn();
    std::map<std::string,ExperimentMemberSetFn> setMethodMap;
    std::map<std::string,ExperimentMemberGetFn> getMethodMap;

    paramBimap parameterEnumBimap;
protected:
    const std::vector<ParameterName> parameters = {};
    std::string name = "Stub Experiment";
    std::string description = "Stub Experiment Description";
    // create 'dummmy' variables in Experiment, to allow them to be referenced safely by general Experiment class funcs
    int maxGPR;
    int GPR;
    int numRounds;
    int networkSize;
    int repeatExperiments;

    bool saveBitmaps = false;
    bool returnGlobalData = true;
    StrategyMethod initialStrategiesMethod;
    UpdateList updateType;

};



class IncrementGamesPerRound : public Experiment {
public:
    IncrementGamesPerRound();
    void Run() override;
    std::map<std::string, int IncrementGamesPerRound::*> intParameters;

    virtual std::string GetParameter(std::string paramName);
    virtual std::string GetParameter(ParameterName);

    void SetIntParameter(std::string paramName,int newParamValue);
    void SetUpdateType(boost::any newUpdateType);
    std::string GetUpdateType();
    void SetInitialStrategiesMethod(boost::any newInitialStrategiesMethod);
    std::string GetInitialStrategiesMethod();

    std::vector<ParameterName> ListParameters();

    int maxGPR;
    int numRounds;
    int networkSize;
    int repeatExperiments;
protected:
    const std::vector<ParameterName> parameters = {ParameterName::MaxGPR,
                                                   ParameterName::NumRounds,
                                                   ParameterName::NetworkSize,
                                                   ParameterName::RepeatExperiments,
                                                   ParameterName::SaveBitmaps,
                                                   ParameterName::ReturnGlobalData,
                                                   ParameterName::InitialStrategiesMethod,
                                                   ParameterName::UpdateType};
    StrategyMethod initialStrategiesMethod;
    UpdateList updateType;
};

class RepeatExperiment : public Experiment {
public:
    RepeatExperiment();
    void Run() override;
    std::map<std::string, int RepeatExperiment::*> intParameters;

    virtual std::string GetParameter(std::string paramName);
    virtual std::string GetParameter(ParameterName);

    void SetIntParameter(std::string paramName,int newParamValue);
    void SetUpdateType(boost::any newUpdateType);
    std::string GetUpdateType();
    void SetInitialStrategiesMethod(boost::any newInitialStrategiesMethod);
    std::string GetInitialStrategiesMethod();

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

