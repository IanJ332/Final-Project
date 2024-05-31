// runProcessManager.cpp

#include "assmble.h"
#include <iostream>
#include <unistd.h>

extern std::vector<PcbEntry> pcbTable;
extern std::vector<int> blockedProcesses;
extern std::vector<int> readyProcesses;
extern int runningProcessIndex;

int runProcessManager(int pipeDescriptor) {
    char ch;
    int pid, ppid, value, startTime, cpuTimeUsed;

    // Implement the process manager logic here
    while (::read(pipeDescriptor, &ch, sizeof(ch)) > 0) {
        switch (ch) {
            case 'RP':
                // Handle Running Process command
                // Assume you have the index of the running process in pcbTable
                std::cout << "Running Process: "
                          << "pid-" << pcbTable[runningProcessIndex].processId << ", "
                          << "ppid-" << pcbTable[runningProcessIndex].parentProcessId << ", "
                          << "value-" << pcbTable[runningProcessIndex].value << ", "
                          << "start time-" << pcbTable[runningProcessIndex].startTime << ", "
                          << "CPU time used so far-" << pcbTable[runningProcessIndex].timeUsed << std::endl;
                break;
            case 'BP':
                // Handle Blocked Process command
                // Assume you have the index of the blocked process in blockedProcesses
                std::cout << "Blocked Process: "
                          << "pid-" << pcbTable[blockedProcesses[0]].processId << ", "
                          << "ppid-" << pcbTable[blockedProcesses[0]].parentProcessId << ", "
                          << "value-" << pcbTable[blockedProcesses[0]].value << ", "
                          << "start time-" << pcbTable[blockedProcesses[0]].startTime << ", "
                          << "CPU time used so far-" << pcbTable[blockedProcesses[0]].timeUsed << std::endl;
                break;
            case 'PR2':
                // Handle Processes Ready to Execute command
                // Assume you have the index of the ready process in readyProcesses
                std::cout << "Processes Ready to Execute: "
                          << "pid-" << pcbTable[readyProcesses[0]].processId << ", "
                          << "ppid-" << pcbTable[readyProcesses[0]].parentProcessId << ", "
                          << "value-" << pcbTable[readyProcesses[0]].value << ", "
                          << "start time-" << pcbTable[readyProcesses[0]].startTime << ", "
                          << "CPU time used so far-" << pcbTable[readyProcesses[0]].timeUsed << std::endl;
                break;
            default:
                break;
        }
    }

    return 0;
}
