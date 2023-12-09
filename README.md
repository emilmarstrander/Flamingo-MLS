# Flamingo MLS
This repository contains the resulting software of my master's thesis. It contains the Flamingo MLS program and source files, compiled for use on Jetson and Ubuntu. 
We had issues with pre-compiling for Ubuntu, so you might have to re-compile for the specific Ubuntu machine used. We have compiled it for the Ground Control Station used in the tests.
The compiled program consist of the executable "flamingo_mls" and the configuration file "flamingo_mls.conf". "mlspp-main" is Cisco's MLS++ library. This is recompiled by running the "make" command inside the folder.
This required the an older version of OpenSSL. We used version 1.1. We compile Flamingo MLS by running "make" in the Flamingo MLS Ubuntu/Jetson folder. Flamingo MLS source files are located in the "source" folder, with header files in the "include" folder, and "main.cpp" in the main folder.


### Simulated Testing
The "Simulated Testing" folder contains files used during the simulated testing. It contains the following:
- Compiled MGEN executables for Jetson and Ubuntu.
- "callgrind.out.3137", which is the file used in QCachegrind to analyze CPU consumption.
- The base configuration file used during simulated testing. This is no longer a valid configuration file, since we changed the parameters after testing.
