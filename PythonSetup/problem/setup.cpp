#include "idefix.hpp"
#include "setup.hpp"

namespace SetupVariables
{
  real forcingFrequency;
  real forcingAmplitude;
}

// Add a sinusoidal forcing in the middle of the domain
void MyForcingTerm(Hydro *hydro, const real t, const real dt) {
  const real forcing_amplitude = 1e-2;
  const real forcing_frequency = 25.0;

  // Local copy of needed arrays
  IdefixArray4D<real> Uc = hydro->Uc;        // Array of conservative variables to be updated 
  IdefixArray1D<real> x1 = hydro->data->x[IDIR];

  const real fAmplitude = SetupVariables::forcingAmplitude;
  const real fFrequency = SetupVariables::forcingFrequency;
  idefix_for("MySourceTerm",
    0, hydro->data->np_tot[KDIR],
    0, hydro->data->np_tot[JDIR],
    0, hydro->data->np_tot[IDIR],
              KOKKOS_LAMBDA (int k, int j, int i) {
                if(std::fabs(x1(i))<1e-2) {
                    Uc(MX1,k,j,i) += fAmplitude*std::cos(2.*M_PI*fFrequency*t)*dt;
                }
      });
}

// Default constructor
// Can be used to allocate
// Arrays or variables which are used later on
Setup::Setup(Input &input, Grid &grid, DataBlock &data, Output &output) {
    // Tell idefix we want to add the source term "MyForcingTerm"
    data.hydro->EnrollUserSourceTerm(&MyForcingTerm);

    // Fetch user-def variables
    SetupVariables::forcingFrequency = input.Get<real>("Setup","forcing_frequency",0); 
    SetupVariables::forcingAmplitude = input.Get<real>("Setup","forcing_amplitude",0); 
}

// This routine initialize the flow
// Note that data is on the device.
// One can therefore define locally
// a datahost and sync it, if needed
void Setup::InitFlow(DataBlock &data) {
    // This is not called!
    IDEFIX_ERROR("You are using the Setup::Initflow method to initialise the flow. Try to use pydefix instead.");
}
