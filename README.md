SJSU CS-149 Project Description:

Enviroment：
When attempting to run `simulated_os`, compiled using GLIBC 2.31, on Ubuntu version 20.04.6, compatibility issues may arise. This is because Ubuntu 20.04.6 comes pre-installed with GLIBC 2.31, and the GLIBC 2.34 version you mentioned contains new features and changes that may cause the program to not run correctly with the older version of GLIBC.

If you encounter issues while running, consider recompiling `simulated_os.cpp` to use the GLIBC version installed on your system. This ensures that the compiled program is compatible with your system environment and avoids compatibility problems.
