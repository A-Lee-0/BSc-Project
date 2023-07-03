//
// Created by Andrew Lee on 01/07/2023.
//

#include "Input.h"
#include <boost/algorithm/string.hpp>
#include <map>

//typedef void (InputCommand::*InputExecuteMethod)(); // function pointer type

#define RETURN_QUIT true
#define RETURN_CONTINUE false

#define INPUT_COMMAND_MAP(instruction,memberName) ()


// std::map<InputInstruction, InputExecuteMethod> InputExecuteMap = {{InputInstruction::Help,&InputCommandHelp::Execute}};


namespace {
    enum class InputInstruction {
        Help,
        RunExperiment,
        ChangeExperiment,
        DisplayParams,
        SetParam,
        GetParam,
        AppendListParam,
        RemoveListParam,
        Quit
    };

    // Disappointed that I've had to add these. I was hoping to avoid needing to instantiate the objects, and just use
    // static members the whole time. But unfortunately that's not feasible, as there's no pointer or reference to a
    // non-initialised class which I could use to access e.g. [InputCommandDerivedclass]::docString.
    // On the other hand, I can merge the setting of valid instruction strings in here.

    InputCommandHelp iHelp ( std::vector<std::string>{"help","h"} );
    InputCommandRun iRunExperiment( std::vector<std::string>{"run","r"} );
    InputCommandChangeExperiment iChangeExperiment( std::vector<std::string>{"experiment","e"} );
    InputCommandDisplayParams DisplayParams( std::vector<std::string>{"params","p"} );
    InputCommandSet SetParam( std::vector<std::string>{"set","s"} );
    InputCommandGet GetParam( std::vector<std::string>{"get","g"} );
//    InputCommandAppendListParam AppendListParam( std::vector<std::string>{"append"} );
//    InputCommandRemoveListParam RemoveListParam( std::vector<std::string>{"remove"} );
    InputCommandQuit Quit( std::vector<std::string>{"quit","q"} );


}

static bool returnValue;
namespace Input {
    bool ProcessInput(Experiment* experiment) {
        returnValue = RETURN_CONTINUE; //slightly gross to have a variable shared across the entire file, but simpler than adding a bool output to however many nested function calls I'll be making!


        std::cout << "There are " << inputCommands.size() << " valid commands." << std::endl;
        for( auto* command :inputCommands){

        }

        // 1. Read input from cin
        std::string rawInput;
        //std::getline(std::cin,rawInput);
        std::getline(std::cin >> std::ws, rawInput);
        boost::trim(rawInput);

        InputInstruction instruction;
        std::string argString;

        std::pair<InputInstruction,std::string> parsedInput;

        try {
            parsedInput = FindInstructionFromString(rawInput);
        }
        catch (UnknownInstruction) {
            std::cout << "No command recognised... try 'help' to see your options." << std::endl;
            return RETURN_CONTINUE;
        }

        instruction = parsedInput.first;
        argString = parsedInput.second;

        //std::cout << "instruction parsed. args: " << argString << std::endl;

        CallInstructionFunction(instruction, experiment, argString);
        return returnValue;
    }
}






/**
 * Processes the string produced by a std::cin.getline(). If the characters up to the first space in the string (or the
 * entire string) match an Instruction Keyword in InputInstructionKeywords, then return the associated InputInstruction.
 * Otherwise raise exception.
 *
 * @param input - String containing the full line of text entered by the user.
 * @return - The matching InputInstruction for the command, if one exists.
 */
std::pair<InputInstruction,std::string> FindInstructionFromString(std::string input){
    std::string instructionString;
    std::string argString;

    // look for space
    auto firstSpacePos = input.find(' ');

    if ( firstSpacePos == std::string::npos){
        // No space found - treat full string as instruction.
        instructionString = input;
        argString = "";
    }
    else {
        instructionString = input.substr(0,firstSpacePos);
        argString = input.substr(firstSpacePos+1);
    }

    boost::to_lower(instructionString); // Ignore case of string by lowering now.

    // check if instruction string is valid
    try {
        InputInstruction instruction = InputInstructionKeywords.at(instructionString);
        return {instruction,argString};
    }
    catch (const std::out_of_range& oor) {
        // Catch error from non-existant InputInstructionKeywords.
        throw UnknownInstruction( "Unknown instruction '" + instructionString + "'." );
    }
}

namespace {
    template<> InputInstruction InputCommandInter<InputCommandHelp>::instruction = InputInstruction::Help;
    template<> std::string InputCommandInter<InputCommandHelp>::docString = "Help: Prints details of all instructions.";
    template<> std::string InputCommandInter<InputCommandHelp>::argString = "    Args: none";
    /**
     * @param experiment - Pointer to the experiment whose parameter we are trying to set.
     * @param args - String containing the text entered by the user after the instruction keyword.
     */
    template<> void InputCommandInter<InputCommandHelp>::Execute(Experiment* experiment, std::string args) {
        //std::map<InputInstruction, std::list<std::string>> reverseKeywordLookup;

        // print details for each instruction
        for(auto command : inputCommands){
            // output dividing line between instructions
            std::cout << ""
    "------------------------------------------------------------------------------------------------------------------------"
                      << std::endl;

            // output instruction name and what it does
            std::cout << command->getDocString() << std::endl;

            // output the keyword options to use the instruction
            std::string keys = "  Commands: ";
            for (auto key : command->getKeywords()){
                keys += "\'" + key + "\', ";
            }
            std::string fullKeys = keys.substr(0, keys.size()-3);
            std::cout << fullKeys << std::endl;

            // output the optional parameters for the instruction.
            std::cout << command->getArgString() << std::endl;

        }
    }


    template<> InputInstruction InputCommandInter<InputCommandRun>::instruction = InputInstruction::RunExperiment;
    template<> std::string InputCommandInter<InputCommandRun>::docString = "Run: Attempts to run the experiment with the current parameters.";
    template<> std::string InputCommandInter<InputCommandRun>::argString = "    Args: none";
    /**
     * @param experiment - Pointer to the experiment whose parameter we are trying to set.
     * @param args - String containing the text entered by the user after the instruction keyword.
     */
    template<> void InputCommandInter<InputCommandRun>::Execute(Experiment* experiment, std::string args) {
        time_t t = time(0);   // get time now
        struct tm * now = localtime( & t );
        char datetime[20];
        strftime(datetime,20,"%Y-%m-%d %H:%M",now);
        std::cout << std::endl; // add an extra line break
        std::cout << "Time Started: " << datetime << std::endl;

        experiment->Run();

        t = time(0);   // get time now
        now = localtime( & t );
        strftime(datetime,20,"%Y-%m-%d %H:%M",now);
        std::cout << std::endl; // add an line break so it doesn't append to boost::progress
        std::cout << "Time Finished: " << datetime << std::endl;
    }



    template<> InputInstruction InputCommandInter<InputCommandChangeExperiment>::instruction = InputInstruction::ChangeExperiment;
    template<> std::string InputCommandInter<InputCommandChangeExperiment>::docString =
            "Experiment: If no argument is given, gives details of the current experiment, and lists valid NewExperimentTypes.\n"
            "            If there is an argument, discards the current experiment and creates a new one of the given NewExperimentType.";
    template<> std::string InputCommandInter<InputCommandChangeExperiment>::argString = "    Args: NewExperimentType";
    /**
     *
     * @param experiment - Pointer to the experiment whose parameter we are trying to set.
     * @param args - String containing the text entered by the user after the instruction keyword. Function attempts to
     *               parse it as an 'experimentNameMap' key.
     */
    template<> void InputCommandInter<InputCommandChangeExperiment>::Execute(Experiment* experiment, std::string args) {
        // If no args are supplied, just display the full details of the current experiment.
        if (args.length() == 0) {
            try {
                std::cout << "The current Experiment is: " << experiment->getName() << std::endl;
                std::cout << "Description: " << experiment->getDescription() << std::endl;
                std::cout << "Valid Experiments are: " << std::endl;
                for (const auto &exPair : experimentNameMap) {
                    std::cout << "    " << exPair.first << std::endl;
                }
            }
            catch (...) {
                std::cout << "Something went wrong..." << std::endl;
            }
        }

            // Otherwise, attempt to change the experiment.
        else{
            try {
                auto pos = args.find(" ");   // only look upto first space
                std::string newExperimentName = args.substr(0, pos);
                boost::trim(newExperimentName);     // Shouldn't do anything, but better safe than sorry.

                if(experimentNameMap.count(newExperimentName)){
                    ExperimentList experimentType = experimentNameMap.at(newExperimentName);
                    delete experiment;
                    experiment = CreateExperiment(experimentType);
                    std::cout << "Created new experiment of type '" << experiment->getName() << "'." << std::endl;
                    PrintParameterTable(experiment);
                }
                else{
                    std::cout << "new Experiment '" << newExperimentName << "' is not a recognised Experiment type. Valid Experiments are: " << std::endl;
                    for(const auto &exPair : experimentNameMap){
                        std::cout << "    " << exPair.first << std::endl;
                    }
                }
            }
            catch (...) {
                std::cout << "Something went wrong..." << std::endl;
            }
        }

    }


    template<> InputInstruction InputCommandInter<InputCommandSet>::instruction = InputInstruction::SetParam;
    template<> std::string InputCommandInter<InputCommandSet>::docString = "Set: Attempts to set a parameter for the experiment to a new value.";
    template<> std::string InputCommandInter<InputCommandSet>::argString = "    Args: ParameterName, NewValue";
    /**
     * Processes the string produced by a std::cin.getline(), whose first 4 characters are "set ", or equivalent.
     * Attempts to set the value of a parameter for the current experiment to a given new value
     *
     * @param experiment - Pointer to the experiment whose parameter we are trying to set.
     * @param args - String containing the text entered by the user after the instruction keyword.
     */
    template<> void InputCommandInter<InputCommandSet>::Execute(Experiment* experiment, std::string args) {
        try {
            auto pos = args.find(" ");
            std::string paramName = args.substr(0, pos);
            std::string newValue = args.substr(pos + 1);
            boost::trim(paramName);     // Shouldn't do anything, but better safe than sorry.
            boost::trim(newValue);
            //std::cout << "Attempting to set parameter '" << paramName << "' to the value '" << newValue << "'."
            //          << std::endl;
            //experiment->SetParameter(paramName, newValue);
            ConsoleReturn consRet = experiment->SetParameter(paramName, newValue);
            switch (consRet) {
                case ConsoleReturn::Success:
                    std::cout << "Parameter name '" << paramName << "' successfully changed to '"
                              << experiment->GetParameter(paramName) << "'." << std::endl;
                    break;
                case ConsoleReturn::UnknownParamString:
                    std::cout << "Parameter name '" << paramName << "' was not recognised." << std::endl;
                    break;
                case ConsoleReturn::NotApplicableParam:
                    std::cout << "Parameter name '" << paramName << "' is not used by experiment type '"
                              << experiment->getName() << "'." << std::endl;
                    break;
                default:
                    std::cout << "Something went wrong! Parameter '" << paramName << "' is currently '"
                              << (experiment->GetParameter(paramName)) << "'." << std::endl;
                    break;
            }
        }
        catch (...){
            std::cout << "Something went wrong..." << std::endl;
        }
    }


    template<> InputInstruction InputCommandInter<InputCommandGet>::instruction = InputInstruction::GetParam;
    template<> std::string InputCommandInter<InputCommandGet>::docString = "Get: Displays the value of a parameter for the experiment.";
    template<> std::string InputCommandInter<InputCommandGet>::argString = "    Args: ParameterName";
    /**
     * Processes the string produced by a std::cin.getline(), whose first word is "get ", or equivalent.
     * Attempts to return the value of a parameter specified in args.
     *
     * @param experiment - Pointer to the current experiment whose parameter we are trying to read.
     * @param args - String containing the text entered by the user after the instruction keyword.
     */
    template<> void InputCommandInter<InputCommandGet>::Execute(Experiment* experiment, std::string args) {
        try {
            auto pos = args.find(" ");  // only look upto first space, in case someone just flips 'set' to 'get'
            std::string paramName = args.substr(0, pos);
            boost::trim(paramName);     // Shouldn't do anything, but better safe than sorry.
            //std::cout << "Attempting to get the current value of parameter '" << paramName << "'." << std::endl;
            std::vector<std::string> paramNames = ListParameterNames(experiment);
            if (std::find(paramNames.begin(), paramNames.end(), paramName) != paramNames.end()) {
                ParameterName param = parameterEnumMap.at(paramName);
                std::cout << "Parameter '" << paramName << "' is currently '" << (experiment->GetParameter(param))
                          << "'." << std::endl;
            }
        }
        catch (...) {
            std::cout << "Something went wrong..." << std::endl;
        }
    }


    template<> InputInstruction InputCommandInter<InputCommandDisplayParams>::instruction = InputInstruction::DisplayParams;
    template<> std::string InputCommandInter<InputCommandDisplayParams>::docString = "Params: Displays all of the parameters applicable to the current experiment, and their respective values.";
    template<> std::string InputCommandInter<InputCommandDisplayParams>::argString = "    Args: none";
    /**
     * @param experiment - Pointer to the current experiment whose parameter we are trying to read.
     * @param args - String containing the text entered by the user after the instruction keyword.
     */
    template<> void InputCommandInter<InputCommandDisplayParams>::Execute(Experiment* experiment, std::string args){
        try {
            std::cout << experiment->getName()<< std::endl;
            PrintParameterTable(experiment);
        }
        catch (...) {
            std::cout << "Something went wrong..." << std::endl;
        }
    }


    template<> InputInstruction InputCommandInter<InputCommandQuit>::instruction = InputInstruction::Quit;
    template<> std::string InputCommandInter<InputCommandQuit>::docString = "Quit: Quits the program.";
    template<> std::string InputCommandInter<InputCommandQuit>::argString = "    Args: none";
    /**
     * @param experiment - Pointer to the current experiment whose parameter we are trying to read.
     * @param args - String containing the text entered by the user after the instruction keyword.
     */
    template<> void InputCommandInter<InputCommandQuit>::Execute(Experiment* experiment, std::string args){
        returnValue = RETURN_QUIT;
    }


    template<class T> std::vector<std::string> InputCommandInter<T>::getKeywords() { return keywords; }
    template<class T> std::string InputCommandInter<T>::getDocString(){ return docString; }
    template<class T> std::string InputCommandInter<T>::getArgString(){ return argString; }
}


// Annoyingly, have to move this below the definitions of the functions, as template method in Input.h seems not to count.
/**
 * Calls the appropriate function for each instruction, and passes the current experiment and additional args.
 *
 * @param instruction - The InputInstruction, e.g. as determined by FindInstructionFromString()
 * @param experiment - Pointer to the current experiment
 * @param args - Additional arguments input in string form, e.g. the parameter name and a value to set it as.
 */
static void CallInstructionFunction(InputInstruction instruction, Experiment* experiment, std::string args){
    switch(instruction){
        case InputInstruction::Help:
            InputCommandHelp::Execute(experiment,args);
            break;
        case InputInstruction::RunExperiment:
            InputCommandRun::Execute(experiment,args);
            break;
        case InputInstruction::ChangeExperiment:
            InputCommandChangeExperiment::Execute(experiment,args);
            break;
        case InputInstruction::DisplayParams:
            InputCommandDisplayParams::Execute(experiment,args);
            break;
        case InputInstruction::SetParam:
            InputCommandSet::Execute(experiment,args);
            break;
        case InputInstruction::GetParam:
            InputCommandGet::Execute(experiment,args);
            break;
        case InputInstruction::AppendListParam:
            // InputCommandAppend::Execute(experiment,args);
            break;
        case InputInstruction::RemoveListParam:
            // InputCommandRemove::Execute(experiment,args);
            break;
        case InputInstruction::Quit:
            InputCommandQuit::Execute(experiment,args);
            break;
        default:
            std::string errorMsg = "The InputInstruction \'"+  std::to_string(static_cast<int>(instruction)) + "\' is not implemented in \'CallInstructionFunction\' in Input.cpp";
            std::cout << errorMsg << std::endl;
            throw std::invalid_argument(errorMsg);
    }
}