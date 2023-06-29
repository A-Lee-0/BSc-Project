//
// Created by Andrew Lee on 28/06/2023.
//

#include "Experiment.h"
#include <boost/progress.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include "Game.h"
//#include <ctime>
#include <typeinfo>

std::map<std::string, ExperimentList> experimentNames = {{"IncrementGamesPerRound", ExperimentList::IncrementGamesPerRound},
                                                         {"RepeatExperiment", ExperimentList::RepeatExperiment}};


std::vector<std::string> ListParameterNames(Experiment * experiment){
    return ListParameterNames(*experiment, (*experiment).ListParameters());
}
std::vector<std::string> ListParameterNames(Experiment experiment, std::vector<ParameterName> paramList){
    std::vector<std::string> out;
    for(ParameterName param : paramList){
        auto name = experiment.parameterEnumBimap.right.at(param);
        out.push_back(experiment.parameterEnumBimap.right.at(param));
    }
    return out;
}

Experiment::Experiment(){
//    testP3 = &Experiment::repeatExperiments;
//    testPInt = &Experiment::repeatExperiments;
    intMap = {{ParameterName::MaxGPR,&Experiment::maxGPR},
              {ParameterName::GPR,&Experiment::GPR},
              {ParameterName::NumRounds,&Experiment::numRounds},
              {ParameterName::NetworkSize,&Experiment::networkSize},
              {ParameterName::RepeatExperiments,&Experiment::repeatExperiments}};
    boolMap = {{ParameterName::ReturnGlobalData, &Experiment::returnGlobalData},
               {ParameterName::SaveBitmaps, &Experiment::saveBitmaps}};
    parameterEnumBimap.left.insert(parameterEnumMap.begin(), parameterEnumMap.end());
}


Experiment* experimentFromExperimentType(ExperimentList experimentType) {
    switch (experimentType) {
        case ExperimentList::IncrementGamesPerRound:
            return new IncrementGamesPerRound();
        case ExperimentList::RepeatExperiment:
            return new RepeatExperiment();
    }
}

std::string Experiment::getName() {
    return this->name;
}
std::string Experiment::getDescription() {
    return this->description;
}
void Experiment::Run() {
    throw std::logic_error("using stub experiment!");
}

void Experiment::SetParameter(std::string paramName, boost::any newParamValue) {
    //std::function<void(boost::any)> method = this->setMethodMap[paramName];
    //method(newParamValue);
    //this->setMethodMap[paramName](newParamValue);

    //auto method = this->setMethodMap[paramName];
    //method(newParamValue);
    (this->*setMethodMap[paramName])(newParamValue);

}

std::string Experiment::GetParameter(std::string paramName) { return "Dummy string"; };
std::string Experiment::GetParameter(ParameterName param) { return "Parameter " + parameterEnumBimap.right.find(param)->second  + " does not exist for experiment type" + getName(); };


std::string Experiment::dummyGetFn(){ return "Dummy string";}

std::vector<ParameterName> Experiment::ListParameters(){
    return parameters;
};



//IncrementGamesPerRound
IncrementGamesPerRound::IncrementGamesPerRound(){
    name = "IncrementGamesPerRound";
    description =
            "This experiment runs the same iterated Prisoner's Dilemma repeatedly, with the same initial network. However,"
                    "it increments the number of Games per Round by 1 each time, up to a specified maximum. This allows us to"
                    "investigate how effective strategies that change between rounds are."
                    "";
    std::map<std::string, int IncrementGamesPerRound::*> intParameters;
    setMethodMap["SetUpdateType"] = SetUpdateType;
    getMethodMap["GetUpdateType"] = GetUpdateType;
    setMethodMap["SetInitialStrategiesMethod"] = SetInitialStrategiesMethod;
    getMethodMap["GetInitialStrategiesMethod"] = GetInitialStrategiesMethod;

    intParameters["GPR"] = &IncrementGamesPerRound::maxGPR;
    intParameters["numRounds"] = &IncrementGamesPerRound::numRounds;
    intParameters["networkSize"] = &IncrementGamesPerRound::networkSize;

    maxGPR = 15;
    numRounds = 50;
    networkSize = 100;
    updateType = UpdateList::G_GDP;
    initialStrategiesMethod = Strategy::random;

}

void IncrementGamesPerRound::Run(){
    boost::progress_display GPRProg(maxGPR);

    std::string updateString = UpdateListStrings[updateType];
    for (int j = 1; j != repeatExperiments; j++) {
        for (int i = 1; i != maxGPR; i++) {
            std::string filepath = name + "gprTest_" + updateString + "_CM_auto\\random_" + std::to_string(i) + "gpr";
            Network network = Network(networkSize, networkSize, initialStrategiesMethod, updateType, filepath);
            network.connectNearestNeighbours();
            playNRounds(network, numRounds, i, saveBitmaps, returnGlobalData);
            ++GPRProg;
        }
    }
}
void IncrementGamesPerRound::SetUpdateType(boost::any newUpdateType){ updateType = boost::any_cast<UpdateList>(newUpdateType); }
std::string IncrementGamesPerRound::GetUpdateType(){ return UpdateListStrings[updateType]; }
void IncrementGamesPerRound::SetInitialStrategiesMethod(boost::any newInitialStrategiesMethod){ initialStrategiesMethod = boost::any_cast<StrategyMethod>(newInitialStrategiesMethod); }
std::string IncrementGamesPerRound::GetInitialStrategiesMethod(){ return StrategyGeneratorNames[initialStrategiesMethod]; }
void IncrementGamesPerRound::SetIntParameter(std::string paramName,int newParamValue){
    auto variable = intParameters[paramName];
    this->*variable = newParamValue;
}

std::string IncrementGamesPerRound::GetParameter(std::string paramName){
    return GetParameter(parameterEnumMap.at(paramName));
}

std::string IncrementGamesPerRound::GetParameter(ParameterName param){
    if (std::find(parameters.begin(), parameters.end(), param) != parameters.end()){
        return _GetFoundParameter(param);
    }
    else {
        return "Parameter " + parameterEnumBimap.right.find(param)->second + " does not exist for experiment type" + getName();
    }
};

std::vector<ParameterName> IncrementGamesPerRound::ListParameters(){
    return parameters;
};



RepeatExperiment::RepeatExperiment(){
    name ="RepeatExperiment";
    GPR = 15;
    numRounds = 50;
    networkSize = 100;
    repeatExperiments = 200;
    updateType = UpdateList::SL_ThresholdScoreTB;
    initialStrategiesMethod = Strategy::random;
}

void RepeatExperiment::Run(){
    std::string updateString = UpdateListStrings[updateType];
    time_t t = time(0);   // get time now
    struct tm * now = localtime( & t );
    char datetime[20];
    strftime(datetime,20,"%Y-%m-%d %H%M",now);
    std::string newDirectory = std::string(datetime) + "\\" + name;
    boost::progress_display repeatsProg(repeatExperiments);
    for (int i = 0; i != repeatExperiments; i++) {
        //std::cout << updateString << i << std::endl;

        std::string filepath = newDirectory + "\\iter" + std::to_string(i);
        Network network = Network(networkSize, networkSize, initialStrategiesMethod, updateType, filepath);
        network.connectNearestNeighbours();
        playNRounds(network, numRounds, GPR, saveBitmaps, returnGlobalData);
        ++repeatsProg;
    }
}

void RepeatExperiment::SetUpdateType(boost::any newUpdateType){ updateType = boost::any_cast<UpdateList>(newUpdateType); }
std::string RepeatExperiment::GetUpdateType(){ return UpdateListStrings[updateType]; }
void RepeatExperiment::SetInitialStrategiesMethod(boost::any newInitialStrategiesMethod){ initialStrategiesMethod = boost::any_cast<StrategyMethod>(newInitialStrategiesMethod); }
std::string RepeatExperiment::GetInitialStrategiesMethod(){ return StrategyGeneratorNames[initialStrategiesMethod]; }
void RepeatExperiment::SetIntParameter(std::string paramName,int newParamValue){
    //auto variable = intParameters[paramName];
    (this->*intParameters[paramName]) = newParamValue;
}


std::string RepeatExperiment::GetParameter(std::string paramName){
    return GetParameter(parameterEnumMap.at(paramName));
}

std::string RepeatExperiment::GetParameter(ParameterName param){
    if (std::find(parameters.begin(), parameters.end(), param) != parameters.end()){
        return _GetFoundParameter(param);
    }
    else {
        return "Parameter " + parameterEnumBimap.right.find(param)->second + " does not exist for experiment type" + getName();
    }
};


std::vector<ParameterName> RepeatExperiment::ListParameters(){
    return parameters;
};






std::string Experiment::_GetFoundParameter(ParameterName param){
    switch (param) {
        case ParameterName::UpdateType:
            return UpdateListStrings[updateType];
        case ParameterName::InitialStrategiesMethod:
            return StrategyGeneratorNames[initialStrategiesMethod];
        case ParameterName::NumRounds:
            return std::to_string(numRounds);
        case ParameterName::MaxGPR:
            return std::to_string(maxGPR);
        case ParameterName::GPR:
            return std::to_string(GPR);
        case ParameterName::NetworkSize:
            return std::to_string(networkSize);
        case ParameterName::RepeatExperiments:
            return std::to_string(repeatExperiments);
        case ParameterName::ReturnGlobalData:
            return returnGlobalData ? "true" : "false";
        case ParameterName::SaveBitmaps:
            return saveBitmaps ? "true" : "false";
        default:
            return "Shouldn't see this! (Experiment::_GetFoundParameter)";
    }
}

int Experiment::_SetFoundParameter(ParameterName param, std::string newValue){
    // Handle ints in bulk, as there're a lot of them!
    if (ParameterType.at(param) == "int"){
        std::cout << "param is int!" << std::endl;
        try{
            int newInt = boost::lexical_cast<int>(newValue);
            return _SetFoundInt(param,newInt);
        }
        catch(...){
            return 1;
        }
    }
    // Handle bools
    if (ParameterType.at(param) == "bool"){
        try{
            bool newBool;
            boost::to_lower(newValue);
            if(newValue == "1"|| newValue == "true") {
                newBool = true;
            }
            else if(newValue == "0"|| newValue == "false"){
                newBool = false;
            }
            else {
                return 1;
            }
            return _SetFoundBool(param,newBool);
        }
        catch(...){
            return 1;
        }
    }


    switch (param) {
        case ParameterName::UpdateType:
            for (auto &i : UpdateListStrings) {
                if (i.second == newValue){
                    updateType = i.first;
                    return 0;
                }
            }
            return 1;
        case ParameterName::InitialStrategiesMethod:
            for (auto &i : StrategyGeneratorNames) {
                if (i.second == newValue){
                    initialStrategiesMethod = i.first;
                    return 0;
                }
            }
            return 1;
        default:
            return 1;//"Shouldn't see this!";
    }
}
int Experiment::_SetFoundInt(ParameterName param, int newValue){
    if (ParameterType.at(param) == "int"){
        try{
            this->*intMap.at(param) = newValue;
            return 0;
        }
        catch(...){
            return 1;
        }
    }
    else {
        return 1;
    }
}

int Experiment::_SetFoundBool(ParameterName param, bool newValue){
    if (ParameterType.at(param) == "bool"){
        try{
            this->*boolMap.at(param) = newValue;
            return 0;
        }
        catch(...){
            return 1;
        }
    }
    else {

        return 1;
    }
}
