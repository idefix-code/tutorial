# Running on a GPU

The idea behind Idefix is to be able to run easily your code on a CPU or a GPU cluster. Here, we will demonstrate how this is done on the Jureca cluster in Germany.

<!-- toc -->

- [Requirements](#requirements)
- [Set up the environement](#environement)
- [First test](#first-tests)
  - [Configuration/compilation on GPUs](#configuration)
  - [Running on GPUs](#running)
  - [Multi-GPU runs](#mpi)


<!-- tocstop -->


## Requirements

This tutorial assumes that you have already deployed the code on Jureca following [Getting Started](README.md)

<a id="environement"></a>
## Environement and Code setup

## First tests

For this first test, we are going to run a simple Orszag-Tang test problem on a single GPU. First cd to the right directory

```shell
cd $IDEFIX_DIR/test/MHD/OrszagTang
```

<a id="configuration"></a>
### Configuring/compiling the code for GPUs using CMAKE

The code configuration can be a bit tricky. When you're not sure about the options, best is to use `ccmake`, a graphical version of `cmake` to switch on and off the options you need. Here, we know we're going to use 
an Nvidia GPU so we will be using CUDA. Moreover, we will configure the code on a GPU node, so we can let cmake auto-detect the right Nvidia architecture for us:

```shell
cmake $IDEFIX_DIR -DKokkos_ENABLE_CUDA=ON
make -j 8
```

While the code configure, you will see that it indeed auto-detect the `AMPERE_80` architecture. If this auto-configuration was to fail (i.e. configuring on a login node), we could add `-DKokkos_ARCH_AMPERE80=ON` to specify the right architecture.

Note that it is always possible to run Idefix compiled for an older architecture (Pascal) on a new one (Ampere), you will only get a warning: `running kernels compiled for compute capability 6.1 on device with compute capability 8.6 , this will likely reduce potential performance.`. The opposite however doesn't work, if you try you will get an error message `Kokkos::Cuda::initialize ERROR: likely mismatch of architecture`

Note that compilation for GPUs can take a looooooong time, so it is always recommended to parallelise the compilation with the `-j` option of `make`.

<a id="running"></a>
### Running the code on GPUs

You then simply launch the executable using srun:

```shell
./idefix
```

You should see Idefix running and finishing rapidly its computation (you can compare the performances in cell/s to the ones you obtain on your laptop for instance for the same test). 

<a id="mpi"></a>
### Multi-GPUs runs

Idefix can run on multiple GPUs (it's been tested on +4000 GPUs simultaneously). This requires an MPI installation compatible with Cuda (e.g. GPU-aware OpenMPI). If you have loaded the environement in [Getting Started](README.md), you should be able to compile a GPU version of Idefix with parallelisation support.

You should first configure the code with CMake adding `-DIdefix_MPI=ON` to the command line and compile. 

```shell
cmake $IDEFIX_DIR -DKokkos_ENABLE_CUDA=ON -DIdefix_MPI=ON
make -j 8
```

If the compilation succeeds, then you can run a multi-GPU simulation with 4 gpus:

```shell
srun -n 4 ./idefix
```

Note that with the module configuration we used above, the code automatically uses NVLink when available and Cuda-Aware MPI (i.e. direct GPU-GPU communications).
