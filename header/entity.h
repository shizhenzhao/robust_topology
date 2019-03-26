#ifndef ENTITY_H
#define ENTITY_H

#include<unordered_map>
#include<vector>

namespace topology {

using Port = int;
using OcsIndex = int;
using BlockIndex = int;

struct Ocs {
  OcsIndex index;
  std::vector<Port> ingress_ports;
  std::vector<Port> egress_ports;
  // A map from ingress ports to egress ports.
  std::unordered_map<int, int> interconnection;
};

struct Block {
  BlockIndex index;
  int speed;  // In Gbps
  std::vector<Port> ingress_ports;
  std::vector<Port> egress_ports;
};

}  // namespace topology

#endif
