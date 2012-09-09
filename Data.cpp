#include "Data.h"

namespace Data {
    // Instruction implementation starts here

    Instruction::Instruction()
        : kind(Undefined), value() {}

    Instruction::Instruction(const std::string& v, bool call)
        : kind(call ? Call : StringLit), value(v) { }

    Instruction::Instruction(char c)
        : kind(CharLit), value(c) { }
    Instruction::Instruction(int n)
        : kind(IntLit), value(n) { }
    Instruction::Instruction(double n)
        : kind(DoubleLit), value(n) { }

    Instruction::Value Instruction::getValue() const
    {
        return value;
    }

    Instruction::Type Instruction::getType() const
    {
        return kind;
    }


    std::ostream& operator<<(std::ostream& os, const Instruction& ins)
    {
        switch(ins.getType()) {
            case Instruction::CharLit:
                os << "(Character)";
                break;
            case Instruction::IntLit:
                os << "(Integer)";
                break;
            case Instruction::DoubleLit:
                os << "(Real)";
                break;
            case Instruction::StringLit:
                os << "(String)";
                break;
            case Instruction::Call:
                os << "(Call)";
                break;
            default:
                os << "(Undefined)";
                break;
        }
        return os << ins.getValue();
    }

    // Subroutine implementation starts here

    Subroutine::~Subroutine()
    {
        for(Instruction* ins : instructions) {
            if(ins != nullptr)
                delete ins;
        }
    }

    void Subroutine::addInstruction(Instruction* i)
    {
        instructions.push_back(i);
    }

    bool Subroutine::hasParent() const
    {
        return parent != nullptr;
    }

    Subroutine* Subroutine::getParent() const
    {
        return parent;
    }

    std::string Subroutine::getName() const
    {
        return name;
    }

    InstructionTable Subroutine::getInstructions() const
    {
        return instructions;
    }
}
