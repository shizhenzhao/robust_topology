#include "header/topology.h"

namespace topology {

Topology::Topology(const PhysicalTopology& physical_topology) {
  // Set up blocks.
  Port next_port_index = 0;
  BlockIndex next_block_index = 0;
  for (const BlockProto& block_proto : physical_topology.blocks()) {
    Block block;
    block.index = next_block_index++;
    block.port_speed = block_proto.port_speed();
    for (int i = 0; i < block_proto.egress_radix(); ++i) {
      block.egress_ports.emplace_back(next_port_index++);
    }
    for (int i = 0; i < block_proto.ingress_radix(); ++i) {
      block.ingress_ports.emplace_back(next_port_index++);
    }
    blocks_.push_back(block);
  }

  // Set up ocses.
  for (int i = 0; i < physical_topology.num_ocses(); ++i) {
    ocses_.push_back(Ocs({.index = i}));
  }

  // Set up the physical topology between blocks and ocses.

}

}  // namespace topology

