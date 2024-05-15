#include <cctype> // for toupper()
#include <cstdlib> // for EXIT_SUCCESS and EXIT_FAILURE
#include <cstring> // for strerror()
#include <cerrno> // for errno
#include <deque> // for deque (used for ready and blocked queues)
#include <fstream> // for ifstream (used for reading simulated programs)
#include <iostream> // for cout, endl, and cin
#include <sstream> // for stringstream (used for parsing simulated programs)
#include <sys/wait.h> // for wait()
#include <unistd.h> // for pipe(), read(), write(), close(), fork(), and _exit()
#include <vector> // for vector (used for PCB table)
using namespace std;

#include <algorithm>
#include <cctype>

#include <locale>

// trim from start (in place)
static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}

class Instruction {
public:
    char operation;
    int intArg;
    string stringArg;
};

class Cpu {
public:
    vector<Instruction> *pProgram;
    int programCounter;
    int value;
    int timeSlice;
    int timeSliceUsed;
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
    vector<Instruction> program;
    unsigned int programCounter;
    int value;
    unsigned int priority;
    State state;
    unsigned int startTime;
    unsigned int timeUsed;
};

PcbEntry pcbEntry[10];
unsigned int timestamp = 0;
Cpu cpu;

int runningState = -1;
deque<int> readyState;
deque<int> blockedState;

double cumulativeTimeDiff = 0;
int numTerminatedProcesses = 0;

bool createProgram(const string &filename, vector<Instruction> &program) {
    ifstream file;
    int lineNum = 0;
    file.open(filename.c_str());
    if (!file.is_open()) {
        cout << "Error opening file " << filename << endl;
        return false;
    }
    while (file.good()) {
        string line;
        getline(file, line);
        trim(line);
        if (line.size() > 0) {
            Instruction instruction;
            instruction.operation = toupper(line[0]);
            instruction.stringArg = trim(line.erase(0, 1));
            stringstream argStream(instruction.stringArg);
            switch (instruction.operation) {
                case 'S':
                case 'A':
                case 'D':
                case 'F':
                    if (!(argStream >> instruction.intArg)) {
                        cout << filename << ":" << lineNum << " - Invalid integer argument " << instruction.stringArg << " for " << instruction.operation << " operation" << endl;
                        file.close();
                        return false;
                    }
                    break;
                case 'B':
                case 'E':
                    break;
                case 'R':
                    if (instruction.stringArg.size() == 0) {
                        cout << filename << ":" << lineNum << " - Missing string argument" << endl;
                        file.close();
                        return false;
                    }
                    break;
                default:
                    cout << filename << ":" << lineNum << " - Invalid operation, " << instruction.operation << endl;
                    file.close();
                    return false;
            }
            program.push_back(instruction);
        }
        lineNum++;
    }
    file.close();
    return true;
}

void set(int value) {
    cpu.value = value;
}

void add(int value) {
    cpu.value += value;
}

void decrement(int value) {
    cpu.value -= value;
}

void schedule() {
    if (runningState != -1) {
        return;
    }
    if (!readyState.empty()) {
        runningState = readyState.front();
        readyState.pop_front();
        pcbEntry[runningState].state = STATE_RUNNING;
        cpu.pProgram = &(pcbEntry[runningState].program);
        cpu.programCounter = pcbEntry[runningState].programCounter;
        cpu.value = pcbEntry[runningState].value;
    }
}

void block() {
    if (runningState != -1) {
        blockedState.push_back(runningState);
        pcbEntry[runningState].state = STATE_BLOCKED;
        pcbEntry[runningState].programCounter = cpu.programCounter;
        pcbEntry[runningState].value = cpu.value;
        runningState = -1;
    }
}

void end() {
    if (runningState != -1) {
        PcbEntry& entry = pcbEntry[runningState];
        cumulativeTimeDiff += timestamp + 1 - entry.startTime;
        numTerminatedProcesses++;
        entry.state = STATE_READY;
        entry.programCounter = 0;
        entry.value = 0;
        runningState = -1;
    }
}

void fork(int value) {
    int newPid = pcbEntry[runningState].processId + 1;
    if (newPid >= 10) {
        cout << "Cannot fork, too many processes" << endl;
        return;
    }
    PcbEntry& parentEntry = pcbEntry[runningState];
    PcbEntry& childEntry = pcbEntry[newPid];
    childEntry.processId = newPid;
    childEntry.parentProcessId = parentEntry.processId;
    childEntry.program = parentEntry.program;
    childEntry.programCounter = parentEntry.programCounter;
    childEntry.value = parentEntry.value;
    childEntry.priority = parentEntry.priority;
    childEntry.state = STATE_READY;
    childEntry.startTime = timestamp;
    readyState.push_back(newPid);
    cpu.programCounter += value;
}

void replace(string &argument) {
    cpu.pProgram->clear();
    if (!createProgram(argument, *cpu.pProgram)) {
        cpu.programCounter++;
    }
    cpu.programCounter = 0;
}

void quantum() {
    if (runningState == -1) {
        cout << "No processes are running" << endl;
        ++timestamp;
        return;
    }
    if (cpu.programCounter < cpu.pProgram->size()) {
        Instruction instruction = (*cpu.pProgram)[cpu.programCounter];
        ++cpu.programCounter;
        switch (instruction.operation) {
            case 'S':
                set(instruction.intArg);
                break;
            case 'A':
                add(instruction.intArg);
                break;
            case 'D':
                decrement(instruction.intArg);
                break;
            case 'B':
                block();
                break;
            case 'E':
                end();
                break;
            case 'F':
                fork(instruction.intArg);
                break;
            case 'R':
                replace(instruction.stringArg);
                break;
        }
    } else {
        cout << "End of program reached without E operation" << endl;
        end();
    }
    ++timestamp;
    schedule();
}

void unblock() {
    if (!blockedState.empty()) {
        int pid = blockedState.front();
        blockedState.pop_front();
        pcbEntry[pid].state = STATE_READY;
        readyState.push_back(pid);
        schedule();
    }
}

void print() {
    cout << "Print command is not implemented until iLab 3" << endl;
}

int runProcessManager(int fileDescriptor) {
    // Implementation omitted for brevity
}

int main(int argc, char *argv[]) {
    // Implementation omitted for brevity
}
