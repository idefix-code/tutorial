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
  // Here should come an idefix_for loop on the whole domain with indices (i,j,k).
    {
      real R = x1(i);
      cs(k,j,i) = // ## TBF ##
    }
}

// User-defined boundaries
void UserdefBoundary(Fluid<DefaultPhysics> *hydro, int dir, BoundarySide side, real t) {
  idfx::pushRegion("UserDefinedBoundary");
  if(dir==IDIR) {
    // "always do shallow copies"
    IdefixArray4D<real> Vc = hydro->Vc;
    IdefixArray1D<real> x1 = hydro->data->x[IDIR];
    // the index in the active domain we use as reference
    int iref;
    if(side == left) {
      iref = hydro->data->beg[IDIR];
    } else if (side==right) {
      iref = hydro->data->end[IDIR]-1;
    }
    // This makes an idefix_loop on the boundary elements (=ghost zones)
    hydro->boundary->BoundaryFor("UserDefBoundary_X1", dir, side,
      KOKKOS_LAMBDA (int k, int j, int i) {

        // We want tho and Vr (VX1) to be equal to their value in the first active zone (index iref)
        // Vphi should be equal to the Keplerian velocity

        //Vc(RHO,k,j,i) = ## TBF ##
        //Vc(VX1,k,j,i) = ## TBF ##
        //Vc(VX2,k,j,i) = ## TBF ##
    });
  }
  idfx::popRegion();
}

/*
// -- Dust --
// User-defined boundaries
void UserdefBoundaryDust(Fluid<DustPhysics> *hydro, int dir, BoundarySide side, real t) {
  idfx::pushRegion("UserDefinedBoundary");
  if(dir==IDIR) {
    // "always do shallow copies"
    IdefixArray4D<real> Vc = hydro->Vc;
    IdefixArray1D<real> x1 = hydro->data->x[IDIR];
    // the index in the active domain we use as reference
    int iref;
    if(side == left) {
      iref = hydro->data->beg[IDIR];
    } else if (side==right) {
      iref = hydro->data->end[IDIR]-1;
    }
    hydro->boundary->BoundaryFor("UserDefBoundary_X1Dust", dir, side,
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
*/

// Default constructor
// Initialisation routine. Can be used to allocate
// Arrays or variables which are used later on
Setup::Setup(Input &input, Grid &grid, DataBlock &data, Output &output)
{
  // Enroll the function used to define the boundary conditions
  // data.hydro->EnrollUserDefBoundary(&UserdefBoundary);

  // Enroll the function used to define the sound speed
  // data.hydro->EnrollIsoSoundSpeed(&MySoundSpeed);

  // Read the disc aspect ratio from the input file
  SetupVariables::h0 = input.Get<real>("Setup","h0",0);

  // -- Dust --
  // User def boundaries for the 1st dust speciy
  // data.dust[0]->EnrollUserDefBoundary(&UserdefBoundaryDust);

}

// This routine initialize the flow
// Note that data is on the device.
// One can therefore define locally
// a datahost and sync it, if needed
void Setup::InitFlow(DataBlock &data) {
    // Create a host copy
    DataBlockHost d(data);
    real h0 = SetupVariables::h0;

    for(int k = 0; k < d.np_tot[KDIR] ; k++) {
        for(int j = 0; j < d.np_tot[JDIR] ; j++) {
            for(int i = 0; i < d.np_tot[IDIR] ; i++) {
                real R=d.x[IDIR](i);
                real Vk=1.0/sqrt(R);

                real cs2=(h0*Vk)*(h0*Vk);

                d.Vc(RHO,k,j,i) = 100/R; // Surface density profile
                d.Vc(VX1,k,j,i) = 0.0;
                d.Vc(VX2,k,j,i) = Vk*sqrt( 1 - 3.0*h0*h0);

                /*
                // ---Tracers ---
                // We want to trace the material outside of the planet's orbital
                d.Vc(TRG,k,j,i) = ## TBF ##
                // --------------
                */


                /*
                // ---Dust grain ---
                // We want the dust grains to have initially
                // a density equal to 0.01 the gas density
                d.dustVc[0](RHO,k,j,i) = ## TBF ##
                d.dustVc[0](VX1,k,j,i) = ## TBF ##
                d.dustVc[0](VX2,k,j,i) = ## TBF ##
                */
            }
        }
    }

    // Send it all, if needed
    d.SyncToDevice();
}
