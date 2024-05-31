// main.cpp

#include <iostream>
#include <vector>
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include "assmble.h"

using namespace std;

int runProcessManager(int pipeDescriptor); // Forward declaration

int main() {
    int pipeDescriptors[2];
    pid_t processMgrPid;
    char ch;
    int result;

    // Create a pipe
    pipe(pipeDescriptors);

    // Use fork() system call to create the child process and save the value returned in processMgrPid variable
    if ((processMgrPid = fork()) == -1) {
        exit(1); /* FORK FAILED */
    }

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
            cout << "Enter RP(Running Process), BP(Blocked Process), PR2(Processes Ready to Execute)" << endl;
            cout << "$ ";
            cin >> ch;

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
