//
// Created by Andrew Lee on 28/06/2023.
//

#include "Experiment.h"
#include <boost/progress.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include "Game.h"


Experiment * CreateExperiment(ExperimentList experiment) {
    switch (experiment) {
        case ExperimentList::IncrementGamesPerRound:
            return new IncrementGamesPerRound();
        case ExperimentList::RepeatExperiment:
            return new RepeatExperiment();
    }
}

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


std::string Experiment::GetParameter(std::string paramName) { return "Dummy string"; };
std::string Experiment::GetParameter(ParameterName param) { return "Parameter " + parameterEnumBimap.right.find(param)->second  + " does not exist for experiment type" + getName(); };

std::vector<ParameterName> Experiment::ListParameters(){
    return parameters;
};



//IncrementGamesPerRound
IncrementGamesPerRound::IncrementGamesPerRound(){
    name = "IncrementGamesPerRound";
    description =
            "This experiment runs the same iterated Prisoner's Dilemma repeatedly, with the same initial network. "
                    "However,it increments the number of Games per Round by 1 each time, up to a specified maximum. "
                    "This allows us to investigate how the efficacy of strategies that change between rounds changes "
                    "with the number of rounds.";
    maxGPR = 15;
    numRounds = 50;
    networkSize = 100;
    repeatExperiments = 1;
    updateType = UpdateList::G_GDP;
    initialStrategiesMethod = &Strategy::random;
}

void IncrementGamesPerRound::Run(){
    time_t t = time(0);   // get time now
    struct tm * now = localtime( & t );
    char datetime[20];
    strftime(datetime,20,"%Y-%m-%d %H%M",now);
    std::string newDirectory = std::string(datetime) + "\\" + name;

    ulong progress = maxGPR*repeatExperiments;
    boost::progress_display GPRProg(progress);
    std::string updateString = UpdateListStrings[updateType];
    for (int j = 0; j != repeatExperiments; j++) {
        std::string filepath = newDirectory + "\\iter" + std::to_string(j);
        Network network = Network(networkSize, networkSize, initialStrategiesMethod, updateType, filepath);
        std::string pathToStartingNetwork = filepath + "\\StartingNetwork.txt";
        network.exportNetwork(pathToStartingNetwork);

        for (int i = 1; i != maxGPR; i++) {
            //Network network = Network(networkSize, networkSize, initialStrategiesMethod, updateType, filepath);
            network = Network(pathToStartingNetwork);

            //network.connectNearestNeighbours();
            playNRounds(network, numRounds, i, saveBitmaps, returnGlobalData);
            ++GPRProg;
        }
    }

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
    description =
            "This experiment runs the iterated Prisoner's Dilemma repeatedly, up to 'RepeatExperiments' times. This is "
                    "useful for making claims about the statistical strength of any conculsions.";
    GPR = 15;
    numRounds = 50;
    networkSize = 100;
    repeatExperiments = 200;
    updateType = UpdateList::SL_ThresholdScoreTB;
    initialStrategiesMethod = &Strategy::random;
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
        std::string filepath = newDirectory + "\\iter" + std::to_string(i);
        Network network = Network(networkSize, networkSize, initialStrategiesMethod, updateType, filepath);
        network.connectNearestNeighbours();
        playNRounds(network, numRounds, GPR, saveBitmaps, returnGlobalData);
        ++repeatsProg;
    }
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
//TODO: Continue moving functions and commented out code blocks into here as more experiments.





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


ConsoleReturn Experiment::SetParameter(std::string paramName, std::string newValue){
    //std::cout << "Test ListParameterNames: " << ListParameterNames(this).size() << std::endl;
    std::vector<std::string> paramNames = ListParameterNames(this);
    if (std::find(paramNames.begin(), paramNames.end(), paramName) != paramNames.end()) {
        ParameterName param = parameterEnumMap.at(paramName);
        return this->_SetFoundParameter(param, newValue);
    }
    else{
        return ConsoleReturn::NotApplicableParam;
    }
}
ConsoleReturn Experiment::_SetFoundParameter(ParameterName param, std::string newValue){
    // Handle ints in bulk, as there're a lot of them!
    if (ParameterType.at(param) == "int"){
        try{
            int newInt = boost::lexical_cast<int>(newValue);
            return _SetFoundInt(param,newInt);
        }
        catch(...){
            return ConsoleReturn::UnknownFailure;
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
                return ConsoleReturn::InvalidNewValue;
            }
            return _SetFoundBool(param,newBool);
        }
        catch(...){
            return ConsoleReturn::UnknownFailure;
        }
    }

    switch (param) {
        case ParameterName::UpdateType:
            for (auto &i : UpdateListStrings) {
                if (i.second == newValue){
                    updateType = i.first;
                    return ConsoleReturn::Success;
                }
            }
            return ConsoleReturn::InvalidNewValue;
        case ParameterName::InitialStrategiesMethod:
            for (auto &i : StrategyGeneratorNames) {
                if (i.second == newValue){
                    initialStrategiesMethod = i.first;
                    return ConsoleReturn::Success;
                }
            }
            return ConsoleReturn::InvalidNewValue;
        default:
            return ConsoleReturn::UnknownFailure;//"Shouldn't see this!";
    }
}
ConsoleReturn Experiment::_SetFoundInt(ParameterName param, int newValue){
    if (ParameterType.at(param) == "int"){
        try{
            this->*intMap.at(param) = newValue;
            return ConsoleReturn::Success;
        }
        catch(...){
            ConsoleReturn::UnknownFailure;
        }
    }
    return ConsoleReturn::InvalidType;
}

ConsoleReturn Experiment::_SetFoundBool(ParameterName param, bool newValue){
    if (ParameterType.at(param) == "bool"){
        try{
            this->*boolMap.at(param) = newValue;
            return ConsoleReturn::Success;
        }
        catch(...){
            return ConsoleReturn::UnknownFailure;
        }
    }
    return ConsoleReturn::InvalidType;

}

void PrintParameterTable(Experiment * ex){
    std::vector<std::string> paramNames = ListParameterNames(ex);
    ulong maxLength = 0;
    for (std::string name : paramNames) {
        if (name.length() > maxLength) { maxLength = name.length(); }
    }
    std::cout << "    " << "Param" << std::string(2+maxLength-5,' ') << "Value" << std::endl;
    for (std::string name : paramNames){
        ParameterName param = parameterEnumMap.at(name);
        std::string value = ex->GetParameter(param);
        std::cout << "    " << name << std::string(2+maxLength-name.length(),' ') << value << std::endl;
    }
}