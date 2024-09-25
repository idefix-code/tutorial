# Your first steps with Idefix

<!-- toc -->

- [About this tutorial](#about)
- [Installation](#installation)
- [Compilation](#compilation)
- [Running](#running)
- [Code Validation](#validation)
- [Run in parallel with MPI](#mpi)
- [Next tutorial: your first setup](#next)

<!-- tocstop -->
<a id="about"></a>
# About this tutorial
This tutorial is provided as a github repository and is mirrored on Jureca for easier access. It is part of the [Toward Exascale-Ready Astrophysics workshop](https://indico3-jsc.fz-juelich.de/event/169/) and has been prepared by Geoffroy Lesur (geoffroy.lesur@univ-grenoble-alpes.fr)

In this tutorial, you will learn how to use idefix on various architectures. Here, we will do everything through a Jupyter notebook opened on Jureca on the `dc-gpu` partition.

## Deploy on Jureca

Log in to https://jupyter-jsc.fz-juelich.de/

Open a lab environment with

- Lab Config:
    - System: JURECA
    - Project: training2437
    - Partition: dc-gpu
    - Reservation: tera_day2
- Resources (opens once dc-gpu is selected)
    - Nodes: 1
    - GPUs: 4
    - Runtime: 90
Kernels and extensions: keep defaults


First open a new console on your Jupyter notebook. We then clone the idefix Github repository. Since we don't have direct access to the internet, we use a small script to copy the sources and the tutorial from a shared directory:

```shell
source /p/project1/training2437/tera_day2/idefix/deploy.sh
```

This will put everything into `/p/project1/training2437/tera_day2/$USER/idefix`

The deploy script already set up the environement (module and environement variable). If you loose connection and need a new console, you can reload the environement:

```shell
source /p/project1/training2437/tera_day2/idefix/env.sh
```

and you're good to go!

## Optionnal: Deploy on your machine

<details> 
<summary>Click here to deploy Idefix on your laptop.</summary>
<br>
Optionnally, you can play around with this tutorial on your laptop/machine. In this case you can clone this tutorial and idefix source code on your machine, so that you can directly use these source files and test what you are doing. In the directory of your choice (this requires an internet access):

```shell
git clone --recurse-submodules https://github.com/idefix-code/idefix.git idefix.src
export IDEFIX_DIR=$PWD/idefix.src
git clone https://github.com/idefix-code/tutorial.git
cd tutorial
git checkout Jureca
```
The last line allows you to reach the dedicated tutorial for Jureca.

For conveniance, we set the `IDEFIX_DIR` environment variable to the absolute path of the root directory of idefix (as above). 

If you intend to use the python script provided in this tutorial, best is to deploy a python environement with everything already set up. 
We therefore create a python environement in the directory `$IDEFIX_DIR/test` with the right modules (this may require an internet access)

```shell
cd $IDEFIX_DIR/test
python3 -m venv ./env
source env/bin/activate
pip install -r python_requirements.txt
```

</details>

<a id="compilation"></a>
## Compile an example

Let's play with a simple Sod shock tube test in hydro:

```shell
cd $IDEFIX_DIR/test/HD/sod
```

Configure the code launching cmake (version >= 3.16) in the example directory:

```shell
cmake $IDEFIX_DIR
```

By default, this will configure the code to run on the CPU only. We will see later how to configure the code for GPU.

Several options can be enabled from the command line (a complete list is available with `cmake $IDEFIX_DIR -LH`). For instance: `-DIdefix_RECONSTRUCTION=Parabolic` (enable PPM reconstruction), `-DIdefix_MPI=ON` (enable mpi), `-DKokkos_ENABLE_OPENMP=ON` (enable openmp parallelisation), etc... For more complex target architectures, it is recommended to use cmake GUI launching `ccmake $IDEFIX_DIR` in place of `cmake` and then switching on the required options.

One can then compile the code:

```shell
make -j8
```

<a id="running"></a>
## Run an example

launch the executable

```shell
./idefix
```

You should see idefix finishing successfully.
<a id="validation"></a>
## Code Validation

Most of tests provided in the `test/` directory can be validated against analytical solution (standard test)
and/or pre-computed solutions (non-regression tests). Note that the validation relies on large reference
files that are stored in the separate `idefix-code/reference` repository that is automatically cloned as a submodule.

In order to check that our test produced the right result, we are going to use the script `testme.py`. 

```sell
./testme.py -check
```

> :warning: **If you are using a Mac with an ARM cpu (M1/M2)**: The non-regression test might not succeed (but standard tests should always pass): this is linked to slight differences in the way roundoff errors are treated on these architectures.
<a id="mpi"></a>
## Run in parallel with MPI

Note: This section requires an MPI library on your machine.

In order to use Idefix with parallel domain decomposition (either on CPUs or on GPUs), you should first configure the code with MPI enabled using the `Idefix_MPI=ON` option. Let's try that for the Orszag-Tang vortex test

```shell
cd $IDEFIX_DIR/test/MHD/OrszagTang
cmake $IDEFIX_DIR -DIdefix_MPI=ON
make -j 8
```

if your build is successful, you can now try to launch idefix with automatic domain decomposition. On a Jureca node (using Slurm):

```shell
srun -n 4 ./idefix
```

<details>
<summary>(optional) On your laptop:</summary>

```shell
mpirun -np 4 ./idefix
```
</details>

<a id="next"></a>
# What next?

Try to [run the code on GPUs](./RunningOnGPUs.md)

Try to [make your first setup](../SimpleSetup/README.md).

