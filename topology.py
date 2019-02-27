from abc import ABCMeta, abstractmethod

class Topology:
  __metaclass = ABCMeta

  def __init__(self, radix_list):
    self.radix_list = radix_list
    num_blocks = len(radix_list)
    self.topology = [[0 for i in range(num_blocks)] for j in range(num_blocks)]

  def Print(self):
    print "Topology"
    for row in self.topology:
      rounded_row = [round(x,2) for x in row]
      print rounded_row
    print "remaining links"
    for i in range(len(self.radix_list)):
      print round(self.radix_list[i] - sum(self.topology[i]), 2)

  @abstractmethod
  def Compute(self):
    pass

class MeshTopology(Topology):

  def __init__(self, radix_list):
    Topology.__init__(self, radix_list)

  def Compute(self):
    radix_list = self.radix_list
    num_blocks = len(radix_list)
    sorted_indices = sorted(range(num_blocks), key=lambda k: radix_list[k])
    index_groups = []
    group = []
    for i in range(num_blocks):
      group.append(sorted_indices[i])
      if i == num_blocks - 1:
        index_groups.append(group)
      else:
        if radix_list[sorted_indices[i+1]] > radix_list[sorted_indices[i]]:
          index_groups.append(group)
          group = []
    allocated_radix = [0 for i in range(num_blocks)]
    for group in index_groups:
      total_radix = 0
      for idx in range(num_blocks):
	if allocated_radix[idx] < radix_list[idx]:
	  total_radix += radix_list[idx]
      current_radix = radix_list[group[0]] - allocated_radix[group[0]]
      for idx1 in group:
	for idx2 in range(num_blocks):
          if idx1 == idx2:
	    continue
	  if allocated_radix[idx2] >= radix_list[idx2]:
	    continue
	  allocation = float(current_radix * radix_list[idx2]) / (total_radix - radix_list[idx1])
	  self.topology[idx1][idx2] = allocation
	  self.topology[idx2][idx1] = allocation
          allocated_radix[idx2] += allocation
      for idx1 in group:
	allocated_radix[idx1] = radix_list[idx1]
    return self.topology


class TrafficAwareTopology(Topology):

  def __init__(self, radix_list, traffic_list):
    Topology.__init__(self, radix_list)

  def Compute(self):


mesh = MeshTopology([256, 256, 1024, 512, 512, 256])
topology = mesh.Compute()
mesh.Print()

