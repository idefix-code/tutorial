from pydefix import *
import numpy as np
import matplotlib.pyplot as plt
import inifix

# The output function
# the only argument is dataBlockHost python object, wrapping a dataBlockHost Idefix object
def output(data,grid,n):
  # GatherIdefixArray is provided by pydefix.
  # It combines all MPI subdomain into one array (not a problem here, as the full domain is reasonably small)
  # If MPI is not enabled, GatherIdefixArray merely does a local copy
  vx = GatherIdefixArray(data.Vc[VX1,:,:,:],data,broadcast=False,keepBoundaries=False)
    
  # fetch frequency from idefix input file
  conf = inifix.load("idefix.ini")
  forcing_frequency = conf["Setup"]["forcing_frequency"] 
  
  # only process #0 performs the output
  if prank==0:
    vxf = np.fft.fft(vx[0,0,:])    # fourier transform of vx
    freqs = np.fft.fftfreq(grid.np_int[IDIR],grid.dx[IDIR][0]) # array of spatial frequencies
      
    # find the peak of the fft
    ipeak = np.argmax(np.abs(vxf))
    lambda_peak = 1/np.fabs(freqs[ipeak])
    cs = forcing_frequency*lambda_peak
    print("Pydefix(output): @t=%f, peak wavelength=%e, measured sound speed=%e"%(data.t,lambda_peak,cs))

    


def initflow(data):
  # Initialize the flow
  data.Vc[RHO,:,:,:] = 1.0
  data.Vc[VX1,:,:,:] = 0
  
