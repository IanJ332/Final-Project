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

string trim(string trimmed_str);

PcbEntry pcbEntry[10];
unsigned int timestamp = 0;
Cpu cpu;

// For the states below, -1 indicates empty (since it is an invalid index).
int runningState = -1;
deque<int> readyState;
deque<int> blockedState;

// In this implementation, we'll never explicitly clear PCB entries and the index in
// the table will always be the process ID. These choices waste memory, but since this
// program is just a simulation it the easiest approach. Additionally, debugging is
// simpler since table slots and process IDs are never re-used.

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

        line = trim(line);

        if (line.size() > 0) {
            Instruction instruction;
            instruction.operation = toupper(line[0]);
            instruction.stringArg = trim(line.erase(0, 1));

            stringstream argStream(instruction.stringArg);
            switch (instruction.operation) {
                case 'S': // Integer argument.
                case 'A': // Integer argument.
                case 'D': // Integer argument.
                case 'F': // Integer argument.
                    if (!(argStream >> instruction.intArg)) {
                        cout << filename << ":" << lineNum
                             << " - Invalid integer argument "
                             << instruction.stringArg << " for "
                             << instruction.operation << " operation"
                             << endl;
                        file.close();
                        return false;
                    }
                    break;
                case 'B': // No argument.
                case 'E': // No argument.
                    break;
                case 'R': // String argument.
                    // Note that since the string is trimmed on both ends, filenames
                    // with leading or trailing whitespace (unlikely) will not work.
                    if (instruction.stringArg.size() == 0) {
                        cout << filename << ":" << lineNum << " -Missing string argument"
                             << endl;
                        file.close();
                        return false;
                    }
                    break;
                default:
                    cout << filename << ":" << lineNum << " - Invalid operation, "
                         << instruction.operation << endl;
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

/**
 * Removes whitespace at the start and the end of the string
 * @param trimmed_str the string which needs to be trimmed.
 * @return the trimmed string without whitespaces at beginning and end of the string.
 */
string trim(string trimmed_str)
{
    int front = trimmed_str.find_first_not_of(' ');
    int rear = trimmed_str.find_last_not_of(' ');

    if (rear == -1 || front == -1) {
        return "";
    }

    return trimmed_str.substr(front, rear - front + 1);
}

// Implements the S operation.
void set(int value) {
    cpu.value = value;
    cout << "Set CPU value to " << value << endl;
}

// Implements the A operation.
void add(int value) {
    cpu.value += value;
    cout << "Added " << value << " to CPU value" << endl;
}

// Implements the D operation.
void decrement(int value) {
    cpu.value -= value;
    cout << "Decremented CPU value by " << value << endl;
}

// Performs scheduling.
void schedule() {
    if (runningState != -1) {
        cout << "A process is already running" << endl;
        return;
    }

    if (!readyState.empty()) {
        int nextProcess = readyState.front();
        readyState.pop_front();

        pcbEntry[nextProcess].state = STATE_RUNNING;
        runningState = nextProcess;

        cpu.pProgram = &(pcbEntry[nextProcess].program);
        cpu.programCounter = pcbEntry[nextProcess].programCounter;
        cpu.value = pcbEntry[nextProcess].value;

        cout << "Scheduled process " << nextProcess << " to run" << endl;
    } else {
        cout << "No processes ready to run" << endl;
    }
}

// Implements the B operation.
void block() {
    if (runningState == -1) {
        cout << "No process is running" << endl;
        return;
    }

    blockedState.push_back(runningState);
    pcbEntry[runningState].state = STATE_BLOCKED;
    pcbEntry[runningState].programCounter = cpu.programCounter;
    pcbEntry[runningState].value = cpu.value;
    runningState = -1;

    cout << "Blocked current process" << endl;
}

// Implements the E operation.
void end() {
    if (runningState == -1) {
        cout << "No process is running" << endl;
        return;
    }

    unsigned int turnaroundTime = timestamp + 1 - pcbEntry[runningState].startTime;
    cumulativeTimeDiff += turnaroundTime;
    numTerminatedProcesses++;

    pcbEntry[runningState].state = STATE_READY;
    readyState.push_back(runningState);
    runningState = -1;

    cout << "Process terminated with turnaround time " << turnaroundTime << endl;
}

// Implements the F operation.
void fork(int value) {
    if (runningState == -1) {
        cout << "No process is running" << endl;
        return;
    }

    int newProcessId = static_cast<int>(pcbEntry.size());
    pcbEntry.resize(newProcessId + 1);

    pcbEntry[newProcessId].processId = newProcessId;
    pcbEntry[newProcessId].parentProcessId = runningState;
    pcbEntry[newProcessId].program = pcbEntry[runningState].program;
    pcbEntry[newProcessId].programCounter = cpu.programCounter;
    pcbEntry[newProcessId].value = cpu.value;
    pcbEntry[newProcessId].priority = pcbEntry[runningState].priority;
    pcbEntry[newProcessId].state = STATE_READY;
    pcbEntry[newProcessId].startTime = timestamp;
    pcbEntry[newProcessId].timeUsed = 0;

    readyState.push_back(newProcessId);
    cpu.programCounter += value;

    cout << "Forked new process " << newProcessId << endl;
}

// Implements the R operation.
void replace(string &argument) {
    cpu.pProgram->clear();
    if (!createProgram(argument, *cpu.pProgram)) {
        cpu.programCounter++;
        return;
    }
    cpu.programCounter = 0;

    cout << "Replaced program with file " << argument << endl;
}

// Implements the Q command.
void quantum() {
    if (runningState == -1) {
        cout << "No process is running" << endl;
        return;
    }

    if (cpu.programCounter < cpu.pProgram->size()) {
        Instruction instruction = (*cpu.pProgram)[cpu.programCounter];
        cpu.programCounter++;

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
            default:
                cout << "Invalid operation: " << instruction.operation << endl;
        }
    } else {
        cout << "End of program reached" << endl;
        end();
    }

    schedule();
}

// Implements the U command.
void unblock() {
    if (!blockedState.empty()) {
        int unblockedProcess = blockedState.front();
        blockedState.pop_front();
        pcbEntry[unblockedProcess].state = STATE_READY;
        readyState.push_back(unblockedProcess);
        cout << "Unblocked process " << unblockedProcess << endl;
    } else {
        cout << "No processes to unblock" << endl;
    }
}

// Implements the P command.
void print() {
    cout << "Cumulative Time Difference: " << cumulativeTimeDiff << endl;
    cout << "Number of Terminated Processes: " << numTerminatedProcesses << endl;
    // Print PCB table
    for (int i = 0; i < 10; i++) {
        if (pcbEntry[i].state != STATE_READY) {
            cout << "Process ID: " << pcbEntry[i].processId << endl;
            cout << "Parent Process ID: " << pcbEntry[i].parentProcessId << endl;
            cout << "Program Counter: " << pcbEntry[i].programCounter << endl;
            cout << "Value: " << pcbEntry[i].value << endl;
            cout << "Priority: " << pcbEntry[i].priority << endl;
            cout << "State: " << pcbEntry[i].state << endl;
            cout << "Start Time: " << pcbEntry[i].startTime << endl;
            cout << "Time Used: " << pcbEntry[i].timeUsed << endl;
            cout << endl;
        }
    }
}

// Function that implements the process manager.
int runProcessManager(int fileDescriptor) {
    // Attempt to create the init process.
    if (!createProgram("init", pcbEntry[0].program)) {
        return EXIT_FAILURE;
    }

    pcbEntry[0].processId = 0;
    pcbEntry[0].parentProcessId = -1;
    pcbEntry[0].programCounter = 0;
    pcbEntry[0].value = 0;
    pcbEntry[0].priority = 0;
    pcbEntry[0].state = STATE_RUNNING;
    pcbEntry[0].startTime = 0;
    pcbEntry[0].timeUsed = 0;

    runningState = 0;

    cpu.pProgram = &(pcbEntry[0].program);
    cpu.programCounter = pcbEntry[0].programCounter;
    cpu.value = pcbEntry[0].value;
    timestamp = 0;
    double avgTurnaroundTime = 0;

    // Loop until a 'T' is read, then terminate.
    char ch;
    do {
        // Read a command character from the pipe.
        if (read(fileDescriptor, &ch, sizeof(ch)) != sizeof(ch)) {
            // Assume the parent process exited, breaking the pipe.
            break;
        }

        switch (ch) {
            case 'Q':
                quantum();
                break;
            case 'U':
                unblock();
                break;
            case 'P':
                print();
                break;
            default:
                cout << "Invalid command: " << ch << endl;
        }
    } while (ch != 'T');

    return EXIT_SUCCESS;
}

int main(int argc, char *argv[]) {
    int pipeDescriptors[2];
    pid_t processMgrPid;
    char ch;
    int result;


    // Create a pipe
    pipe(pipeDescriptors);

    // Use fork() to create the child process and save the value returned in processMgrPid variable
    if ((processMgrPid = fork()) == -1) exit(1); /* FORK FAILED */
    if (processMgrPid == 0) {
        // The process manager process is running.
        // Close the unused write end of the pipe for the process manager process.
        close(pipeDescriptors[1]);

        // Run the process manager.
        result = runProcessManager(pipeDescriptors[0]);

        // Close the read end of the pipe for the process manager process (for cleanup purposes).
        close(pipeDescriptors[0]);
        _exit(result);
    } else {
        // The commander process is running.
        // Close the unused read end of the pipe for the commander process.
        close(pipeDescriptors[0]);
        // Loop until a 'T' is written or until the pipe is broken.
        do {
            cout << "Enter Q, P, U or T" << endl;
            cout << "$ ";
            cin >> ch ;

            // Pass commands to the process manager process via the pipe.
            if (write(pipeDescriptors[1], &ch, sizeof(ch)) != sizeof(ch)) {
                // Assume the child process exited, breaking the pipe.
                break;
            }
        } while (ch != 'T');

        write(pipeDescriptors[1], &ch, sizeof(ch));
        // Close the write end of the pipe for the commander process (for cleanup purposes).
        close(pipeDescriptors[1]);
        // Wait for the process manager to exit.
        wait(&result);
    }

    return result;
}
