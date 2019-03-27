#ifndef TOPOLOGY_H
#define TOPOLOGY_H

#include<unordered_map>
#include<vector>

#include "header/entity.h"
#include "proto/topology.pb.h"

namespace topology {

class Topology {
 public:
  explicit Topology(const PhysicalTopology& physical_topology);
  
  // APIs for setting up logical topology.
  bool SetOcsConfiguration(OcsIndex ocs_index, const std::unordered_map<int, int>& configuration);

  // Querying functions for physical topology.
  int NumOcses() {
    return ocses_.size();
  }
  int NumBlocks() {
    return blocks_.size();
  }
  int NumBlockToOcsLinks(BlockIndex block_index, OcsIndex ocs_index) {
    return block_to_ocs_link_count_.at(block_index).at(ocs_index);
  }
  int NumOcsToBlockLinks(OcsIndex ocs_index, BlockIndex block_index) {
    return ocs_to_block_link_count_.at(block_index).at(ocs_index);
  }
  const Ocs* FindOcsByPort(Port port) {
    return &ocses_.at(port_to_ocs_index_[port]);
  }
  const Ocs* FindOcsByIndex(OcsIndex ocs_index) {
    return &ocses_.at(ocs_index);
  }
  const Block* FindBlockByPort(Port port) {
    return &blocks_.at(port_to_block_index_[port]);
  }
  const Block* FindBlockByIndex(BlockIndex block_index) {
    return &blocks_.at(block_index);
  }

  // Querying functions for logical topology.
  int NumBlockToOcsToBlockLinks(BlockIndex block1_index, OcsIndex ocs_index, BlockIndex block2_index);
  int NumBlockToBlockLinks(BlockIndex block1_index, BlockIndex block2_index);

 private:
  // Physical topology.
  std::vector<Block> blocks_;
  std::vector<Ocs> ocses_;
  BlockOcsLevelTopology block_to_ocs_link_count_;
  BlockOcsLevelTopology ocs_to_block_link_count_;

  // Query support.
  std::unordered_map<Port, BlockIndex> port_to_block_index_;
  std::unordered_map<Port, OcsIndex> port_to_ocs_index_;

  // Logical topology.
  std::vector<BlockLevelTopology> per_ocs_logical_topology_;
  BlockLevelTopology logical_topology_;
};

}  // namespace topology

#endif
