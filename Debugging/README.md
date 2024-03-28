# Programming in Idefix: Debugging and profiling

<!-- toc -->

- [Pre-requisities](#pre-requisities)
- [Problem1: a CPU segmentation fault](#problem1-a-cpu-segmentation-fault)
  * [Base run](#base-run)
  * [Track down the bug with Idefix_DEBUG](#track-down-the-bug-with-idefix_debug)
  * [Track down the bug with a debugger](#track-down-the-bug-with-a-debugger)
  * [Use Kokkos bound check to nail it down](#use-kokkos-bound-check-to-nail-it-down)
- [Problem2: a GPU segmentation fault](#problem2-a-gpu-segmentation-fault)
  * [Base run](#base-run-1)
  * [Let's debug this](#lets-debug-this)
- [Problem 3: GPU segmentation fault](#problem-3-gpu-segmentation-fault)
- [Problem 4: a low performance bug.](#problem-4-a-low-performance-bug)
  * [Tracking down performance issue: profiling with Kokkos](#tracking-down-performance-issue-profiling-with-kokkos)

<!-- tocstop -->

## Pre-requisities

This session assumes that you know how to connect and work on the Bigfoot cluster. If not, follow the [tutorial on bigfoot](../../env/README.md).

## Problem1: a CPU segmentation fault

### Base run
The first problem is a simple 1D shock tube problem. This can be compiled and run *on your laptop*.

```shell
cd idefix-days/tutorials/debugging/problem1
```

We then configure, compile and run the code
```shell
cmake $IDEFIX_DIR
make -j 8
./idefix
```

Do you see the problem?

### Track down the bug with Idefix_DEBUG

We first enable `Idefix_DEBUG` during the configuration phase:

```shell
cmake $IDEFIX_DIR -DIdefix_DEBUG=ON
```

then recompile and run
```shell
make -j 8
./idefix
```

As you can see, `Idefix_DEBUG` allows one to track what's happening in the code. This is based on the functions `idfx::pushRegion()` and `idfx::popRegion()` embedded in the code.

### Track down the bug with a debugger

Let's use `gdb` (or `lldb` for those without gdb on macs):
```shell
gdb ./idefix
```
Then once in gdb, run the code and backtrace the error:
```shell
run
bt
```
Depending on the compiler and the system, the error might happen at different places. It is usually close to the `SyncToDevice` method in `Setup::InitFlow`, defined in `setup.cpp`

From there, you would think the Idefix is crap since the main source code is making segmentation faults. You'd be wrong!

### Use Kokkos bound check to nail it down

When facing a segmentation fault on CPU, the first thing to check
is that you're not trying to read/write outside of an allocated array. This is not possible in standard C++, but it is possible thanks to Kokkos for every `IdefixArray`.

To enable this bound check, add the option to cmake during configuration:

```shell
cmake $IDEFIX_DIR -DKokkos_ENABLE_DEBUG_BOUNDS_CHECK=ON
```

then recompile and run
```shell
make -j 8
./idefix
```

Now, instead of a segmentation fault, you should see an exception raised by Kokkos. In particular, we're accessing an array outside of its bounds. If you now use the debugger as above, you will see which line in `setup.cpp` Kokkos is complaining about. Can you see now the mistake?

<details><summary>Solution</summary>

The for loops in `Setup::Initflow` have `np_tot` elements in each direction, hence the for loops should read (note the `<` instead of `<=`):

```c++
for(int k = 0; k < d.np_tot[KDIR] ; k++) {
    for(int j = 0; j < d.np_tot[JDIR] ; j++) {
        for(int i = 0; i < d.np_tot[IDIR] ; i++) {
```

</p>
</details>


## Problem2: a GPU segmentation fault

### Base run
The second problem is a pure thermal diffusion problem where the gas is kept fixed with 0 velocity. This can be compiled and run *on your laptop*.

```shell
cd idefix-days/tutorials/debugging/problem2
```

We then configure, compile and run the code
```shell
cmake $IDEFIX_DIR
make -j 8
./idefix
```

And this runs beaufiully, congrats!

Now, let's run this on a GPU. First follow the procedure describe in the [environement tutorial](../../env/README.md) to connect to a compute node and set up your environement, then go to the problem2 directory, configure for GPU, compile and run...

...and?

This is a typical example of a code that runs fine on a cpu but fails on GPU. These are very common problems that are also usually difficult to debug. Let's see how to proceed.

### Let's debug this

As for problem 1, the first step is to enable the debugging in Idefix. To do this, let's call cmake again

```shell
cmake $IDEFIX_DIR -DIdefix_DEBUG=ON <$YOUR_TEAM_FLAG>
```
where ``<$YOUR_TEAM_FLAG>`` is either ``$AMD_FLAGS`` or ``$NVIDIA_FLAGS``.
then recompile and run
```shell
make -j 4
./idefix
```
At this point, we see that an error occurs in the ``EnforcePeriodic`` method of the ``Boundary`` class. One would have thought that Idefix is more robust than this... but as it turns out, Errors on GPUs are always detected late by the host CPU, so it is *not* the point where the error actually occurs!

The key problem here is that the CPU and the GPU are not synchronized: the CPU keeps sending jobs to the GPU without waiting for each finish. Hence, the GPU is lagging behind, and errors are raised at random places in the CPU code by the GPU.

To fix this, we need to force both the CPU and GPU to stay in sync. This leads to poor performances, but since we're debugging, it's ok.

The easiest way to force CPU and GPU to stay in sync is to use the kernel logging module of [Kokkos Tools](https://github.com/kokkos/kokkos-tools). Luckily, we (the LOC) have already compiled a version of Kokkos tools on the machine you are running on, so you won't have to do it yourself (otherwise, just follow the documentation there).

Kokkos tools are always enabled *at runtime* by setting the environement variable ``KOKKOS_TOOLS_LIBS`` to the path to the tool you want to use. Note that there is no need to recompile!

Here, we're going to use the Kernel logger. The kernel logger forces Kokkos to show which kernel is being launched, where it is, and when it finishes. This forces CPU and GPU to stay in sync by adding ``Kokkos::fence`` at the end of each ``idefix_for``

So in practive, we enable Kokkos tools Kernel logger with
```shell
export KOKKOS_TOOLS_LIBS=$KOKKOS_TOOLS_DIR/debugging/kernel-logger/libkp_kernel_logger.so
```
and we next run the code
```shell
./idefix
```
This time, we see that the code complains about what's happening in a kernel named ``InternalBoundary``. The kernel name is the first parameter used in each ``idefix_for``: now you see why it's important to give maningful names!

This ``idefix_for`` is localised in setup.cpp, so you just have to find it, and possibly fix the problem !

<details><summary>Solution</summary>

The ``idefix_for`` loop contains a pointer to a fluid object (the variable ``hydro``). This pointer
is an argument of the function ``InternalBoundary``, hence it's a pointer in CPU memory. When the GPU runs it uses this pointer to find the array ``Vc`` but it can't find it, because it points to CPU memory, not GPU memory!

A way to fix this is to do copies of everything you need locally before calling ``idefix_for``. This rule should always been followed, as it solves 95% of the bugs. Here we can do:

```c++
  void InternalBoundary(Fluid<DefaultPhysics> * hydro, const real t) {
    // We shallow copy Vc locally first using the pointer in CPU memory space.
    IdefixArray4D<real> Vc = hydro->Vc;
    idefix_for("InternalBoundary",0,hydro->data->np_tot[KDIR],
                                  0,hydro->data->np_tot[JDIR],
                                  0,hydro->data->np_tot[IDIR],
                KOKKOS_LAMBDA (int k, int j, int i) {
                  // Here we live in GPU memory, so pointers to CPU memory are forbidden
                  Vc(VX1,k,j,i) = 0.0;
                  Vc(VX2,k,j,i) = 0.0;
                  Vc(VX3,k,j,i) = 0.0;
                });
  }
```

Note that the copy we do here on the first line is just a shallow copy. The memory content of
``Vc`` hasn't moved and hasn't been duplicated. We just duplicate the *reference* to the memory
content.

</p>
</details>

## Problem 3: GPU segmentation fault

Problem 3 is a disk+planet problem. It introduces the concept of additional source files, that are added to Idefix using the ``add_idefix_source`` function in the `CMakeLists.txt` of the setup (check it out). Here, the additional source files defines a new class that compute the sound speed at every point.

Follow the same procedure as for problem 2: configure, compile and run it on your laptop and then on the GPU of your choice. Follow the same debugging tracks as problem 3 and try to nail it down. Can you find where the error is?

<details><summary>Explanation</summary>

As you can see with the Kernel logger, the problem is clearly in the ``idefix_for`` called in ``SoundSpeed::Compute``. The problem is actually due to the variables ``Rcoord`` (a 1D ``IdefixArray``) and ``h0`` (a simple scalar). These variables are not defined in the function ``Compute`` but are instead member variables of the class ``SoundSpeed``. From the compiler point of view, these member variables are always accessed through the pointer ``this->`` that point to the current object. Hence, in this particular example, the compiler expands our ``idefix_for`` as:

```c++

  void SoundSpeed::Compute(IdefixArray3D<real> &cs) {
  idfx::pushRegion("SoundSpeed::Compute");
  idefix_for("MySoundSpeed",0,np_tot[KDIR],0,np_tot[JDIR],0,np_tot[IDIR],
              KOKKOS_LAMBDA (int k, int j, int i) {
                real R = this->Rcoord(i);
                cs(k,j,i) = this->h0/sqrt(R);
              });
  idfx::popRegion();
}
```

Now you clearly see the problem: the ``this->`` pointer, that point to the current object, is in CPU space, so the GPU can't find the variable we need. Can you find a way to fix this?
</p>
</details>

<details><summary>Solution</summary>

The solution is the same as for problem2: just do shallow copies:

```c++

  void SoundSpeed::Compute(IdefixArray3D<real> &cs) {
  idfx::pushRegion("SoundSpeed::Compute");
  IdefixArray1D<real> Rcoord = this->Rcoord;
  real h0 = this->h0;
  idefix_for("MySoundSpeed",0,np_tot[KDIR],0,np_tot[JDIR],0,np_tot[IDIR],
              KOKKOS_LAMBDA (int k, int j, int i) {
                real R = Rcoord(i);   // We're now using a local copy
                cs(k,j,i) = h0/sqrt(R); // Same for h0
              });
  idfx::popRegion();
}
```


This kind of bug is very common and very hard to track down sometimes. Actually, there are entire discussions about this [on the Kokkos repo](https://github.com/kokkos/kokkos/issues/695)... It turns out it is a defect of the C++ standard. Another workaround is to use ``KOKKOS_CLASS_LAMBDA`` instead of ``KOKKOS_LAMBDA``. This however copies the entire class content onto the GPU, which can therefore lead to a large overhead, and is therefore not recommended for general applications.

</p>
</details>

## Problem 4: a low performance bug.

Let's move to problem 4, which is again a planet-disk interraction problem. This can be compiled and run *on your laptop* or on the bigfoot cluster, but let's focus for now on the GPU version on the bigfoot cluster (you can try to do the exercise on your laptop, but you will need to compile Kokkos tools first). First go to the right directory

```shell
cd idefix-days/tutorials/debugging/problem4
```

We then configure
```shell
cmake $IDEFIX_DIR <$YOUR_TEAM_FLAG>
```
where ``<$YOUR_TEAM_FLAG>`` is either ``$AMD_FLAGS`` or ``$NVIDIA_FLAGS``,
then compile and run
```shell
make -j 4
./idefix
```
At this point, Idefix should run fine and finishes. While we could be satisfied, it's always a good idea to check the code performances, shown in the column cell update/s. This quantifies how many grid cells the code is able to update per second. Note that this number is for the whole code: if you are using MPI, the number of cell update per second should be proportional to the number of MPI processes.

In this particular case, we see that we get a few 1e7 cell updates/s on a single GPU. That's low: if you look at the [Idefix paper](https://ui.adsabs.harvard.edu/abs/2023arXiv230413746L/abstract), you'll see that we typically get at least 1e8 cell/sec on a single Nvidia V100 (that's about 4e8 cell/sec on a full node with 4 V100, see tables 3 & 4), and the test in the paper is 3D MHD cartesian. Our problem is 2D and hydro, so it should be more than this.

There are several reasons why Idefix could be slower: more complex physics (not quite applicable here), and a too small domain size for each GPU, which is not sufficient to feed all of the computational units of the GPU (reminder: there are 1000s of computational unit in a single V100). Here, the resolution is 1024^2 (more than 1e6 cells), that is equivalent to a 100^3 3D problem. This should be largely sufficient to feed a V100, so we clearly have a problem.

### Tracking down performance issue: profiling with Kokkos

While there are vendor-specific tools (like Nvidia systems), Idefix seeks portability. It turns out that Kokkos provides its own profiling tools: the space time stack. As usual with Kokkos tools, this is enabled by setting ``KOKKOS_TOOLS_LIBS`` to the path of the compiled space-time-stack library. In our case, this is:


```shell
export KOKKOS_TOOLS_LIBS=$KOKKOS_TOOLS_DIR/profiling/space-time-stack/libkp_space_time_stack.so
```

That's it, that's all, just re-run the code (no need to compile!), and wait...

```shell
./idefix
```

Now we you have all of the information about what the code is doing and where it's spending its time. Note *en passant* that the name of the regions is the one provided by ``idfx::pushRegion`` the names of the  "for" and "reduce" are that provided to ``idefix_for`` and ``idefix_reduce``, and finally, the name given to the ``IdefixArray`` constructor is reflected in the ``Kokkos::view::initialization``. So all these strings that are provided in the code turns out really useful!

  From this inspection, can you tell what is the problem?

<details><summary>Analysis of the bug</summary>

As you can see in the space-time stack, the code spends a lot of time in the user-defined analysis function, and in particular in the Host copy of the datablock. That's a typical example where you see that transferring data from the GPU to the CPU is actually relatively slow. Now that we have understood that the code spends a lot of time in the analysis function, can you find an easy fix to this?

</p>
</details>

<details><summary>Solution</summary>

If you inspect `idefix.ini`, you will see that the entry ``analysis`` of the block ``[Output]`` is set to 0. This means that idefix will run the user-defined analysis at each time step. That's probably not what was intended, so the best thing to do is to put a non-zero number to ``analysis``, like 0.01. After this, check that you recover the expected performance!
</p>
</details>
