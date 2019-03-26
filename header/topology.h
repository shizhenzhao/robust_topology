#ifndef TOPOLOGY_H
#define TOPOLOGY_H

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
};

}  // namespace topology

#endif
