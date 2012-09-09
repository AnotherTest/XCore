#include <unordered_map>
#include "Data.h"
#include "Interpreter.h"

namespace XCore {
    typedef void (*XFunction) (Data::XStack&, SharedData&);
    std::unordered_map<std::string, XFunction> fun_table;

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
        std::cout << getStackTop(stack)->getValue() << std::endl;
    }

    void x_pop(Data::XStack& stack, SharedData&)
    {
        stack.pop();
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
        Interpreter x(data.routines, data.modules, routine);
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
            Interpreter x(data.routines, data.modules, routine);
            x.interpret();
        }
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
        fun_table["Show"] = x_show;
        fun_table["Pop"] = x_pop;
        fun_table["If"] = x_if;
        fun_table["Repeat"] = x_repeat;
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
        return true;
    }

    void call(const char* s, Data::XStack& stack, SharedData& data)
    {
        XCore::handleCall(s, stack, data);
    }
}
