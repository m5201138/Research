# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.8

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
CMAKE_COMMAND = /opt/local/bin/cmake

# The command to remove a file.
RM = /opt/local/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/m5201138/Research

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/m5201138/Research

# Include any dependencies generated for this target.
include CMakeFiles/TwString.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/TwString.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/TwString.dir/flags.make

# Object files for target TwString
TwString_OBJECTS =

# External object files for target TwString
TwString_EXTERNAL_OBJECTS =

bin/TwString: CMakeFiles/TwString.dir/build.make
bin/TwString: /opt/local/lib/libmpfr.dylib
bin/TwString: /opt/local/lib/libgmp.dylib
bin/TwString: /opt/local/lib/libCGAL_Core.12.0.0.dylib
bin/TwString: /opt/local/lib/libCGAL.12.0.0.dylib
bin/TwString: /opt/local/lib/libboost_thread-mt.dylib
bin/TwString: /opt/local/lib/libboost_system-mt.dylib
bin/TwString: /opt/local/lib/libboost_chrono-mt.dylib
bin/TwString: /opt/local/lib/libboost_date_time-mt.dylib
bin/TwString: /opt/local/lib/libboost_atomic-mt.dylib
bin/TwString: lib/libAntTweakBar.a
bin/TwString: /opt/local/lib/libmpfr.dylib
bin/TwString: /opt/local/lib/libgmp.dylib
bin/TwString: /opt/local/lib/libCGAL_Core.12.0.0.dylib
bin/TwString: /opt/local/lib/libCGAL.12.0.0.dylib
bin/TwString: /opt/local/lib/libboost_thread-mt.dylib
bin/TwString: /opt/local/lib/libboost_system-mt.dylib
bin/TwString: /opt/local/lib/libboost_chrono-mt.dylib
bin/TwString: /opt/local/lib/libboost_date_time-mt.dylib
bin/TwString: /opt/local/lib/libboost_atomic-mt.dylib
bin/TwString: CMakeFiles/TwString.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/m5201138/Research/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Linking CXX executable bin/TwString"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/TwString.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/TwString.dir/build: bin/TwString

.PHONY : CMakeFiles/TwString.dir/build

CMakeFiles/TwString.dir/requires:

.PHONY : CMakeFiles/TwString.dir/requires

CMakeFiles/TwString.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/TwString.dir/cmake_clean.cmake
.PHONY : CMakeFiles/TwString.dir/clean

CMakeFiles/TwString.dir/depend:
	cd /Users/m5201138/Research && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/m5201138/Research /Users/m5201138/Research /Users/m5201138/Research /Users/m5201138/Research /Users/m5201138/Research/CMakeFiles/TwString.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/TwString.dir/depend

