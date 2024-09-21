#include "idefix.hpp"
#include "setup.hpp"

namespace SetupVariables {
  real flowVelocity;
}


// Constructor of our setup, which is called when Idefix starts up. 
// It can be used to allocate
// Arrays or variables which are used later on
Setup::Setup(Input &input, Grid &grid, DataBlock &data, Output &output) {

  // read the flow velocity parameter from the input file
  // and store it in the global variable flowVelocity

  SetupVariables::flowVelocity = input.Get// !!TBF //
}


// This routine initialize the flow
// Note that data is on the device.
// To simplify the setup, we make a 
// copy of the datablock on theHost, and sync it
// when done
void Setup::InitFlow(DataBlock &data) {
  // Create a host copy
  DataBlockHost dataHost(data);


  for(int k = 0; k < dataHost.np_tot[KDIR] ; k++) {
    for(int j = 0; j < dataHost.np_tot[JDIR] ; j++) {
      real y = dataHost.x[JDIR](j);
      for(int i = 0; i < dataHost.np_tot[IDIR] ; i++) {
        real x = dataHost.x[IDIR](i);

        // KHI
        dataHost.Vc(RHO,k,j,i) = ONE_F;

        // define an oscillating interface around y=0.5
        real yInterface =  0.5 + 0.05 * ( std::sin(0.5 * M_PI * x) + 
                                          std::cos(4.0 * M_PI * x)
                                        );

        dataHost.Vc(VX1,k,j,i) = // !!TBF //
        dataHost.Vc(VX2,k,j,i) = // !!TBF //

      }
    }
  }

  // Send our initial condition to the device!
  dataHost.SyncToDevice();
}
