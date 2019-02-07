# the compiler: gcc for C program, define as g++ for C++
CC = g++

INC = -I /Library/gurobi810/mac64/include/

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -g -Wall

# the build target executable:
TARGET = mip

all: $(TARGET)

$(TARGET): $(TARGET).cc
	$(CC) $(INC) $(CFLAGS) -v -o $(TARGET) $(TARGET).cc

clean:
	$(RM) $(TARGET)

run:
	./$(TARGET)

