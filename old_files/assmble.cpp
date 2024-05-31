// assmble.cpp

#include "assmble.h"

std::vector<PcbEntry> pcbTable; // Global PCB table
std::vector<int> blockedProcesses; // Global list of blocked processes
std::vector<int> readyProcesses; // Global list of processes ready to execute
int runningProcessIndex; // Index of the running process in pcbTable

void printSystemState() {
    // Print the header
    std::cout << "****************************************************************" << std::endl;
    std::cout << "The current system state is as follows:" << std::endl;
    std::cout << "****************************************************************" << std::endl;

    // Print current time
    std::cout << "CURRENT TIME: time" << std::endl;

    // Print running process
    std::cout << "RUNNING PROCESS:" << std::endl;
    std::cout << pcbTable[runningProcessIndex].processId << ", "
         << pcbTable[runningProcessIndex].parentProcessId << ", "
         << pcbTable[runningProcessIndex].priority << ", "
         << pcbTable[runningProcessIndex].value << ", "
         << pcbTable[runningProcessIndex].startTime << ", "
         << pcbTable[runningProcessIndex].timeUsed << std::endl;

    // Print blocked processes
    std::cout << "BLOCKED PROCESSES:" << std::endl;
    for (int i = 0; i < blockedProcesses.size(); ++i) {
        std::cout << pcbTable[blockedProcesses[i]].processId << ", "
             << pcbTable[blockedProcesses[i]].parentProcessId << ", "
             << pcbTable[blockedProcesses[i]].priority << ", "
             << pcbTable[blockedProcesses[i]].value << ", "
             << pcbTable[blockedProcesses[i]].startTime << ", "
             << pcbTable[blockedProcesses[i]].timeUsed << std::endl;
    }

    // Print processes ready to execute
    std::cout << "PROCESSES READY TO EXECUTE:" << std::endl;
    for (int i = 0; i < readyProcesses.size(); ++i) {
        std::cout << pcbTable[readyProcesses[i]].processId << ", "
             << pcbTable[readyProcesses[i]].parentProcessId << ", "
             << pcbTable[readyProcesses[i]].priority << ", "
             << pcbTable[readyProcesses[i]].value << ", "
             << pcbTable[readyProcesses[i]].startTime << ", "
             << pcbTable[readyProcesses[i]].timeUsed << std::endl;
    }

    std::cout << "****************************************************************" << std::endl;
}
