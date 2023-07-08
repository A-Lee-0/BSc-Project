//
// Created by Andrew Lee on 01/07/2023.
//

#include <boost/algorithm/string.hpp>
#include <map>

#include "ANSI_codes.h"
#include "Input.h"
#include "Console.h"

//typedef void (InputCommand::*InputExecuteMethod)(); // function pointer type

#define RETURN_QUIT true
#define RETURN_CONTINUE false


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
    bool TEST_ANSI = false;

    /**This function reads the input from the terminal using the Console module, and determines if it contains a valid
     * instruction. If so, it then calls the corresponding function.
     * (if any) to handle that inpu
     *
     * @param experiment
     * @return
     */
    bool ProcessInput(Experiment* experiment) {
        returnValue = RETURN_CONTINUE; //slightly gross to have a variable shared across the entire file, but simpler than adding a bool output to however many nested function calls I'll be making!

        if(TEST_ANSI){
            std::string cppString = TestANSICodes();
            printCP(cppString);
            TEST_ANSI = false;      // only print the test strings once.
        }

        //std::pair<int,int> termSize = Console::GetSize();
        //printCP("Terminal is currently " + std::to_string(termSize.first) + " rows, and " + std::to_string(termSize.second) + " columns large.\n");

        // 1. Read input from Console
        std::string inputString = Console::GetInput();
        Console::ResetLine();   // we have the string, so clear the line and reprint it formatted.
        std::stringstream printString;
        printString << ansi_escape_codes::color_rgb(80,80,255) << ansi_escape_codes::bold;
        printString << inputString << ansi_escape_codes::reset << std::endl;
        printCP(printString.str());  // Print formatted input

        // 2. Look for corresponding input instruction
        InputInstruction instruction;
        std::string argString;
        std::pair<InputInstruction,std::string> parsedInput;

        try {
            parsedInput = FindInstructionFromString(inputString);
        }
        catch (UnknownInstruction) {
            // 3a. Break if no instruction is found
            printCP("No command recognised... try 'help' to see your options.\n");
            return RETURN_CONTINUE;
        }

        // 3b. Call the corresponding function if instruction is found
        instruction = parsedInput.first;
        argString = parsedInput.second;

        CallInstructionFunction(instruction, experiment, argString);
        return returnValue;
    }
}



/**Processes the string produced by a Console::GetInput(). If the characters up to the first space in the string (or the
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
    /**Prints the Help text associated with the given command. Used in  InputCommandInter<InputCommandHelp>::Execute()
     *
     * @param command - The command to print the Help text for.
     */
    void PrintInstructionHelp(InputCommand* command){
        // output instruction name and what it does
        printCP(command->getDocString() + "\n");

        // output the keyword options to use the instruction
        std::string keys = "  Commands: ";
        for (auto key : command->getKeywords()) {
            keys += "\'" + key + "\', ";
        }
        std::string fullKeys = keys.substr(0, keys.size() - 2);
        printCP(fullKeys + "\n");

        // output the optional parameters for the instruction.
        printCP(command->getArgString() + "\n");
    }

    template<> InputInstruction InputCommandInter<InputCommandHelp>::instruction = InputInstruction::Help;
    template<> std::string InputCommandInter<InputCommandHelp>::docString = "Help: Prints details of all instructions, or just the instruction indicated by commandString.";
    template<> std::string InputCommandInter<InputCommandHelp>::argString = "    Args: [commandString]";
    /**
     * @param experiment - Pointer to the current experiment.
     * @param args - String containing the remaining text from the input after the 'help' command. If it starts with a commandString, print only that docString.
     */
    template<> void InputCommandInter<InputCommandHelp>::Execute(Experiment* experiment, std::string args) {
        //std::map<InputInstruction, std::list<std::string>> reverseKeywordLookup;

        uint printWidth = Console::GetSize().second;
        std::string divider = std::string(printWidth, '-') + "\n";

        // If args is not blank, test to see if it contains an Instruction String. If it does, only show docstring for
        // that instruction.
        bool listAllInstructions = true;
        InputInstruction instruction;
        if (args != "") {

            std::pair<InputInstruction, std::string> parsedInput;
            try {
                parsedInput = FindInstructionFromString(args);
                instruction = parsedInput.first;
                listAllInstructions = false;
            }
            catch (UnknownInstruction) {

            }
        }

        if (listAllInstructions) {
            // print details for each instruction
            for (auto command : inputCommands) {
                // output dividing line between instructions
                printCP(divider);

                // print the Help text for the command
                PrintInstructionHelp(command);


            }
        }
        else{
            for (auto command : inputCommands){
                if (command->getInstruction() == instruction){
                    // print the Help text for the command
                    PrintInstructionHelp(command);
                }
            }
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
        printCP("\n");
        printCP("Time Started: " + std::string(datetime) + "\n");

        experiment->Run();

        t = time(0);   // get time now
        now = localtime( & t );
        strftime(datetime,20,"%Y-%m-%d %H:%M",now);
        printCP("\n");  // add an line break so it doesn't append to boost::progress
        printCP("Time Finished: " + std::string(datetime) + "\n");
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
                printCP("The current Experiment is: " + experiment->getName() + "\n");
                printCP("Description: " + experiment->getDescription() + "\n");
                printCP("Valid Experiments are:\n");
                for (const auto &exPair : experimentNameMap) {
                    printCP("    " + exPair.first + "\n");
                }
            }
            catch (...) {
                printCP("Something went wrong...\n");
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
                    printCP("Created new experiment of type \'" + experiment->getName() + "\'.\n");
                    printCP(PrintParameterTable(experiment));
                }
                else{
                    printCP("new Experiment '" + newExperimentName + "' is not a recognised Experiment type. Valid Experiments are:\n");
                    for(const auto &exPair : experimentNameMap){
                        printCP("    " + exPair.first + "\n");
                    }
                }
            }
            catch (...) {
                printCP("Something went wrong...\n");
            }
        }

    }


    template<> InputInstruction InputCommandInter<InputCommandSet>::instruction = InputInstruction::SetParam;
    template<> std::string InputCommandInter<InputCommandSet>::docString = "Set: Attempts to set a parameter for the experiment to a new value.";
    template<> std::string InputCommandInter<InputCommandSet>::argString = "    Args: ParameterName, NewValue";
    /**Attempts to set the value of a parameter for the current experiment to a given new value.
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
            //experiment->SetParameter(paramName, newValue);
            ConsoleReturn consRet = experiment->SetParameter(paramName, newValue);
            std::stringstream printString;
            switch (consRet) {
                case ConsoleReturn::Success:
                    printCP("Parameter name \'" + paramName + "\' successfully changed to \'"
                                + experiment->GetParameter(paramName) + "\'.\n");
                    break;
                case ConsoleReturn::UnknownParamString:
                    printCP("Parameter name \'" + paramName + "\' was not recognised.\n");
                    break;
                case ConsoleReturn::NotApplicableParam:
                    printCP("Parameter name \'" + paramName + "\' is not used by experiment type \'"
                                + experiment->getName() + "\'.\n");
                    break;
                default:
                    printCP("Something went wrong! Parameter \'" + paramName + "\' is currently \'"
                                + (experiment->GetParameter(paramName)) + "\'.\n");
                    break;
            }
        }
        catch (...){
            printCP("Something went wrong...\n");
        }
    }


    template<> InputInstruction InputCommandInter<InputCommandGet>::instruction = InputInstruction::GetParam;
    template<> std::string InputCommandInter<InputCommandGet>::docString = "Get: Displays the value of a parameter for the experiment.";
    template<> std::string InputCommandInter<InputCommandGet>::argString = "    Args: ParameterName";
    /**Attempts to return the value of a parameter specified in args.
     *
     * @param experiment - Pointer to the current experiment whose parameter we wish to know.
     * @param args - String containing the text entered by the user after the instruction keyword.
     */
    template<> void InputCommandInter<InputCommandGet>::Execute(Experiment* experiment, std::string args) {
        try {
            auto pos = args.find(" ");  // only look upto first space, in case someone just flips 'set' to 'get'
            std::string paramName = args.substr(0, pos);
            boost::trim(paramName);     // Shouldn't do anything, but better safe than sorry.
            std::vector<std::string> paramNames = ListParameterNames(experiment);
            if (std::find(paramNames.begin(), paramNames.end(), paramName) != paramNames.end()) {
                ParameterName param = parameterEnumMap.at(paramName);
                std::stringstream printString;
                printString << "Parameter '" << paramName << "' is currently '" << (experiment->GetParameter(param))
                          << "'." << std::endl;
                printCP(printString.str());
            }
        }
        catch (...) {
            printCP("Something went wrong...\n");
        }
    }


    template<> InputInstruction InputCommandInter<InputCommandDisplayParams>::instruction = InputInstruction::DisplayParams;
    template<> std::string InputCommandInter<InputCommandDisplayParams>::docString = "Params: Displays all of the parameters applicable to the current experiment, and their respective values.";
    template<> std::string InputCommandInter<InputCommandDisplayParams>::argString = "    Args: none";
    /**Prints all of the parameters that are relevant to the current experiment.
     *
     * @param experiment - Pointer to the current experiment whose parameters we wish to know.
     * @param args - N/A
     */
    template<> void InputCommandInter<InputCommandDisplayParams>::Execute(Experiment* experiment, std::string args){
        try {
            printCP(experiment->getName() + "\n");
            printCP(PrintParameterTable(experiment));
        }
        catch (...) {
            printCP("Something went wrong...\n");
        }
    }


    template<> InputInstruction InputCommandInter<InputCommandQuit>::instruction = InputInstruction::Quit;
    template<> std::string InputCommandInter<InputCommandQuit>::docString = "Quit: Quits the program.";
    template<> std::string InputCommandInter<InputCommandQuit>::argString = "    Args: none";
    /**Quits the program.
     *
     * @param experiment - N/A
     * @param args - N/A
     */
    template<> void InputCommandInter<InputCommandQuit>::Execute(Experiment* experiment, std::string args){
        returnValue = RETURN_QUIT;
    }


    template<class T> std::vector<std::string> InputCommandInter<T>::getKeywords() { return keywords; }
    template<class T> std::string InputCommandInter<T>::getDocString(){ return docString; }
    template<class T> std::string InputCommandInter<T>::getArgString(){ return argString; }
    template<class T> InputInstruction InputCommandInter<T>::getInstruction(){ return instruction; }
}


// Annoyingly, have to move this below the definitions of the functions, as template method in Input.h seems not to count.

/**Calls the appropriate function for each instruction, and passes the current experiment and additional args.
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
            printCP(errorMsg + "\n");
            throw std::invalid_argument(errorMsg);
    }
}


static std::string TestANSICodes(){
    // test ANSI codes
    // Discovered that some console can more consistently parse the ansi codes if I merge multiple lines into a single string. Maybe a syncing problem?
    std::stringstream printString;
    printString << ansi_escape_codes::bold << "Test bold string" << ansi_escape_codes::reset << std::endl;
    printString << ansi_escape_codes::faint << "Test faint string" << ansi_escape_codes::reset << std::endl;
    printString << ansi_escape_codes::italic << "Test italic string" << ansi_escape_codes::reset << std::endl;
    printString << ansi_escape_codes::underline << "Test underline string" << ansi_escape_codes::reset << std::endl;
    printString << ansi_escape_codes::slow_blink << "Test slow blinking string" << ansi_escape_codes::reset << std::endl;
    printString << ansi_escape_codes::rapid_blink << "Test rapid blinking string" << ansi_escape_codes::reset << std::endl;
    printString << ansi_escape_codes::inverse << "Test inverse string" << ansi_escape_codes::reset << std::endl;
    printString << ansi_escape_codes::conceal << "Test conceal string" << ansi_escape_codes::reset << std::endl;
    printString << ansi_escape_codes::crossed_out << "Test crossed_out string" << ansi_escape_codes::reset << std::endl;

    //colors!
    printString << ansi_escape_codes::blue << "Test blue string" << ansi_escape_codes::reset << std::endl;
    printString << ansi_escape_codes::cyan << "Test cyan string" << ansi_escape_codes::reset << std::endl;
    printString << ansi_escape_codes::bright_blue << "Test \'bright blue\' string" << ansi_escape_codes::reset << std::endl;
    printString << ansi_escape_codes::bold << ansi_escape_codes::blue << "Test bold blue string" << ansi_escape_codes::reset << std::endl;
    printString << ansi_escape_codes::blue_bg << "Test blue background string" << ansi_escape_codes::reset << std::endl;
    printString << ansi_escape_codes::cyan_bg << "Test cyan background string" << ansi_escape_codes::reset << std::endl;
    printString << ansi_escape_codes::bright_blue_bg << "Test \'bright blue\' background string" << ansi_escape_codes::reset << std::endl;
    printString << ansi_escape_codes::bold << ansi_escape_codes::blue_bg << "Test bold blue background string" << ansi_escape_codes::reset << std::endl;
    printString << ansi_escape_codes::color_rgb(255,0,255) << "Test rgb Magenta string" <<ansi_escape_codes::reset << std::endl;
    printString << ansi_escape_codes::color_rgb(255,0,255) << "Test rgb Magenta string " << ansi_escape_codes::color_bg_rgb(255,255,0) << "with rgb yellow background!" <<ansi_escape_codes::reset << std::endl;

    //check reset and screen width!
    printString << "Test normal string" << std::endl;
    printString << "Test very long string to see if the text will loop at the edge of the window, or if it will break the formatting and run off the edge of the screen!" << std::endl;

    return printString.str();
}



