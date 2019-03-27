#ifndef ENTITY_H
#define ENTITY_H

#include<unordered_map>
#include<vector>

namespace topology {

using Port = int;
using OcsIndex = unsigned int;
using BlockIndex = unsigned int;
using BlockLevelTopology = std::unordered_map<BlockIndex, std::unordered_map<BlockIndex, int>>;
using BlockOcsLevelTopology = std::unordered_map<BlockIndex, std::unordered_map<OcsIndex, int>>;

struct Ocs {
  OcsIndex index;
  std::vector<Port> ingress_ports;
  std::vector<Port> egress_ports;
  // A map from ingress ports to egress ports.
  std::unordered_map<Port, Port> interconnection;
};

struct Block {
  BlockIndex index;
  int port_speed;  // In Gbps
  std::vector<Port> ingress_ports;
  std::vector<Port> egress_ports;
};

}  // namespace topology

#endif
