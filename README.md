# idefix-tutorial
Tutorial series for Idefix

## Requirements

In order to use Idefix, you will need
- cmake > 3.16
- git
- a C++ compiler (clang++, g++)
- an MPI library compatible with the MPI 3 (e.g. OpenMPI > 4.0)


# This tutorial

You should proceed with this tutorial in the following order:

1. Try to deploy idefix on your machine, compile and test an exemple in [Getting Started](GettingStarted/README.md)
2. Deploy idefix on a GPU cluster, learn how to configure and compile for GPUs and run a test in [Running on GPUs](GettingStarted/RunningOnGPUs.md)
3. Make your first setup (a Kelvin-Hemholtz instability), learn how to set up your initial conditions and read from the input file in [a Simple Setup](SimpleSetup/README.md)
4. Make a more advanced setup with a protoplanet embedded in a disk. Code your first `idefix_for` loop and learn how to custom boundary conditions. Add tracers and even dust grains in the [advanced setup](AdvancedSetup/README.md)
5. Learn different visualisation technics, from the very simple `readVTK` functions to the most advanced `paraview` in the [visualisation tutorial](VisualisationAndPost/README.md)
6. Discover the usual mistakes and tips to detect and debug them in the [debugging tutorial](Debugging/README.md)