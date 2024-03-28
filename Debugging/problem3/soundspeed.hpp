#include <vector>
#include "idefix.hpp"
#include "input.hpp"
#include "dataBlock.hpp"

class SoundSpeed {
 public:
  SoundSpeed(Input &, DataBlock &);  // Constructor
  void Compute(IdefixArray3D<real> &cs);            // Compute the sound speed
 private:
  real h0;                   // disc scale height
  IdefixArray1D<real> Rcoord;// Radius coordinate
  std::vector<int> np_tot;        // number of points in each direction
};
