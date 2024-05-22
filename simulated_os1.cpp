#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>
#include <string>
using namespace std;

struct Process {
    int pid;
    int ppid;
    int priority;
    int value;
    int startTime;
    int cpuTimeUsed;
    int programCounter;
    vector<string> program;
};

struct SystemState {
    int currentTime;
    Process runningProcess;
    vector<Process> blockedProcesses;
    vector<Process> readyProcesses;
};

bool executeInstruction(const string& instruction, SystemState& state) {
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
            break;
        }
        case 'E': {
            state.runningProcess = Process();
            break;
        }
        case 'F': {
            int n;
            iss >> n;
            Process newProcess = state.runningProcess;
            newProcess.programCounter = n;
            newProcess.startTime = state.currentTime;
            newProcess.pid = state.readyProcesses.size() + state.blockedProcesses.size() + 1;
            newProcess.ppid = state.runningProcess.pid;
            state.readyProcesses.push_back(newProcess);
            break;
        }
        case 'R': {
            string filename;
            iss >> filename;
            ifstream file(filename);
            if (file.is_open()) {
                string line;
                Process newProcess = state.runningProcess;
                newProcess.program.clear();
                newProcess.programCounter = 0;
                while (getline(file, line)) {
                    newProcess.program.push_back(line);
                }
                newProcess.startTime = state.currentTime;
                newProcess.pid = state.readyProcesses.size() + state.blockedProcesses.size() + 1;
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
    state.currentTime++;
    return true;
}

void printSystemState(const SystemState& state) {
    cout << "****************************************************************" << endl;
    cout << "The current system state is as follows:" << endl;
    cout << "****************************************************************" << endl;
    cout << "CURRENT TIME: " << state.currentTime << endl;
    cout << "RUNNING PROCESS:" << endl;
    cout << "pid, ppid, priority, value, start time, CPU time used so far, program counter" << endl;
    cout << state.runningProcess.pid << ", " << state.runningProcess.ppid << ", " << state.runningProcess.priority << ", "
        << state.runningProcess.value << ", " << state.runningProcess.startTime << ", " << state.runningProcess.cpuTimeUsed << ", "
        << state.runningProcess.programCounter << endl;
    cout << "BLOCKED PROCESSES:" << endl;
    cout << "Queue of blocked processes:" << endl;
    cout << "pid, ppid, priority, value, start time, CPU time used so far, program counter" << endl;
    for (const auto& process : state.blockedProcesses) {
        cout << process.pid << ", " << process.ppid << ", " << process.priority << ", "
            << process.value << ", " << process.startTime << ", " << process.cpuTimeUsed << ", "
            << process.programCounter << endl;
    }
    cout << "PROCESSES READY TO EXECUTE:" << endl;
    for (const auto& process : state.readyProcesses) {
        cout << process.pid << ", " << process.ppid << ", " << process.priority << ", "
            << process.value << ", " << process.startTime << ", " << process.cpuTimeUsed << ", "
            << process.programCounter << endl;
    }
    cout << "****************************************************************" << endl;
}

int main() {
    SystemState state;
    state.currentTime = 0;

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
