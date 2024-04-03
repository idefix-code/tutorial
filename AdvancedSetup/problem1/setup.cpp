#include <algorithm>
#include "idefix.hpp"
#include "setup.hpp"

namespace SetupVariables {
real h0;   // The disc aspect ratio
}


void MySoundSpeed(DataBlock &data, const real t, IdefixArray3D<real> &cs) {
  // Always make local "copies" of the arrays you intent to use!
  IdefixArray1D<real> x1 = data.x[IDIR]; // NB: this is a shallow copy, it doesn't copy the data contained in the array, juste the "pointer"
  real h0 = SetupVariables::h0;
  // Loop on the full domain
  idefix_for("MySoundSpeed",0,data.np_tot[KDIR],0,data.np_tot[JDIR],0,data.np_tot[IDIR],
    KOKKOS_LAMBDA (int k, int j, int i) {
      real R = x1(i);
      cs(k,j,i) = h0/sqrt(R);
    });
}

// User-defined boundaries
void UserdefBoundary(Hydro *hydro, int dir, BoundarySide side, real t) {
  idfx::pushRegion("UserDefinedBoundary");
  if(dir==IDIR) {
    // "always do shallow copies"
    IdefixArray4D<real> Vc = hydro->Vc;
    IdefixArray1D<real> R = data->x[IDIR];
    // the index in the active domain we use as reference
    int iref;
    if(side == left) {
      iref = data->beg[IDIR];
    } else if (side==right) {
      iref = data->end[IDIR]-1;
    }
    hydro->boundary->BoundaryFor("UserDefBoundary_X1Beg", dir, side,
      KOKKOS_LAMBDA (int k, int j, int i) {
        real R=x1(i);
        real Vk = 1.0/sqrt(R);

        Vc(RHO,k,j,i) = Vc(RHO,k,j,iref);
        Vc(VX1,k,j,i) = Vc(VX1,k,j,iref);
        Vc(VX2,k,j,i) = Vk;
    });
  }
  idfx::popRegion();
}

// Default constructor
// Initialisation routine. Can be used to allocate
// Arrays or variables which are used later on
Setup::Setup(Input &input, Grid &grid, DataBlock &data, Output &output)
{
  // Set the function for userdefboundary
  data.hydro->EnrollUserDefBoundary(&UserdefBoundary);
  data.hydro->EnrollIsoSoundSpeed(&MySoundSpeed);
  SetupVariables::h0 = input.Get<real>("Setup","h0",0);

}

// This routine initialize the flow
// Note that data is on the device.
// One can therefore define locally
// a datahost and sync it, if needed
void Setup::InitFlow(DataBlock &data) {
    // Create a host copy
    DataBlockHost d(data);
    real h0=h0Glob;

    for(int k = 0; k < d.np_tot[KDIR] ; k++) {
        for(int j = 0; j < d.np_tot[JDIR] ; j++) {
            for(int i = 0; i < d.np_tot[IDIR] ; i++) {
                real R=d.x[IDIR](i);
                real Vk=1.0/sqrt(R);

                real cs2=(h0*Vk)*(h0*Vk);

                d.Vc(RHO,k,j,i) = 100/R; // Surface density profile
                d.Vc(VX1,k,j,i) = 0.0;
                d.Vc(VX2,k,j,i) = Vk*sqrt( 1 - 3.0*h0*h0);
            }
        }
    }

    // Send it all, if needed
    d.SyncToDevice();
}
