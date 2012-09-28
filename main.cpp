#include <unordered_map>
#include "Data.h"
#include "Interpreter.h"

template <typename T>
T getValue(const Data::Instruction& i, Data::Instruction::Type t, const std::string& fun = "XCore call")
{
    if(i.getType() != t)
        throw std::runtime_error("argument of incorrect type for " + fun);
    return boost::get<T>(i.getValue());
}

Data::Instruction operator+(const Data::Instruction& left, const Data::Instruction& right)
{
    Data::Instruction::Type t = left.getType();
    switch(t) {
        case Data::Instruction::IntLit:
            return getValue<int>(left, t, "Integer Sum") + getValue<int>(right, t, "Integer Sum");
        case Data::Instruction::DoubleLit:
            return getValue<double>(left, t, "Real Sum") + getValue<double>(right, t, "Real Sum");
        case Data::Instruction::CharLit:
            return getValue<char>(left, t, "Char Sum") + getValue<char>(right, t, "Char Sum");
        case Data::Instruction::StringLit:
            return getValue<std::string>(left, t, "String Sum")
                   + getValue<std::string>(right, t, "String Sum");
        default:
            throw std::runtime_error("Type does not support XCore summation.");
    }
}

Data::Instruction operator-(const Data::Instruction& left, const Data::Instruction& right)
{
    Data::Instruction::Type t = left.getType();
    switch(t) {
        case Data::Instruction::IntLit:
            return getValue<int>(left, t, "Integer Subt") - getValue<int>(right, t, "Integer Subt");
        case Data::Instruction::DoubleLit:
            return getValue<double>(left, t, "Real Subt") - getValue<double>(right, t, "Real Subt");
        default:
            throw std::runtime_error("Type does not support XCore subtraction.");
    }
}

Data::Instruction operator*(const Data::Instruction& left, const Data::Instruction& right)
{
    Data::Instruction::Type t = left.getType();
    switch(t) {
        case Data::Instruction::IntLit:
            return getValue<int>(left, t, "Integer Mult") * getValue<int>(right, t, "Integer Mult");
        case Data::Instruction::DoubleLit:
            return getValue<double>(left, t, "Real Mult") * getValue<double>(right, t, "Real Mult");
        default:
            throw std::runtime_error("Type does not support XCore multiplication.");
    }
}

Data::Instruction operator/(const Data::Instruction& left, const Data::Instruction& right)
{
    Data::Instruction::Type t = left.getType();
    switch(t) {
        case Data::Instruction::IntLit:
            return getValue<int>(left, t, "Integer Div") / getValue<int>(right, t, "Integer Div");
        case Data::Instruction::DoubleLit:
            return getValue<double>(left, t, "Real Div") / getValue<double>(right, t, "Real Div");
        default:
            throw std::runtime_error("Type does not support XCore division.");
    }
}

namespace XCore {
    typedef void (*XFunction) (Data::XStack&, SharedData&);
    std::unordered_map<std::string, XFunction> fun_table;
    std::unordered_map<std::string, Data::Instruction*> var_table;
    std::vector<Data::Instruction*> new_instructions; // must be deallocated on unload

    Data::Instruction* getStackTop(Data::XStack& stack)
    {
        Data::Instruction* top = stack.top();
        stack.pop();
        return top;
    }

    Data::Instruction* getStackTop(Data::XStack& stack, Data::Instruction::Type t,
                                   const std::string& fun)
    {
        Data::Instruction* top = getStackTop(stack);
        if(top->getType() != t)
            throw std::runtime_error("argument of incorrect type for " + fun);
        return top;
    }

    Data::Subroutine* findRoutine(const std::string& name, Data::Subroutine* parent,
                                   Data::SubTable& subs)
    {
        Data::Subroutine* current = parent;
        std::string head;
        std::string tail = name;
        do {
           head = getCallHead(tail);
           tail = getCallTail(tail);
           current = findSubroutine(subs, head, current); // throws if not found
        } while(head != tail);
        return current;
    }

    void x_show(Data::XStack& stack, SharedData&)
    {
        std::cout << getStackTop(stack)->getValue();
        std::cout.flush();
    }

    void x_ask(Data::XStack& stack, SharedData&)
    {
        std::string line;
        std::getline(std::cin, line);
        Data::Instruction* ins = new Data::Instruction(line);
        new_instructions.push_back(ins);
        stack.push(ins);
    }

    void x_pop(Data::XStack& stack, SharedData&)
    {
        stack.pop();
    }

    void x_swap(Data::XStack& stack, SharedData&)
    {
        Data::Instruction* first = getStackTop(stack);
        Data::Instruction* second = getStackTop(stack);
        stack.push(first);
        stack.push(second);
    }

    void x_duplicate(Data::XStack& stack, SharedData&)
    {
        stack.push(stack.top());
    }

    void x_if(Data::XStack& stack, SharedData& data)
    {
        Data::Instruction* top = getStackTop(stack, Data::Instruction::IntLit, "If");
        int value = boost::get<int>(top->getValue());
        if(!value)
            return;
        top = getStackTop(stack, Data::Instruction::StringLit, "If");
        std::string to_call = boost::get<std::string>(top->getValue());
        Data::Subroutine* routine = findRoutine(to_call, data.current, data.routines);
        Interpreter x(data.routines, data.modules, stack, routine);
        x.interpret();
    }

    void x_repeat(Data::XStack& stack, SharedData& data)
    {
        Data::Instruction* top = getStackTop(stack, Data::Instruction::IntLit, "Repeat");
        int value = boost::get<int>(top->getValue());
        if(!value)
            return;
        top = getStackTop(stack, Data::Instruction::StringLit, "Repeat");
        std::string to_call = boost::get<std::string>(top->getValue());
        Data::Subroutine* routine = findRoutine(to_call, data.current, data.routines);
        for(int i = 0; i < value; ++i) {
            Interpreter x(data.routines, data.modules, stack, routine);
            x.interpret();
        }
    }

    void x_add(Data::XStack& stack, SharedData& data)
    {
        Data::Instruction right = *getStackTop(stack);
        Data::Instruction left = *getStackTop(stack);
        Data::Instruction* ins = new Data::Instruction(left + right);
        new_instructions.push_back(ins);
        stack.push(ins);
    }

    void x_sub(Data::XStack& stack, SharedData& data)
    {
        Data::Instruction right = *getStackTop(stack);
        Data::Instruction left = *getStackTop(stack);
        Data::Instruction* ins = new Data::Instruction(left - right);
        new_instructions.push_back(ins);
        stack.push(ins);
    }

    void x_mul(Data::XStack& stack, SharedData&)
    {
        Data::Instruction right = *getStackTop(stack);
        Data::Instruction left = *getStackTop(stack);
        Data::Instruction* ins = new Data::Instruction(left * right);
        new_instructions.push_back(ins);
        stack.push(ins);
    }

    void x_div(Data::XStack& stack, SharedData&)
    {
        Data::Instruction right = *getStackTop(stack);
        Data::Instruction left = *getStackTop(stack);
        Data::Instruction* ins = new Data::Instruction(left / right);
        new_instructions.push_back(ins);
        stack.push(ins);
    }

    void x_setvar(Data::XStack& stack, SharedData&)
    {
        std::string name = getValue<std::string> (
            *getStackTop(stack), Data::Instruction::StringLit, "Variable.Set"
        );
        var_table[name] = getStackTop(stack);
    }

    void x_getvar(Data::XStack& stack, SharedData&)
    {
        std::string name = getValue<std::string>(
            *getStackTop(stack), Data::Instruction::StringLit, "Variable.Get"
        );
        if(var_table.count(name) > 0)
            stack.push(var_table[name]);
        else
            throw std::runtime_error("could not get nonexistant variable " + name);
    }

    void x_delvar(Data::XStack& stack, SharedData&)
    {
        std::string name = getValue<std::string>(
            *getStackTop(stack), Data::Instruction::StringLit, "Variable.Del"
        );
        if(var_table.count(name) > 0)
            var_table.erase(name);
        else
            throw std::runtime_error("could not remove nonexistant variable " + name);
    }

    void handleCall(const std::string& s, Data::XStack& stack, SharedData& data)
    {
        auto it = fun_table.find(s);
        if(it == fun_table.end())
            std::cerr << "Unkown subroutine in XCore: " << s << std::endl;
        try {
            (*it->second)(stack, data);
        } catch(std::exception& e) {
            std::cout << "XCore error:\n" << e.what() << std::endl;
        }
    }

    void registerCalls()
    {
        // IO
        fun_table["Show"] = x_show;
        fun_table["Ask"] = x_ask;
        // stack operations
        fun_table["Pop"] = x_pop;
        fun_table["Swap"] = x_swap;
        fun_table["Duplicate"] = x_duplicate;
        // control modifiers
        fun_table["If"] = x_if;
        fun_table["Repeat"] = x_repeat;
        // arithmetic
        fun_table["Add"] = x_add;
        fun_table["Sub"] = x_sub;
        fun_table["Mul"] = x_mul;
        fun_table["Div"] = x_div;
        // variables
        fun_table["Variable.Set"] = x_setvar;
        fun_table["Variable.Get"] = x_getvar;
        fun_table["Variable.Del"] = x_delvar;
    }

    void cleanUp()
    {
        for(Data::Instruction* i : new_instructions) {
            if(i != nullptr)
                delete i;
        }
    }
}

extern "C"
{
    bool load()
    {
        XCore::registerCalls();
        return true;
    }

    bool unload()
    {
        XCore::cleanUp();
        return true;
    }

    void call(const char* s, Data::XStack& stack, SharedData& data)
    {
        XCore::handleCall(s, stack, data);
    }
}
