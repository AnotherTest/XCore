#ifndef _X_INTERPRETER_H_INCLUDE_GUARD
#define _X_INTERPRETER_H_INCLUDE_GUARD

#include <stdexcept>
#include "Data.h"
#include "Module.h"

class InterpreterException : public std::exception {
    std::string message;
public:
    InterpreterException(const std::string& msg)
        : message(msg) {}
    const char* what() const throw()
    {
        return message.c_str();
    }
};

Data::Subroutine* findChild(Data::SubTable& routines, Data::Subroutine* parent, const std::string& name);

Data::Subroutine* findSubroutine(Data::SubTable& subs, const std::string& name, Data::Subroutine* scope);

std::string getCallHead(const std::string& call);
std::string getCallTail(const std::string& call);
std::string getCallRest(const std::string& call);

class Interpreter {
    Data::SubTable routines;
    Data::XStack& stack;
    Data::Subroutine* current;
    ModuleLoader modules;

    void executeCall(Data::Instruction* instruction);
    void executeLibCall(const std::string& name);
    void execute(const Data::InstructionTable& instructions);
public:
    Interpreter(const Data::SubTable& subs, const ModuleLoader& mods, Data::XStack& stack,
                Data::Subroutine* entry = nullptr);
    Interpreter(const Interpreter&) = delete;
    Interpreter& operator=(Interpreter&) = delete;
    ~Interpreter() = default;
    void interpret();
};

#endif // _X_INTERPRETER_H_INCLUDE_GUARD

