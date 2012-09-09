#ifndef _X_DATA_H_INCLUDE_GUARD
#define _X_DATA_H_INCLUDE_GUARD

#include <string>
#include <vector>
#include <ostream>
#include <stack>
#include <boost/variant.hpp>

namespace Data {
    class Subroutine;
    class Instruction;
    typedef std::vector<Subroutine*> SubTable;
    typedef std::vector<Instruction*> InstructionTable;
    typedef std::stack<Data::Instruction*> XStack;
    struct Instruction {
        typedef boost::variant<char, int, double, std::string> Value;
        enum Type {
            Undefined, CharLit, IntLit, DoubleLit, StringLit, CodeLit, Call
        };

        Instruction();
        Instruction(const std::string& v, bool call = false);
        Instruction(char c);
        Instruction(int n);
        Instruction(double n);

        Value getValue() const;
        Type getType() const;
    private:
        Type kind;
        Value value;
    };

    std::ostream& operator<<(std::ostream& os, const Instruction& ins);

    class Subroutine {
        Subroutine* parent;
        std::string name;
        InstructionTable instructions;
    public:
        Subroutine(Subroutine* p, const std::string& n)
            : parent(p), name(n), instructions() {}
        Subroutine(const Subroutine&) = delete;
        Subroutine& operator=(Subroutine&) = delete;
        ~Subroutine();

        void addInstruction(Instruction* i);

        bool hasParent() const;
        Subroutine* getParent() const;
        std::string getName() const;
        InstructionTable getInstructions() const;
    };

}

#endif // _X_DATA_H_INCLUDE_GUARD

