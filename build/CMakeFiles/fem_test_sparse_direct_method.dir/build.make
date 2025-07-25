# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 4.0

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
CMAKE_COMMAND = /opt/homebrew/bin/cmake

# The command to remove a file.
RM = /opt/homebrew/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/yincheangng/worksapce/Github/SparseDirectMethod

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/yincheangng/worksapce/Github/SparseDirectMethod/build

# Include any dependencies generated for this target.
include CMakeFiles/fem_test_sparse_direct_method.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/fem_test_sparse_direct_method.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/fem_test_sparse_direct_method.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/fem_test_sparse_direct_method.dir/flags.make

CMakeFiles/fem_test_sparse_direct_method.dir/codegen:
.PHONY : CMakeFiles/fem_test_sparse_direct_method.dir/codegen

CMakeFiles/fem_test_sparse_direct_method.dir/fem_test.cpp.o: CMakeFiles/fem_test_sparse_direct_method.dir/flags.make
CMakeFiles/fem_test_sparse_direct_method.dir/fem_test.cpp.o: /Users/yincheangng/worksapce/Github/SparseDirectMethod/fem_test.cpp
CMakeFiles/fem_test_sparse_direct_method.dir/fem_test.cpp.o: CMakeFiles/fem_test_sparse_direct_method.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/yincheangng/worksapce/Github/SparseDirectMethod/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/fem_test_sparse_direct_method.dir/fem_test.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/fem_test_sparse_direct_method.dir/fem_test.cpp.o -MF CMakeFiles/fem_test_sparse_direct_method.dir/fem_test.cpp.o.d -o CMakeFiles/fem_test_sparse_direct_method.dir/fem_test.cpp.o -c /Users/yincheangng/worksapce/Github/SparseDirectMethod/fem_test.cpp

CMakeFiles/fem_test_sparse_direct_method.dir/fem_test.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/fem_test_sparse_direct_method.dir/fem_test.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/yincheangng/worksapce/Github/SparseDirectMethod/fem_test.cpp > CMakeFiles/fem_test_sparse_direct_method.dir/fem_test.cpp.i

CMakeFiles/fem_test_sparse_direct_method.dir/fem_test.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/fem_test_sparse_direct_method.dir/fem_test.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/yincheangng/worksapce/Github/SparseDirectMethod/fem_test.cpp -o CMakeFiles/fem_test_sparse_direct_method.dir/fem_test.cpp.s

# Object files for target fem_test_sparse_direct_method
fem_test_sparse_direct_method_OBJECTS = \
"CMakeFiles/fem_test_sparse_direct_method.dir/fem_test.cpp.o"

# External object files for target fem_test_sparse_direct_method
fem_test_sparse_direct_method_EXTERNAL_OBJECTS =

fem_test_sparse_direct_method: CMakeFiles/fem_test_sparse_direct_method.dir/fem_test.cpp.o
fem_test_sparse_direct_method: CMakeFiles/fem_test_sparse_direct_method.dir/build.make
fem_test_sparse_direct_method: libSparseDirectMethod.a
fem_test_sparse_direct_method: CMakeFiles/fem_test_sparse_direct_method.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/Users/yincheangng/worksapce/Github/SparseDirectMethod/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable fem_test_sparse_direct_method"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/fem_test_sparse_direct_method.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/fem_test_sparse_direct_method.dir/build: fem_test_sparse_direct_method
.PHONY : CMakeFiles/fem_test_sparse_direct_method.dir/build

CMakeFiles/fem_test_sparse_direct_method.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/fem_test_sparse_direct_method.dir/cmake_clean.cmake
.PHONY : CMakeFiles/fem_test_sparse_direct_method.dir/clean

CMakeFiles/fem_test_sparse_direct_method.dir/depend:
	cd /Users/yincheangng/worksapce/Github/SparseDirectMethod/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/yincheangng/worksapce/Github/SparseDirectMethod /Users/yincheangng/worksapce/Github/SparseDirectMethod /Users/yincheangng/worksapce/Github/SparseDirectMethod/build /Users/yincheangng/worksapce/Github/SparseDirectMethod/build /Users/yincheangng/worksapce/Github/SparseDirectMethod/build/CMakeFiles/fem_test_sparse_direct_method.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/fem_test_sparse_direct_method.dir/depend

