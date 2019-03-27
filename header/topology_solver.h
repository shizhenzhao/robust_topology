#ifndef TOPOLOGY_SOLVER_H
#define TOPOLOGY_SOLVER_H

#include<unordered_map>
#include<vector>

#include "header/entity.h"
#include "header/topology.h"

namespace topology {

class TopologySolver {
 public:
  virtual ~TopologySolver() {}
  
  virtual void SetUpPhysicalTopologyQuery(const Topology* topology) {
    topology_ = topology;
  }

  // Derived classes should implement their own method.
  virtual void ComputeTopology() = 0; 

  virtual const std::unordered_map<Port, Port>& GetConfigurationForOcs(OcsIndex ocs_index) {
    return per_ocs_configuration_.at(ocs_index);
  }

  virtual FractionalMatrix GetTopologyWithCapacity() {
    return capacity_topology_;
  }

 protected:
  // Physical topology query.
  const Topology* topology_;
  // Final topology holder.
  std::vector<std::unordered_map<Port, Port>> per_ocs_configuration_;
  // Block level capacity information.
  FractionalMatrix capacity_topology_;
};

// ILP solution for uniform topology (Baseline).
class UniformTopologySolver : public TopologySolver {
 public:
  UniformTopologySolver() {}

  void ComputeTopology() override; 
};

// ILP solution for traffic aware topology (Best case).
class TrafficAwareIlpSolver : public TopologySolver {
 public:
  TrafficAwareIlpSolver(const FractionalMatrix& traffic) : traffic_(traffic) {}

  void ComputeTopology() override;

 private:
  FractionalMatrix traffic_;
};

// Solution in Helios (heuristic).
class TrafficAwareHeliosSolver : public TopologySolver {
 public:
  TrafficAwareHeliosSolver(const FractionalMatrix& traffic) : traffic_(traffic) {}

  void ComputeTopology() override;

 private:
  FractionalMatrix traffic_;
};

// Solution in LFToE for single traffic.
class LftoeSingleTrafficSolver : public TopologySolver {
 public:
  LftoeSingleTrafficSolver(const FractionalMatrix& traffic) : traffic_(traffic) {}

  void ComputeTopology() override;

 private:
  FractionalMatrix traffic_;
};

// Solution in LFToE for multiple traffic.
class LftoeMultiTrafficSolver : public TopologySolver {
 public:
  LftoeMultiTrafficSolver(const std::vector<FractionalMatrix>& traffic) : traffic_(traffic) {}

  void ComputeTopology() override;

 private:
  std::vector<FractionalMatrix> traffic_;
};

}  // namespace topology

#endif
