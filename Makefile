# the compiler: gcc for C program, define as g++ for C++
CC_GUROBI = g++-4.8
CC = g++

# gurobi flags
INC = -I /opt/gurobi810/linux64/include -L /opt/gurobi810/linux64/lib -l gurobi_c++ -l gurobi81

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -g -Wall -I .

# the build target executable:
TARGET = test

# Directory
PROTO_DIR = proto
HEADER_DIR = header
SRC_DIR = src
OBJ_DIR = obj

# Objects
OBJECTS = topology.o 

all: $(TARGET)

$(TARGET): topology.o
	$(CC_GUROBI) $(CFLAGS) $(TARGET).cc -o $(TARGET) $(INC)

topology.o: topology.pb.*
	$(CC) $(CFLAGS) -o $(OBJ_DIR)/topology.o -c $(SRC_DIR)/topology.cc

topology.pb.*:
	protoc -I=$(PROTO_DIR) --cpp_out=$(PROTO_DIR) $(PROTO_DIR)/topology.proto

clean:
	$(RM) $(TARGET) $(PROTO_DIR)/*.pb* $(OBJ_DIR)/* $(HEADER_DIR)/*.gch

run:
	./$(TARGET)

