//
// Created by Andrew Lee on 01/07/2023.
//

#ifndef SGCB_INPUT_H
#define SGCB_INPUT_H

#include "Experiment.h"

// public functions to call
namespace Input {
    bool ProcessInput(Experiment*);
}
// anonymous namespace to keep InputInstruction from being exposed outside Input.
namespace{ enum class InputInstruction; }

static std::pair<InputInstruction,std::string> FindInstructionFromString(std::string input);
static void CallInstructionFunction(InputInstruction instruction, Experiment* experiment, std::string args);


// anonymous namespace to keep internal Command classes from being exposed outside Input.
namespace {
    class InputCommand {
    public:
        virtual std::vector<std::string> getKeywords();

        virtual std::string getDocString();

        virtual std::string getArgString();
    };

    std::map<std::string, InputInstruction> InputInstructionKeywords;        // Map to hold the allowed strings for each input instruction.
    static std::list<InputCommand *> inputCommands;

    // This is an intermediate class to define the static variables and methods, but still allow for all the derived
    // classes to be of one type (InputCommand).
    template<class dummy>
    class InputCommandInter : public InputCommand {
    public:
        static InputInstruction instruction;
        static std::string docString;
        static std::string argString;
        std::vector<std::string> keywords;

        static void Execute(Experiment *, std::string);

        std::vector<std::string> getKeywords();

        std::string getDocString();

        std::string getArgString();

        //InputCommandInter(){ inputCommands.push_back(this); };
        InputCommandInter(std::vector<std::string> newKeywords) {
            inputCommands.push_back(this);
            keywords = newKeywords;
            for (auto key : newKeywords) {
                InputInstructionKeywords[key] = instruction;
            }
        };
    };

    class InputCommandHelp : public InputCommandInter<InputCommandHelp> { using InputCommandInter::InputCommandInter; };
    class InputCommandRun : public InputCommandInter<InputCommandRun> { using InputCommandInter::InputCommandInter; };
    class InputCommandChangeExperiment : public InputCommandInter<InputCommandChangeExperiment> { using InputCommandInter::InputCommandInter; };
    class InputCommandDisplayParams : public InputCommandInter<InputCommandDisplayParams> { using InputCommandInter::InputCommandInter; };
    class InputCommandSet : public InputCommandInter<InputCommandSet> { using InputCommandInter::InputCommandInter; };
    class InputCommandGet : public InputCommandInter<InputCommandGet> { using InputCommandInter::InputCommandInter; };
    class InputCommandAppendListParam : public InputCommandInter<InputCommandAppendListParam> { using InputCommandInter::InputCommandInter; };
    class InputCommandRemoveListParam : public InputCommandInter<InputCommandRemoveListParam> { using InputCommandInter::InputCommandInter; };
    class InputCommandQuit : public InputCommandInter<InputCommandQuit> { using InputCommandInter::InputCommandInter; };


    // Exception to be thrown when the instruction string is unrecognised.
    class UnknownInstruction : public std::exception {
    public:
        UnknownInstruction(std::string msg) : message(msg) {}

        const char *what() { return message.c_str(); }

    private:
        std::string message;
    };
}
#endif //SGCB_INPUT_H