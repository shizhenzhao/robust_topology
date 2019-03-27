# the compiler: gcc for C program, define as g++ for C++
CC_GUROBI = g++-4.8
CC = g++

# gurobi flags
INC = -I/opt/gurobi810/linux64/include -L/opt/gurobi810/linux64/lib -lgurobi_c++ -lgurobi81

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -g -Wall -I. -std=c++11

# protobuf flags
LIBS = -lprotobuf

# the build target executable:
TARGET = topology_main

# Directory
PROTO_DIR = proto
HEADER_DIR = header
SRC_DIR = src
OBJ_DIR = obj

all: $(TARGET) $(OBJ_DIR)/topology_solver.o

#$(TARGET):
#	$(CC_GUROBI) $(CFLAGS) $(TARGET).cc -o $(TARGET) $(INC)

topology_main: $(OBJ_DIR)/topology.o $(OBJ_DIR)/topology_main.o $(OBJ_DIR)/topology.pb.o
	$(CC) $(CFLAGS) $(OBJ_DIR)/topology_main.o $(OBJ_DIR)/topology.o $(OBJ_DIR)/topology.pb.o $(LIBS) -o topology_main

$(OBJ_DIR)/topology_main.o: topology_main.cc $(HEADER_DIR)/topology.h $(HEADER_DIR)/entity.h $(PROTO_DIR)/topology.pb.h
	$(CC) $(CFLAGS) -o $(OBJ_DIR)/topology_main.o -c topology_main.cc

$(OBJ_DIR)/topology_solver.o: $(SRC_DIR)/topology_solver.cc $(HEADER_DIR)/topology.h $(HEADER_DIR)/entity.h
	$(CC) $(CFLAGS) -o $(OBJ_DIR)/topology_solver.o -c $(SRC_DIR)/topology_solver.cc

$(OBJ_DIR)/topology.o: $(SRC_DIR)/topology.cc $(HEADER_DIR)/topology.h $(HEADER_DIR)/entity.h $(PROTO_DIR)/topology.pb.h
	$(CC) $(CFLAGS) -o $(OBJ_DIR)/topology.o -c $(SRC_DIR)/topology.cc

$(OBJ_DIR)/topology.pb.o: $(PROTO_DIR)/topology.pb.cc $(PROTO_DIR)/topology.pb.h
	$(CC) $(CFLAGS) -o $(OBJ_DIR)/topology.pb.o -c $(PROTO_DIR)/topology.pb.cc

$(PROTO_DIR)/topology.pb.h $(PROTO_DIR)/topology.pb.cc: $(PROTO_DIR)/topology.proto
	protoc -I=$(PROTO_DIR) --cpp_out=$(PROTO_DIR) $(PROTO_DIR)/topology.proto

clean:
	$(RM) $(TARGET) $(PROTO_DIR)/*.pb* $(OBJ_DIR)/* $(HEADER_DIR)/*.gch topology_main

run:
	./$(TARGET)

