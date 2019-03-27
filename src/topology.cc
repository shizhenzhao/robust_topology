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
  for (unsigned int i = 0; i < physical_topology.num_ocses(); ++i) {
    ocses_.push_back(Ocs({.index = i}));
  }

  // Set up the physical topology between blocks and ocses.
  std::unordered_map<BlockIndex, int> block_to_next_egress_port;
  std::unordered_map<BlockIndex, int> block_to_next_ingress_port;
  for (const PhysicalLinkProto& physical_link_proto : physical_topology.links()) {
    const BlockIndex block_index = physical_link_proto.block_index();
    const Block& block = blocks_[block_index];
    const OcsIndex ocs_index = physical_link_proto.ocs_index();
    Ocs& ocs = ocses_[ocs_index];
    // Set up the egress ports for ocses.
    const int num_ocs_block_links = physical_link_proto.num_ocs_block_links();
    auto next_ingress_port_it = block_to_next_ingress_port.find(block_index);
    int next_ingress_port_idx = (next_ingress_port_it == block_to_next_ingress_port.end()) ? 0 : next_ingress_port_it->second;
    for (int i = 0; i < num_ocs_block_links; ++i) {
      // Crash if next_ingress_port_idx + num_ocs_block_links >= block.ingress_ports.size().
      ocs.egress_ports.push_back(block.ingress_ports[next_ingress_port_idx + i]);
    }
    block_to_next_ingress_port[block_index] = next_ingress_port_idx + num_ocs_block_links;
    ocs_to_block_link_count_[block_index][ocs_index] = num_ocs_block_links;
    // Set up the ingress ports for ocses.
    const int num_block_ocs_links = physical_link_proto.num_block_ocs_links();
    auto next_egress_port_it = block_to_next_egress_port.find(block_index);
    int next_egress_port_idx = (next_egress_port_it == block_to_next_egress_port.end()) ? 0 : next_egress_port_it->second;
    for (int i = 0; i < num_block_ocs_links; ++i) {
      // Crash if next_egress_port_idx + num_block_ocs_links >= block.egress_ports.size();
      ocs.ingress_ports.push_back(block.egress_ports[next_egress_port_idx + i]);
    }
    block_to_next_egress_port[block_index] = next_egress_port_idx + num_block_ocs_links;
    block_to_ocs_link_count_[block_index][ocs_index] = num_block_ocs_links;
  }

  // Set up port to ocs/block query.
  for (const Block& block : blocks_) {
    for (Port port : block.ingress_ports) {
      port_to_block_index_[port] = block.index;
    }
    for (Port port : block.egress_ports) {
      port_to_block_index_[port] = block.index;
    }
  }
  for (const Ocs& ocs : ocses_) {
    for (Port port : ocs.ingress_ports) {
      port_to_ocs_index_[port] = ocs.index;
    }
    for (Port port : ocs.egress_ports) {
      port_to_ocs_index_[port] = ocs.index;
    }
  }

  // Initialize logical topology.
  per_ocs_logical_topology_.resize(ocses_.size());
}

bool Topology::SetOcsConfiguration(OcsIndex ocs_index, const std::unordered_map<int, int>& configuration) {
  if (ocs_index >= ocses_.size()) {
    return false;
  }
  const std::vector<Port>& ingress_ports = ocses_[ocs_index].ingress_ports;
  std::unordered_set<Port> ingress_port_set(ingress_ports.begin(), ingress_ports.end());
  const std::vector<Port>& egress_ports = ocses_[ocs_index].egress_ports;
  std::unordered_set<Port> egress_port_set(egress_ports.begin(), egress_ports.end());
  BlockLevelTopology topology_in_ocs;
  for (const auto& ingress_and_egress : configuration) {
    // Check if the ingress/egress ports belong to this ocs.
    Port ingress_port = ingress_and_egress.first;
    if (ingress_port_set.find(ingress_port) == ingress_port_set.end()) {
      return false;
    }
    Port egress_port = ingress_and_egress.second;
    if (egress_port_set.find(egress_port) == egress_port_set.end()) {
      return false;
    }
    // Record the block level link count.
    topology_in_ocs[port_to_block_index_.at(ingress_port)][port_to_block_index_.at(egress_port)] += 1;
  }
  // Update logical topology.
  for (const auto& src_and_dst_map : per_ocs_logical_topology_[ocs_index]) {
    BlockIndex src_index = src_and_dst_map.first;
    for (const auto& dst_and_value : src_and_dst_map.second) {
      BlockIndex dst_index = dst_and_value.first;
      logical_topology_[src_index][dst_index] -= dst_and_value.second;
    }
  }
  for (const auto& src_and_dst_map : topology_in_ocs) {
    BlockIndex src_index = src_and_dst_map.first;
    for (const auto& dst_and_value : src_and_dst_map.second) {
      BlockIndex dst_index = dst_and_value.first;
      logical_topology_[src_index][dst_index] += dst_and_value.second;
    }
  }
  per_ocs_logical_topology_[ocs_index] = topology_in_ocs;
  ocses_[ocs_index].interconnection = configuration;
  return true;
}

namespace {

int MapLookUp(const BlockLevelTopology& block_level_topology, BlockIndex block1_index, BlockIndex block2_index) {
  auto first_it = block_level_topology.find(block1_index);
  if (first_it == block_level_topology.end()) {
    return 0;
  }
  auto second_it = first_it->second.find(block2_index);
  if (second_it == first_it->second.end()) {
    return 0;
  }
  return second_it->second;
}

}

int Topology::NumBlockToOcsToBlockLinks(BlockIndex block1_index, OcsIndex ocs_index, BlockIndex block2_index) {
  if (ocs_index >= per_ocs_logical_topology_.size()) {
    return 0;
  }
  return MapLookUp(per_ocs_logical_topology_[ocs_index], block1_index, block2_index);
}

int Topology::NumBlockToBlockLinks(BlockIndex block1_index, BlockIndex block2_index) {
  return MapLookUp(logical_topology_, block1_index, block2_index);
}

}  // namespace topology

