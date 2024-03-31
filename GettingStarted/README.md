# idefix-tutorial
Tutorial series for Idefix
<!-- toc -->

- [Download:](#download)
- [Installation:](#installation)
- [Compile an example:](#compile-an-example)
- [Running](#running)
  * [serial (gpu/cpu), openMP (cpu)](#serial-gpucpu-openmp-cpu)
  * [With MPI (cpu)](#with-mpi-cpu)
  * [With MPI (gpu)](#with-mpi-gpu)
- [Profiling](#profiling)
- [Debugging](#debugging)
- [Code Validation](#code-validation)
- [Contributing](#contributing)

<!-- tocstop -->

Download:
---------

Assuming you want to use https to get idefix (easiest option):

```shell
git clone --recurse-submodules https://github.com/idefix-code/idefix.git idefix
cd idefix
```

This will create and deploy Idefix in the directory `idefix`.

Installation:
-------------

Set the `IDEFIX_DIR` environment variable to the absolute path of the directory

```shell
export IDEFIX_DIR=<idefix main folder>
```

Add this line to `~/.<shell_rc_file>` for a permanent install.


Compile an example:
-------------------
Go to the example directory.
For instance:

```shell
cd test/HD/sod
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

Running
-------------------
### serial (gpu/cpu), openMP (cpu)
simple launch the executable

```shell
./idefix
```

You should see idefix finishing successfully.

Code Validation
---------------

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
./testme.py -all
```

Tests require Python 3 along with some third party dependencies to be installed.
To install those deps, run
```shell
pip install -r test/python_requirements.txt
```

### With MPI (cpu)
`-dec` can be used to specify a domain decomposition manually.

It can be omitted for 1D problems, or if `NX`, `NY`, `NZ` and `nproc` are **all** powers of 2.
Otherwise, `-dec` is mandatory. For instance, in 2D, using a 2x2 domain decomposition:

```shell
mpirun -np 4 ./idefix -dec 2 2
```

or in 3D, using a 1x2x4 decomposition:

```shell
mpirun -np 8 ./idefix -dec 1 2 4
```
