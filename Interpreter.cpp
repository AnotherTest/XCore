#include "Interpreter.h"
#include <algorithm>
#include "XAssert.h"

// Interpreter private member functions implementation starts here

Data::Subroutine* findChild(Data::SubTable& routines, Data::Subroutine* parent, const std::string& name)
{
    auto predicate = [&](Data::Subroutine* sub) -> bool
    {
        return sub->getParent() == parent && sub->getName() == name;
    };
    auto pos = std::find_if(routines.begin(), routines.end(), predicate);
    if(pos == routines.end())
        throw InterpreterException(
            "could not find routine " + name + " with parent " + parent->getName()
        );
    else
        return *pos;
}

Data::Subroutine* findSubroutine(Data::SubTable& subs, const std::string& name, Data::Subroutine* scope)
{
    try {
        return findChild(subs, scope, name);
    } catch(const InterpreterException& e) {
        if(scope->hasParent())
            return findSubroutine(subs, name, scope->getParent());
        else
            throw;
    }
}

std::string getCallHead(const std::string& call)
{
    return call.substr(0, call.find('.'));
}

std::string getCallTail(const std::string& call)
{
    size_t pos = call.find_last_of('.');
    if(pos == std::string::npos)
        return getCallHead(call);
    return call.substr(pos + 1, call.length() - pos);
}

std::string getCallRest(const std::string& call)
{
    return call.substr(call.find('.') + 1);
}

void Interpreter::executeCall(Data::Instruction* instruction)
{
    X_ASSERT(instruction != nullptr);
    X_ASSERT(instruction->getType() == Data::Instruction::Call);
    std::string name = boost::get<std::string>(instruction->getValue());
    bool found = true;
    try {
        std::string head;
        std::string tail = name;
        do {
            head = getCallHead(tail);
            tail = getCallTail(tail);
            current = findSubroutine(routines, head, current);
        } while(head != tail);
    } catch(const InterpreterException& e) {
        found = false;
    }
    return found ? execute(current->getInstructions()) : executeLibCall(name);
}

void Interpreter::executeLibCall(const std::string& name)
{
    if(name == "Include") {
        Data::Instruction* ins = stack.top();
        if(ins->getType() != Data::Instruction::StringLit)
            throw InterpreterException("Import expects string literal as top-most stack element");
        std::string lib_name = boost::get<std::string>(ins->getValue());
        modules.load(lib_name);
    } else if (name == "Exclude") {
        Data::Instruction* ins = stack.top();
        if(ins->getType() != Data::Instruction::StringLit)
            throw InterpreterException("Export expects string literal as top-most stack element");
        std::string lib_name = boost::get<std::string>(ins->getValue());
        modules.unload(lib_name);
    } else {
        SharedData data(routines, current, modules);
        modules.call(getCallHead(name), getCallRest(name), stack, data);
    }
}

void Interpreter::execute(const Data::InstructionTable& instructions)
{
    for(Data::Instruction* action : instructions) {
        X_ASSERT(action->getType() != Data::Instruction::Undefined);
        if(action->getType() == Data::Instruction::Call) {
            executeCall(action);
        } else {
            stack.push(action);
        }
    }
}

// Interpreter public member functions implementation starts here

Interpreter::Interpreter(const Data::SubTable& subs, const ModuleLoader& mods, Data::XStack& stack,
                         Data::Subroutine* entry)
    : routines(subs), stack(stack), current(entry), modules(mods)
{

}

void Interpreter::interpret()
{
    try {
        if(current == nullptr) {
            // first routine is always the root
            Data::Subroutine* root = routines.front();
            X_ASSERT(root->hasParent() == false);
            current = findChild(routines, root, "Main"); // program starts at Main
        }
        execute(current->getInstructions());
    } catch(const InterpreterException& e) {
        std::cerr << "Error while interpreting:\n"
                  << e.what() << std::endl;
    }

}
