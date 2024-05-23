#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>
#include <string>
#include <chrono>
#include <thread>

using namespace std;
using namespace std::chrono;

struct Process {
    int pid;
    int ppid;
    int priority;
    int value;
    double startTime;
    double cpuTimeUsed;
    int programCounter;
    vector<string> program;

    // Constructor to initialize a new process
    Process() : pid(0), ppid(0), priority(0), value(0), startTime(0), cpuTimeUsed(0), programCounter(0) {}
};

struct SystemState {
    double currentTime;
    double totalTime;
    int nextPid;
    int commandCounter;
    Process runningProcess;
    vector<Process> blockedProcesses;
    vector<Process> readyProcesses;

    // Constructor to initialize the system state
    SystemState() : currentTime(0), totalTime(0), nextPid(0), commandCounter(0) {
        // Initialize the initial running process
        runningProcess.pid = nextPid++;
        runningProcess.ppid = -1;  // Initial process has no parent
    }
};

bool executeInstruction(const string& instruction, SystemState& state) {
    auto start = high_resolution_clock::now();  // Start timing

    istringstream iss(instruction);
    char command;
    iss >> command;

    switch (command) {
        case 'S': {
            int n;
            iss >> n;
            state.runningProcess.value = n;
            break;
        }
        case 'A': {
            int n;
            iss >> n;
            state.runningProcess.value += n;
            break;
        }
        case 'D': {
            int n;
            iss >> n;
            state.runningProcess.value -= n;
            break;
        }
        case 'B': {
            state.blockedProcesses.push_back(state.runningProcess);
            state.runningProcess = Process();
            state.runningProcess.pid = state.nextPid++;
            state.runningProcess.ppid = -1;  // No parent as it is newly initialized
            break;
        }
        case 'E': {
            state.runningProcess = Process();
            state.runningProcess.pid = state.nextPid++;
            state.runningProcess.ppid = -1;  // No parent as it is newly initialized
            break;
        }
        case 'F': {
            int n;
            iss >> n;
            Process newProcess = state.runningProcess;
            newProcess.programCounter = n;
            newProcess.startTime = state.totalTime;  // Use total time as the start time for consistency
            newProcess.pid = state.nextPid++;
            newProcess.ppid = state.runningProcess.pid;
            state.readyProcesses.push_back(newProcess);
            state.runningProcess = Process();  // Reset running process
            state.runningProcess.pid = state.nextPid++;
            state.runningProcess.ppid = -1;  // No parent as it is newly initialized
            break;
        }
        case 'R': {
            string filename;
            iss >> filename;
            ifstream file(filename);
            if (file.is_open()) {
                string line;
                Process newProcess;
                newProcess.program.clear();
                newProcess.programCounter = 0;
                while (getline(file, line)) {
                    newProcess.program.push_back(line);
                }
                newProcess.startTime = state.totalTime;  // Use total time as the start time for consistency
                newProcess.pid = state.nextPid++;
                newProcess.ppid = state.runningProcess.pid;
                state.readyProcesses.push_back(newProcess);
            } else {
                cout << "Failed to open file: " << filename << endl;
            }
            break;
        }
        default: {
            // Handle invalid command
            cout << "Invalid command. Please enter a valid command." << endl;
            return false;
        }
    }

    auto end = high_resolution_clock::now();  // End timing
    duration<double> elapsed = end - start;
    state.currentTime = elapsed.count();  // Time taken for the current command
    state.totalTime += state.currentTime;  // Total time accumulated
    state.commandCounter++;  // Increment command counter

    // Delay to make the time more obvious
    this_thread::sleep_for(chrono::milliseconds(500));

    return true;
}

void printSystemState(const SystemState& state) {
    cout << "****************************************************************" << endl;
    cout << "The current system state is as follows:" << endl;
    cout << "****************************************************************" << endl;
    cout << "CURRENT TIME: " << state.currentTime << "s" << endl;
    cout << "Total TIME: " << state.totalTime << "s" << endl;
    cout << "Command Counter: " << state.commandCounter << endl;
    cout << "RUNNING PROCESS:" << endl;
    cout << "pid, ppid, priority, value, start time, CPU time used so far, program counter" << endl;
    cout << state.runningProcess.pid << ", " << (state.runningProcess.ppid == -1 ? "NULL" : to_string(state.runningProcess.ppid)) << ", " << state.runningProcess.priority << ", "
        << state.runningProcess.value << ", " << state.runningProcess.startTime << ", " << state.runningProcess.cpuTimeUsed << ", "
        << state.runningProcess.programCounter << endl;
    cout << "BLOCKED PROCESSES:" << endl;
    cout << "Queue of blocked processes:" << endl;
    cout << "pid, ppid, priority, value, start time, CPU time used so far, program counter" << endl;
    for (const auto& process : state.blockedProcesses) {
        cout << process.pid << ", " << (process.ppid == -1 ? "NULL" : to_string(process.ppid)) << ", " << process.priority << ", "
            << process.value << ", " << process.startTime << ", " << process.cpuTimeUsed << ", "
            <<
            process.programCounter << endl;
    }
    cout << "PROCESSES READY TO EXECUTE:" << endl;
    for (const auto& process : state.readyProcesses) {
        cout << process.pid << ", " << (process.ppid == -1 ? "NULL" : to_string(process.ppid
)) << ", " << process.priority << ", "
<< process.value << ", " << process.startTime << ", " << process.cpuTimeUsed << ", "
<< process.programCounter << endl;
}
cout << "****************************************************************" << endl;
}

int main() {
SystemState state;

string input;
while (true) {
    cout << "Enter a command (S <value>, A <value>, D <value>, B, E, F <value>, R <filename>, Q to quit): ";
    if (getline(cin, input)) {
        if (input == "Q") {
            break;
        }
        if (executeInstruction(input, state)) {
            printSystemState(state);
        }
    } else {
        break;
    }
}

return 0;
}