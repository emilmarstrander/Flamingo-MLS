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
include lib/mls_vectors/CMakeFiles/mls_vectors.dir/depend.make

# Include the progress variables for this target.
include lib/mls_vectors/CMakeFiles/mls_vectors.dir/progress.make

# Include the compile flags for this target's objects.
include lib/mls_vectors/CMakeFiles/mls_vectors.dir/flags.make

lib/mls_vectors/CMakeFiles/mls_vectors.dir/src/mls_vectors.cpp.o: lib/mls_vectors/CMakeFiles/mls_vectors.dir/flags.make
lib/mls_vectors/CMakeFiles/mls_vectors.dir/src/mls_vectors.cpp.o: lib/mls_vectors/src/mls_vectors.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/MLS/mlspp-main/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object lib/mls_vectors/CMakeFiles/mls_vectors.dir/src/mls_vectors.cpp.o"
	cd /home/MLS/mlspp-main/lib/mls_vectors && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/mls_vectors.dir/src/mls_vectors.cpp.o -c /home/MLS/mlspp-main/lib/mls_vectors/src/mls_vectors.cpp

lib/mls_vectors/CMakeFiles/mls_vectors.dir/src/mls_vectors.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/mls_vectors.dir/src/mls_vectors.cpp.i"
	cd /home/MLS/mlspp-main/lib/mls_vectors && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/MLS/mlspp-main/lib/mls_vectors/src/mls_vectors.cpp > CMakeFiles/mls_vectors.dir/src/mls_vectors.cpp.i

lib/mls_vectors/CMakeFiles/mls_vectors.dir/src/mls_vectors.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/mls_vectors.dir/src/mls_vectors.cpp.s"
	cd /home/MLS/mlspp-main/lib/mls_vectors && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/MLS/mlspp-main/lib/mls_vectors/src/mls_vectors.cpp -o CMakeFiles/mls_vectors.dir/src/mls_vectors.cpp.s

lib/mls_vectors/CMakeFiles/mls_vectors.dir/src/mls_vectors.cpp.o.requires:

.PHONY : lib/mls_vectors/CMakeFiles/mls_vectors.dir/src/mls_vectors.cpp.o.requires

lib/mls_vectors/CMakeFiles/mls_vectors.dir/src/mls_vectors.cpp.o.provides: lib/mls_vectors/CMakeFiles/mls_vectors.dir/src/mls_vectors.cpp.o.requires
	$(MAKE) -f lib/mls_vectors/CMakeFiles/mls_vectors.dir/build.make lib/mls_vectors/CMakeFiles/mls_vectors.dir/src/mls_vectors.cpp.o.provides.build
.PHONY : lib/mls_vectors/CMakeFiles/mls_vectors.dir/src/mls_vectors.cpp.o.provides

lib/mls_vectors/CMakeFiles/mls_vectors.dir/src/mls_vectors.cpp.o.provides.build: lib/mls_vectors/CMakeFiles/mls_vectors.dir/src/mls_vectors.cpp.o


# Object files for target mls_vectors
mls_vectors_OBJECTS = \
"CMakeFiles/mls_vectors.dir/src/mls_vectors.cpp.o"

# External object files for target mls_vectors
mls_vectors_EXTERNAL_OBJECTS =

lib/mls_vectors/libmls_vectors.a: lib/mls_vectors/CMakeFiles/mls_vectors.dir/src/mls_vectors.cpp.o
lib/mls_vectors/libmls_vectors.a: lib/mls_vectors/CMakeFiles/mls_vectors.dir/build.make
lib/mls_vectors/libmls_vectors.a: lib/mls_vectors/CMakeFiles/mls_vectors.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/MLS/mlspp-main/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libmls_vectors.a"
	cd /home/MLS/mlspp-main/lib/mls_vectors && $(CMAKE_COMMAND) -P CMakeFiles/mls_vectors.dir/cmake_clean_target.cmake
	cd /home/MLS/mlspp-main/lib/mls_vectors && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/mls_vectors.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
lib/mls_vectors/CMakeFiles/mls_vectors.dir/build: lib/mls_vectors/libmls_vectors.a

.PHONY : lib/mls_vectors/CMakeFiles/mls_vectors.dir/build

lib/mls_vectors/CMakeFiles/mls_vectors.dir/requires: lib/mls_vectors/CMakeFiles/mls_vectors.dir/src/mls_vectors.cpp.o.requires

.PHONY : lib/mls_vectors/CMakeFiles/mls_vectors.dir/requires

lib/mls_vectors/CMakeFiles/mls_vectors.dir/clean:
	cd /home/MLS/mlspp-main/lib/mls_vectors && $(CMAKE_COMMAND) -P CMakeFiles/mls_vectors.dir/cmake_clean.cmake
.PHONY : lib/mls_vectors/CMakeFiles/mls_vectors.dir/clean

lib/mls_vectors/CMakeFiles/mls_vectors.dir/depend:
	cd /home/MLS/mlspp-main && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/MLS/mlspp-main /home/MLS/mlspp-main/lib/mls_vectors /home/MLS/mlspp-main /home/MLS/mlspp-main/lib/mls_vectors /home/MLS/mlspp-main/lib/mls_vectors/CMakeFiles/mls_vectors.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : lib/mls_vectors/CMakeFiles/mls_vectors.dir/depend

