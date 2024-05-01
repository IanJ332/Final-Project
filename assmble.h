// assmble.h
#ifndef ASSMBLE_H
#define ASSMBLE_H

#include <vector>
#include <string>

class Instruction {
public:
    char operation;
    int intArg;
    std::string stringArg;

    Instruction(char op, int intArg, const std::string& stringArg)
        : operation(op), intArg(intArg), stringArg(stringArg) {}
};

class Cpu {
public:
    std::vector<Instruction>* pProgram;
    int programCounter;
    int value;
    int timeSlice;
    int timeSliceUsed;

    Cpu() : pProgram(nullptr), programCounter(0), value(0), timeSlice(0), timeSliceUsed(0) {}
};

enum State {
    STATE_READY,
    STATE_RUNNING,
    STATE_BLOCKED
};

class PcbEntry {
public:
    int processId;
    int parentProcessId;
    std::vector<Instruction> program;
    unsigned int programCounter;
    int value;
    unsigned int priority;
    State state;
    unsigned int startTime;
    unsigned int timeUsed;

    PcbEntry() : processId(0), parentProcessId(0), programCounter(0), value(0), priority(0), state(STATE_READY), startTime(0), timeUsed(0) {}
};

#endif // ASSMBLE_H
