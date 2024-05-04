#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <queue>
#include <iomanip>

using namespace std;

struct Process {
    int pid;
    int ppid;
    int priority;
    int value;
    int startTime;
    int cpuTimeUsed;
};

struct ComparePriority {
    bool operator()(const Process& p1, const Process& p2) {
        return p1.priority > p2.priority;
    }
};

struct SystemState {
    int currentTime;
    Process runningProcess;
    map<int, queue<Process>> readyProcesses;
    queue<Process> blockedProcesses;
};

void printSystemState(const SystemState& state) {
    cout << "****************************************************************" << endl;
    cout << "The current system state is as follows:" << endl;
    cout << "****************************************************************" << endl;
    cout << "CURRENT TIME: " << state.currentTime << endl;

    cout << "RUNNING PROCESS:" << endl;
    cout << "pid, ppid, priority, value, start time, CPU time used so far" << endl;
    if (state.runningProcess.pid != -1) {
        cout << state.runningProcess.pid << ", " << state.runningProcess.ppid << ", "
             << state.runningProcess.priority << ", " << state.runningProcess.value << ", "
             << state.runningProcess.startTime << ", " << state.runningProcess.cpuTimeUsed << endl;
    }

    cout << "BLOCKED PROCESSES:" << endl;
    cout << "Queue of blocked processes:" << endl;
    cout << "pid, ppid, priority, value, start time, CPU time used so far" << endl;
    queue<Process> blockedCopy = state.blockedProcesses;
    while (!blockedCopy.empty()) {
        Process p = blockedCopy.front();
        cout << p.pid << ", " << p.ppid << ", "
             << p.priority << ", " << p.value << ", "
             << p.startTime << ", " << p.cpuTimeUsed << endl;
        blockedCopy.pop();
    }

    cout << "PROCESSES READY TO EXECUTE:" << endl;
    for (const auto& pair : state.readyProcesses) {
        cout << "Queue of processes with priority " << pair.first << ":" << endl;
        cout << "pid, ppid, value, start time, CPU time used so far" << endl;
        queue<Process> readyCopy = pair.second;
        while (!readyCopy.empty()) {
            Process p = readyCopy.front();
            cout << p.pid << ", " << p.ppid << ", "
                 << p.value << ", " << p.startTime << ", " << p.cpuTimeUsed << endl;
            readyCopy.pop();
        }
    }
    cout << "****************************************************************" << endl;
}

int main() {
    SystemState state;
    state.currentTime = 0;
    state.runningProcess = {-1, -1, -1, -1, -1, -1}; // No process running
    priority_queue<Process, vector<Process>, ComparePriority> processQueue;

    while (true) {
        cout << "Enter a command (S <time>, A <pid>, D <pid>, F <pid>, R <filename>, E to exit): ";
        string input;
        getline(cin, input);
        istringstream iss(input);
        char command;
        iss >> command;
        if (command == 'E') {
            break;
        }

        switch (command) {
            case 'S': {
                int time;
                iss >> time;
                state.currentTime = time;
                break;
            }
            case 'A': {
                Process p;
                iss >> p.pid;
                processQueue.push(p);
                break;
            }
            case 'D': {
                int pid;
                iss >> pid;
                // Implement deletion logic
                break;
            }
            case 'F': {
                int pid;
                iss >> pid;
                // Implement fork logic
                break;
            }
            case 'R': {
                string filename;
                iss >> filename;
                // Implement read file logic
                break;
            }
            default:
                cout << "Invalid command." << endl;
        }

        // Update system state and print
        state.runningProcess = processQueue.empty() ? Process{-1, -1, -1, -1, -1, -1} : processQueue.top();
        printSystemState(state);
    }

    return 0;
}
