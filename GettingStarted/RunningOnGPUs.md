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

This tutorial assumes that you have an account on the Jureca cluster and that you can log into `jureca.fz-juelinch.de`.

<a id="environement"></a>
## Environement and Code setup

First log into the cluster

```shell
ssh yourlogin@jureca.fz-juelinch.de
```

Then clone idefix. For this, we will put the code into a directory of your choice `<your_favourire_directory>`:
```shell
cd <your_favourire_directory>
git clone --recurse-submodules https://github.com/idefix-code/idefix.git idefix
export IDEFIX_DIR=$PWD/idefix
```

We then load the environement required by Idefix: cmake and Cuda (we will be using Nvidia GPU, so Cuda is needed). Add the openmpi module if you want to use multiple GPUs in parallel.

<a id="modules"></a>
```shell
 module load Stages/2024 GCC/12.3.0 CUDA OpenMPI MPI-settings/CUDA CMake
```

and we're good to go!

## First tests

For this first test, we are going to run a simple Orszag-Tang test problem on a single GPU. First cd to the right directory

```shell
cd $IDEFIX_DIR/test/MHD/OrszagTang
```

<a id="configuration"></a>
### Configuring/compiling the code for GPUs using CMAKE

The code configuration can be a bit tricky. When you're not sure about the options, best is to use `ccmake`, a graphical version of `cmake` to switch on and off the options you need. Here, we know we're going to use 
the Ampere A100 GPUs, so the flags should be

```shell
cmake $IDEFIX_DIR -DKokkos_ENABLE_CUDA=ON -DKokkos_ARCH_AMPERE80=ON
make -j 8
```


The only difference is in the Kokkos_ARCH option, that tells Idefix's backend `Kokkos` to compile for that specific type of GPU. It is always possible to run Idefix compiled for an older architecture (Pascal) on a new one (Ampere), you will only get a warning: `running kernels compiled for compute capability 6.1 on device with compute capability 8.6 , this will likely reduce potential performance.`. The opposite however doesn't work, if you try you will get an error message `Kokkos::Cuda::initialize ERROR: likely mismatch of architecture`

Note that compilation for GPUs can take a looooooong time, so it is always recommended to parallelise the compilation with the `-j` option of `make`.

<a id="running"></a>
### Running the code on GPUs

In order to run *Idefix* interactively, we must get access to a GPU node. To get access to an accelerated node:

```shell
salloc -p dc-gpu --nodes=1 --gres=gpu:1 --time=0:10:0 -A <budget>
```
which gives you a 10 minutes slot on one GPU on the account `<budget>` (to be replaced by your own project)

You then simply launch the executable using srun:

```shell
srun ./idefix
```

You should see Idefix running and finishing rapidly its computation (you can compare the performances in cell/s to the ones you obtain on your laptop for instance for the same test). 
Don't forget to log out of the compute node so that others can try! 

<a id="mpi"></a>
### Multi-GPUs runs

Idefix can run on multiple GPUs (it's been tested on +4000 GPUs simultaneously). This requires an MPI installation compatible with Cuda (e.g. GPU-aware OpenMPI). If you have loaded the openmpi module [suggested above](modules), you should be able to compile a GPU version of Idefix with parallelisation support.

You should configure the code with CMake adding `-DIdefix_MPI=ON` to the command line and compile. If the compilation succeeds, then you can request a multi-GPU job (here a 8 GPUs job, on 2 nodes with 4 GPUs/nodes):

```shell
salloc -p dc-gpu --nodes=2 --gres=gpu:4 --time=0:10:0 -A <account>
```
and run idefix as in (here for 2 GPUs):

```shell
srun -n 8 ./idefix
```

Note that with the module configuration we used above, the code automatically uses NVLink when available and Cuda-Aware MPI (i.e. direct GPU-GPU communications).
