#include <iostream>
#include <fstream>
#include <string>

#include "header/entity.h"
#include "header/topology.h"
#include "proto/topology.pb.h"

int main(int argc, char *argv[]) {
  // Verify that the version of the library that we linked against is
  // compatible with the version of the headers we compiled against.
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  
  topology::PhysicalTopology physical_topology;
  const int num_blocks = 8;
  const int num_ocses = 2;
  const int port_count = 8;
  for (int i = 0; i < num_blocks; ++i) {
    topology::BlockProto* block_proto = physical_topology.add_blocks();
    block_proto->set_port_speed(100);
    block_proto->set_egress_radix(port_count / 2);
    block_proto->set_ingress_radix(port_count / 2);
  }
  physical_topology.set_num_ocses(num_ocses);
  for (int i = 0; i < num_blocks; ++i) {
    for (int j = 0; j < num_ocses; ++j) {
      topology::PhysicalLinkProto* physical_link = physical_topology.add_links();
      physical_link->set_ocs_index(j);
      physical_link->set_block_index(i);
      physical_link->set_num_ocs_block_links(port_count / 2 / num_ocses);
      physical_link->set_num_block_ocs_links(port_count / 2 / num_ocses);
    }
  }
  
  topology::Topology topology(physical_topology);
  assert(topology.NumBlocks() == 8);
  assert(topology.NumOcses() == 2);
  assert(topology.NumOcsToBlockLinks(1,4) == 2);
  assert(topology.NumBlockToOcsLinks(3,0) == 2);
  assert(topology.FindOcsByPort(4)->index == 0);

  std::fstream output("output", std::ios::out | std::ios::trunc | std::ios::binary);
  if (!physical_topology.SerializeToOstream(&output)) {
    std::cerr << "Failed to write topology." << std::endl;
    return -1;
  }
  output.close();

  topology::PhysicalTopology physical_topology2;
  // Read the existing address book.
  std::fstream input("output", std::ios::in | std::ios::binary);
  physical_topology2.ParseFromIstream(&input);
  std::cout << physical_topology2.blocks().size() << std::endl;  
  physical_topology2.PrintDebugString();

  // Optional:  Delete all global objects allocated by libprotobuf.
  google::protobuf::ShutdownProtobufLibrary();

  return 0;
}
