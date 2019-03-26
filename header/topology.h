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
  
  // Querying functions for physical topology.
  int NumOcses();
  int NumBlocks();
  int NumBlockToOcsLinks(BlockIndex block_index, OcsIndex ocs_index);
  int NumOcsToBlockLinks(OcsIndex ocs_index, BlockIndex block_index);
  const Ocs* FindOcsByPort(Port port);
  const Ocs* FindOcsByIndex(OcsIndex ocs_index);
  const Block* FindBlockByPort(Port port);
  const Block* FindBlockByIndex(BlockIndex block_index);

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
  std::unordered_map<Port, BlockIndex> port_to_block_;
  std::unordered_map<Port, OcsIndex> port_to_ocs_;

  // Logical topology.
  std::vector<BlockLevelTopology> per_ocs_logical_topology_;
  BlockLevelTopology logical_topology_;
};

}  // namespace topology

#endif
