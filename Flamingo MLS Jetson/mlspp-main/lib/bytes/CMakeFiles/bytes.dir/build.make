# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/MLS/mlspp-main

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/MLS/mlspp-main

# Include any dependencies generated for this target.
include lib/bytes/CMakeFiles/bytes.dir/depend.make

# Include the progress variables for this target.
include lib/bytes/CMakeFiles/bytes.dir/progress.make

# Include the compile flags for this target's objects.
include lib/bytes/CMakeFiles/bytes.dir/flags.make

lib/bytes/CMakeFiles/bytes.dir/src/bytes.cpp.o: lib/bytes/CMakeFiles/bytes.dir/flags.make
lib/bytes/CMakeFiles/bytes.dir/src/bytes.cpp.o: lib/bytes/src/bytes.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/MLS/mlspp-main/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object lib/bytes/CMakeFiles/bytes.dir/src/bytes.cpp.o"
	cd /home/MLS/mlspp-main/lib/bytes && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/bytes.dir/src/bytes.cpp.o -c /home/MLS/mlspp-main/lib/bytes/src/bytes.cpp

lib/bytes/CMakeFiles/bytes.dir/src/bytes.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/bytes.dir/src/bytes.cpp.i"
	cd /home/MLS/mlspp-main/lib/bytes && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/MLS/mlspp-main/lib/bytes/src/bytes.cpp > CMakeFiles/bytes.dir/src/bytes.cpp.i

lib/bytes/CMakeFiles/bytes.dir/src/bytes.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/bytes.dir/src/bytes.cpp.s"
	cd /home/MLS/mlspp-main/lib/bytes && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/MLS/mlspp-main/lib/bytes/src/bytes.cpp -o CMakeFiles/bytes.dir/src/bytes.cpp.s

lib/bytes/CMakeFiles/bytes.dir/src/bytes.cpp.o.requires:

.PHONY : lib/bytes/CMakeFiles/bytes.dir/src/bytes.cpp.o.requires

lib/bytes/CMakeFiles/bytes.dir/src/bytes.cpp.o.provides: lib/bytes/CMakeFiles/bytes.dir/src/bytes.cpp.o.requires
	$(MAKE) -f lib/bytes/CMakeFiles/bytes.dir/build.make lib/bytes/CMakeFiles/bytes.dir/src/bytes.cpp.o.provides.build
.PHONY : lib/bytes/CMakeFiles/bytes.dir/src/bytes.cpp.o.provides

lib/bytes/CMakeFiles/bytes.dir/src/bytes.cpp.o.provides.build: lib/bytes/CMakeFiles/bytes.dir/src/bytes.cpp.o


# Object files for target bytes
bytes_OBJECTS = \
"CMakeFiles/bytes.dir/src/bytes.cpp.o"

# External object files for target bytes
bytes_EXTERNAL_OBJECTS =

lib/bytes/libbytes.a: lib/bytes/CMakeFiles/bytes.dir/src/bytes.cpp.o
lib/bytes/libbytes.a: lib/bytes/CMakeFiles/bytes.dir/build.make
lib/bytes/libbytes.a: lib/bytes/CMakeFiles/bytes.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/MLS/mlspp-main/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libbytes.a"
	cd /home/MLS/mlspp-main/lib/bytes && $(CMAKE_COMMAND) -P CMakeFiles/bytes.dir/cmake_clean_target.cmake
	cd /home/MLS/mlspp-main/lib/bytes && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/bytes.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
lib/bytes/CMakeFiles/bytes.dir/build: lib/bytes/libbytes.a

.PHONY : lib/bytes/CMakeFiles/bytes.dir/build

lib/bytes/CMakeFiles/bytes.dir/requires: lib/bytes/CMakeFiles/bytes.dir/src/bytes.cpp.o.requires

.PHONY : lib/bytes/CMakeFiles/bytes.dir/requires

lib/bytes/CMakeFiles/bytes.dir/clean:
	cd /home/MLS/mlspp-main/lib/bytes && $(CMAKE_COMMAND) -P CMakeFiles/bytes.dir/cmake_clean.cmake
.PHONY : lib/bytes/CMakeFiles/bytes.dir/clean

lib/bytes/CMakeFiles/bytes.dir/depend:
	cd /home/MLS/mlspp-main && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/MLS/mlspp-main /home/MLS/mlspp-main/lib/bytes /home/MLS/mlspp-main /home/MLS/mlspp-main/lib/bytes /home/MLS/mlspp-main/lib/bytes/CMakeFiles/bytes.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : lib/bytes/CMakeFiles/bytes.dir/depend

