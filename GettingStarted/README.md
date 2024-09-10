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

In this tutorial, you will learn how to use idefix on various architectures.
Some of the exercises involve Jupyer notebook and source code extracts. It is therefore recommended to clone this tutorial on your machine, so that you can directly use these source files and test what you are doing. In the directory of your choice:

```shell
git clone https://github.com/idefix-code/idefix-tutorial.git
git checkout Jureca
```
The last line allows you to reach the dedicated tutorial for Jureca.

# Basic setup
<a id="installation"></a>
## Download and install Idefix

Assuming you want to use https to get idefix (easiest option):

```shell
git clone --recurse-submodules https://github.com/idefix-code/idefix.git idefix
cd idefix
```

This will create and deploy Idefix in the directory `idefix`.


For conveniance, set the `IDEFIX_DIR` environment variable to the absolute path of the root directory of idefix. Assuming you have not changed directory:

```shell
export IDEFIX_DIR=$PWD
```


<a id="compilation"></a>
## Compile an example

Go to the example directory.
For instance:

```shell
cd $IDEFIX_DIR/test/HD/sod
```

Configure the code launching cmake (version >= 3.16) in the example directory:

```shell
cmake $IDEFIX_DIR
```

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

In order to check that our test produced the right result, we are going to use the script `testme.py`. To use this,
we first create a python environement in the directory `$IDEFIX_DIR/test` with the right modules

```shell
cd $IDEFIX_DIR/test
python3 -m venv ./env
source env/bin/activate
pip install -r python_requirements.txt
```

We now go back to the location where we ran our first test, and check that the solution we got is correct:

```sell
cd $IDEFIX_DIR/test/HD/sod
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

if your build is successful, you can now try to launch idefix with automatic domain decomposition:

```shell
mpirun -np 4 ./idefix
```

<a id="next"></a>
# What next?

Try to [run the code on GPUs](./RunningOnGPUs.md)

Try to [make your first setup](../SimpleSetup/README.md).

