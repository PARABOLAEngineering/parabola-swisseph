# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.25

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
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
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/reed/reeddev/Parabolagpu

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/reed/reeddev/Parabolagpu

# Include any dependencies generated for this target.
include CMakeFiles/parabola_wrapper.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/parabola_wrapper.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/parabola_wrapper.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/parabola_wrapper.dir/flags.make

CMakeFiles/parabola_wrapper.dir/parabola_wrapper.cpp.o: CMakeFiles/parabola_wrapper.dir/flags.make
CMakeFiles/parabola_wrapper.dir/parabola_wrapper.cpp.o: parabola_wrapper.cpp
CMakeFiles/parabola_wrapper.dir/parabola_wrapper.cpp.o: CMakeFiles/parabola_wrapper.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/reed/reeddev/Parabolagpu/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/parabola_wrapper.dir/parabola_wrapper.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/parabola_wrapper.dir/parabola_wrapper.cpp.o -MF CMakeFiles/parabola_wrapper.dir/parabola_wrapper.cpp.o.d -o CMakeFiles/parabola_wrapper.dir/parabola_wrapper.cpp.o -c /home/reed/reeddev/Parabolagpu/parabola_wrapper.cpp

CMakeFiles/parabola_wrapper.dir/parabola_wrapper.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/parabola_wrapper.dir/parabola_wrapper.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/reed/reeddev/Parabolagpu/parabola_wrapper.cpp > CMakeFiles/parabola_wrapper.dir/parabola_wrapper.cpp.i

CMakeFiles/parabola_wrapper.dir/parabola_wrapper.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/parabola_wrapper.dir/parabola_wrapper.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/reed/reeddev/Parabolagpu/parabola_wrapper.cpp -o CMakeFiles/parabola_wrapper.dir/parabola_wrapper.cpp.s

CMakeFiles/parabola_wrapper.dir/swevid_loader.cpp.o: CMakeFiles/parabola_wrapper.dir/flags.make
CMakeFiles/parabola_wrapper.dir/swevid_loader.cpp.o: swevid_loader.cpp
CMakeFiles/parabola_wrapper.dir/swevid_loader.cpp.o: CMakeFiles/parabola_wrapper.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/reed/reeddev/Parabolagpu/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/parabola_wrapper.dir/swevid_loader.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/parabola_wrapper.dir/swevid_loader.cpp.o -MF CMakeFiles/parabola_wrapper.dir/swevid_loader.cpp.o.d -o CMakeFiles/parabola_wrapper.dir/swevid_loader.cpp.o -c /home/reed/reeddev/Parabolagpu/swevid_loader.cpp

CMakeFiles/parabola_wrapper.dir/swevid_loader.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/parabola_wrapper.dir/swevid_loader.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/reed/reeddev/Parabolagpu/swevid_loader.cpp > CMakeFiles/parabola_wrapper.dir/swevid_loader.cpp.i

CMakeFiles/parabola_wrapper.dir/swevid_loader.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/parabola_wrapper.dir/swevid_loader.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/reed/reeddev/Parabolagpu/swevid_loader.cpp -o CMakeFiles/parabola_wrapper.dir/swevid_loader.cpp.s

# Object files for target parabola_wrapper
parabola_wrapper_OBJECTS = \
"CMakeFiles/parabola_wrapper.dir/parabola_wrapper.cpp.o" \
"CMakeFiles/parabola_wrapper.dir/swevid_loader.cpp.o"

# External object files for target parabola_wrapper
parabola_wrapper_EXTERNAL_OBJECTS =

libparabola_wrapper.a: CMakeFiles/parabola_wrapper.dir/parabola_wrapper.cpp.o
libparabola_wrapper.a: CMakeFiles/parabola_wrapper.dir/swevid_loader.cpp.o
libparabola_wrapper.a: CMakeFiles/parabola_wrapper.dir/build.make
libparabola_wrapper.a: CMakeFiles/parabola_wrapper.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/reed/reeddev/Parabolagpu/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX static library libparabola_wrapper.a"
	$(CMAKE_COMMAND) -P CMakeFiles/parabola_wrapper.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/parabola_wrapper.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/parabola_wrapper.dir/build: libparabola_wrapper.a
.PHONY : CMakeFiles/parabola_wrapper.dir/build

CMakeFiles/parabola_wrapper.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/parabola_wrapper.dir/cmake_clean.cmake
.PHONY : CMakeFiles/parabola_wrapper.dir/clean

CMakeFiles/parabola_wrapper.dir/depend:
	cd /home/reed/reeddev/Parabolagpu && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/reed/reeddev/Parabolagpu /home/reed/reeddev/Parabolagpu /home/reed/reeddev/Parabolagpu /home/reed/reeddev/Parabolagpu /home/reed/reeddev/Parabolagpu/CMakeFiles/parabola_wrapper.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/parabola_wrapper.dir/depend

