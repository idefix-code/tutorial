# Running on a GPU

The idea behind Idefix is to be able to run easily your code on a CPU or a GPU cluster. Here, we will demonstrate how this is done on the LMU cluster.

## Requirements

This tutorial assumes that you have an account on the LMU cluster and that you can log into `login.physik.uni-muenchen.de`.

## Environement and Code setup

First log into the cluster

```shell
ssh cluster-el9.hpc.physik.uni-muenchen.de
```

Then clone idefix. For this, we will put the code into a directory of your choice `<your_favourire_directory>`:
```shell
cd <your_favourire_directory>
git clone --recurse-submodules https://github.com/idefix-code/idefix.git idefix
export IDEFIX_DIR=$PWD/idefix
```

We then load the environement required by Idefix: cmake and Cuda (we will be using Nvidia GPU, so Cuda is needed). 

```shell
module load spack/2023.11
module load cmake/3.20.2-gcc-11.3.1 cuda/11.8.0
```

and we're good to go!

## First tests

For this first test, we are going to run a simple Orszag-Tang test problem on the GPU. First cd to the right directory

```shell
cd $IDEFIX_DIR/test/MHD/OrszagTang
```

### configuring/compiling the code for GPUs using CMAKE

The code configuration can be a bit tricky. When you're not sure about the options, best is to use `ccmake`, a graphical version of `cmake` to switch on and off the options you need. Here, we know we're going to use either the A40 GPUs 
or the P5000, and I'm telling you explicitely the flags needed.

For the A40 GPUs, you should configure and compile Idefix with:

```shell
cmake $IDEFIX_DIR -DKokkos_ENABLE_CUDA=ON -DKokkos_ARCH_AMPERE86=ON
make -j 8
```

For the P5000, you should do instead:
```shell
cmake $IDEFIX_DIR -DKokkos_ENABLE_CUDA=ON -DKokkos_ARCH_PASCAL61=ON
make -j 8
```

The only difference is in the Kokkos_ARCH option, that tells Idefix's backend `Kokkos` to compile for that specific type of GPU. It is always possible to run Idefix compiled for an older architecture (Pascal) on a new one (Ampere), you will only get a warning: `running kernels compiled for compute capability 6.1 on device with compute capability 8.6 , this will likely reduce potential performance.`. The opposite however doesn't work, if you try you will get an error message `Kokkos::Cuda::initialize ERROR: likely mismatch of architecture`

Note that compilation for GPUs can take a looooooong time, so it is always recommended to parallelise the compilation with the `-j` option of `make`.

### Running the code on GPUs

In order to run *Idefix* interactively, we must get access to a GPU node on the LMU cluster. If using the A40, this is done through:

```shell
intjob --gres=gpu:a40 --time=0:05:0
```
which gives you a 5 minutes slot on a A40 GPU node. For the p5000, replace `a40` by `p5000`.

You then simply launch the executable:

```shell
./idefix
```

You should see Idefix running and finishing rapidly its computation (you can compare the performances in cell/s to the ones you obtain on your laptop for instance for the same test). 
Don't forget to log out of the compute node so that others can try! You can check

### Multi-GPUs runs

In principle, Idefix can run on multiple GPUs (it's been tested on +4000 GPUs simultaneously). However, this requires an MPI installation compatible with Cuda (e.g. GPU-aware OpenMPI). At the time of writing, this is not available on the LMU cluster. You can still give it a try:

```shell
module purge
module load spack/2023.11
module load cmake/3.20.2-gcc-11.3.1 cuda/11.8.0 openmpi
```

Then compile the code adding `-DIdefix_MPI=ON` to the command line. If the compilation succeeds, then you can request a multi-GPU job and run idefix as in (here for 2 GPUs):

```shell
mpirun -np 2 ./idefix
```

