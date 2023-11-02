# Makefile to (1) compile and (2) clean
#  - Rule for compilation should create solution.exe
#     - Should only recompile code when any source code changed
#  - Rule for clean should remove solution.exe
#     - Should not show error if the file does not exist
#  - Use GCC compiler toolchain
#  - Specify the following options:
#     - Disable optimization
#     - Disable generation of debug information
#     - Enable all general warnings
#     - Enable conversion warnings

  CC = g++
  CFLAGS  = -g -Wall
  CXXFLAGS = -std=c++17
 
  # The build target 
  TARGET = solution
 
  all: $(TARGET)
 
  $(TARGET): $(TARGET).cpp
              $(CC) $(CFLAGS) $(CXXFLAGS) -o $(TARGET) $(TARGET).cpp
 
  clean:
              $(RM) $(TARGET)

