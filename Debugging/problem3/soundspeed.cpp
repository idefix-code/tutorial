#include "idefix.hpp"
#include "soundspeed.hpp"
SoundSpeed::SoundSpeed(Input &input, DataBlock &data) {
  idfx::pushRegion("SoundSpeed::SoundSpeed");
  // Get a copy of the radius and the disc thickness for later use
  this->Rcoord = data.x[IDIR];
  h0 = input.Get<real>("Setup","h0",0);
  np_tot = data.np_tot;
  idfx::popRegion();
}

void SoundSpeed::Compute(IdefixArray3D<real> &cs) {
  idfx::pushRegion("SoundSpeed::Compute");
  idefix_for("MySoundSpeed",0,np_tot[KDIR],0,np_tot[JDIR],0,np_tot[IDIR],
              KOKKOS_LAMBDA (int k, int j, int i) {
                real R = Rcoord(i);
                cs(k,j,i) = h0/sqrt(R);
              });
  idfx::popRegion();
}
