# A more advanced setup

## Before we start


In this tutorial we will introduce several important aspects hidden in the [Getting Started](../GettingStarted/README.md) tutorial: Host and Device memory space, the `idefix_loop` construct and the tricks associated with it.

This tutorial is not intended to duplicate Idefix documentation. It is strongly recommended to read the introduction in the programming guide regarding [Host and device](https://idefix.readthedocs.io/latest/programmingguide.html#host-and-device), [Arrays](https://idefix.readthedocs.io/latest/programmingguide.html#arrays) and [Loops](https://idefix.readthedocs.io/latest/programmingguide.html#execution-space-and-loops).

The tutorial can be executed on CPU or on GPU, with or without MPI enabled. Feel free to experiment any combination depending on your level of expertise.

## The problem
Our goal is to make a complete planet-disk interaction problem, where we will progressively add more complexity by making our own boundary condition, add a planet, include a passive tracer and even dust grains.

The setup provided in problem 1 is a simple 2D Keplerian disk in polar coordinates ($x_1=R,x_2=\phi$). The initial conditions are already written and defines a surface density profile $\Sigma=100 R^{-1}$ (the code does not include units). We have also defined the disk aspect ratio $h_0\equiv H/R=c_s/V_k$, which is read by the Setup constructor from idefix.ini. Here, $V_k$ is the Keplerian velocity, which reads with our units (central Mass=1), $V_K=1/\sqrt{R}$.

<!-- toc -->

- [Define the sound speed](#define-the-sound-speed)
  - [Your first idefix_for](#your-first-idefix_for)
  - [Fill the sound speed array](#fill-the-cskji-array-and-compile)
  - [Function enrollment](#function-enrollment)
  - [First visualization](#first-visualisation-of-your-result)
- [Add a planet](#add-a-planet)
- [Fix the radial boundary conditions](#fix-the-radial-boundaries)
- [Restart/stop the code](#restartingstopping-the-code)
- [Add a tracer](#add-a-tracer)
- [Add dust](#add-dust-grains)

<!-- tocstop -->

## Define the sound speed

We assume the flow is locally isothermal, meaning that we assume the temperature (and therefore the sound speed, since $T\propto c_s^2$) at each radius $R$ is fixed. As you can see in `idefix.ini`, in the [Hydro] block, we have said that the sound speed is user-defined. This is because we want to tell idefix explicitely which function it should use for the sound speed. To define this sound speed, we are going to assume that the disk aspect ratio is constant, so that $c_s=h_0 V_K=h_0/\sqrt{R}$. 

Tp do this, we have already started to write a function `MySoundSpeed` in `setup.cpp`, in which we have already gathered the radial coordinate array ($x_1$) and the aspect ratio ($h_0$). The goal of this function is to fill an idefixArray `cs` (that appears as a parameter of this function) that idefix will use to get the sound speed at each point.

### Your first idefix_for
Your first task is to invoke an `idefix_for` (read the [doc](https://idefix.readthedocs.io/latest/programmingguide.html#execution-space-and-loops)!). This idefix for should cover the entire domain (or sub-domain if using MPI) of the simulation.
The domain extends from $0$ to `data.np_tot[IDIR]` in the x1 direction, $0$ to `data.np_tot[JDIR]` in the x2 direction, and $0$ to `data.np_tot[KDIR]` in the x3 direction (see the [dataBlock documentation](https://idefix.readthedocs.io/latest/programmingguide.html#execution-space-and-loops)). Note that in idefix, it is cusommary to use first the `k` index running on `x3`, then `j` running on `x2` etc. This is to ensure that the fastest running index is always `i` and is also the last  index of every array for optimal performance.

### Fill the cs(k,j,i) array and compile
Next, you should fill the array `cs(k,j,i)` with the expression we want for the sound speed. This will be the core of your "compute kernel", i.e. the code that will be effectively executed by the device. Here, and very often in idefix, this kernel is defined as a `KOKKOS_LAMBDA`, which is a simple, inlined way to define a function to be executed by the device. 

At this point, you can try to configure and compile the code, and it should build properly. However, if you run it, you will get an error message. The reason is simple: we have defined a function to compute the sound speed array, we have told idefix that we were going to use a user-defined function, but we have not said *where* was this function! This is the role played by enrollment.

### Function enrollment

The Enrollment is a very common thing in idefix: each time you define a "user-defined" function, you need to Enroll it: i.e. tell idefix where it is. For computer people, this can be considered as linking your functions at runtime to the main idefix code.

Enrollment is usually done in the Setup constructor. Since we have created a function to compute the sound speed, you should enroll it using the `EnrollIsoSoundSpeed` function. You can find the right line commented in the Setup constructor. Uncomment it and recompile the code. It should now run well.

### First visualisation of your result

The code will integrate the equations of motion for 3 orbital period at R=1. You can visualize the flow with paraview, or with the provided python script `read_problem.py` to see the flow after 1 orbit, simply type
```shell
python3 read_problem.py 1
```
You should see a weak axisymmetric wave propagating outwards from the inner boundary condition. We will come back to this inner boundary condition later. For now, let's add a planet.

## Add a planet

To add a planet, we must modify the gravitational potential. This can be done by writing your own gravitational potential. But you're in luck, because planet interaction problems are so common, this has already been done for you. You will be able to add your planet without recompiling the code!

Everything happens in idefix.ini: first, you must add the planet to the gravitationnal potential. In the `[Gravity]` block, in the entry `potential` add `planet` next to `central`. This way, idefix will understand that you want planets in addition to the central potential.

Next, we must describe our planets. This is done in the `[Planet]`block. Have a look at the dedicated [documentation of the planet module](https://idefix.readthedocs.io/latest/modules/planet.html). For now, you can simply uncomment the proposed configuration for our (single!) planet in `idefix.ini`.

Now you can re-run the code (no need to recompile) and tada! here comes our planet.

## Fix the radial boundaries

As pointed out above, our radial boundary conditions are "outflow". This is partly incorrect because we are in a Keplerian disc, so expect $v_\phi$ to be close to Keplerian, while the outflow copies the last active zone value into the ghost zones. Hence, a better boundary would be to copy the last active zone except for $v_\phi$, where we would like the flow to be Keplerian in the ghost zones. This means that we need to code our own boundary conditions.

The first step is to modify the `[Boundary]` block in `idefix.ini`, to say that you want to use `userdef` boundaries in the X1 directions (at both ends). From this point, idefix will expect you to enroll a function to define the boundaries.

We then move to `setup.cpp`. You will see that we have already defined a function `UserdefBoundary`, that needs to be completed. So we first enroll this function as a boundary function in the `Setup` constructor (by now, you should know how to do this). Next, we need to define what's happening in the ghost zones of our domain in the `Userdefboundary` function.

In order to simplify your life, idefix comes with pre-defined loops on the boundaries, called `BoundaryFor`. These loops are identical to `idefix_for`, but they automatically define their bounds according to the direction and side of the boundary condition to be defined. Our plan is therefore to apply our new boundary conditions by copying the first active zone (with index `iref`) to the ghost zones, except for $v_\phi$ (`VX2`), for which we want to impose a Keplerian profile.

Once done, you have to recompile and re-run the code. You should see a slight improvement of the solution at the radial boundaries.

## Restarting/stopping the code

As any code, idefix can restart a simulation that have been saved to disk. This is done using the .dmp files (called a dump file), a format specific to Idefix, that contains all of the variables at the precision required during the code configuration. These dump files do not depend on the architecture, you can restart with your favourite GPU a run that started on a GPU. Similarly, you can change the domain decomposition and/or enable/disable MPI altogether. You can also remove/add some physics at restart. The only thing fixed is the total domain resolution and extent: idefix doesn't interpolate from a dump.

Our current setup produces one dump every orbit (check the `[Output]` block in idefix.ini). In order to restart from a dump, use the `-restart n` option when calling idefix, where `n` is the dump file number, e.g.

```shell
./idefix -restart 1
```

Note that if you omit the dump number `n`, idefix will automatically restart from the latest produced dump file. This can be handy when your run time is limited by the job scheduling of your cluster.

It is possible to nicely stop the code while running. Just go to the directory where the code has been launched from, and create an empty `stop` file:

```shell
cd <running_idefix_dir>
touch stop
```
This automatically makes a dump and stops the code. It is also possible to set a [maximum runtime](https://idefix.readthedocs.io/latest/reference/idefix.ini.html#timeintegrator-section) in `idefix.ini`, and stop the code using [POSIX signals](https://idefix.readthedocs.io/latest/reference/commandline.html#signal-handling). 

## Add a tracer

A passive tracer, or a scalar, is a quantity $\sigma$ that follows the equation
$$
\partial_t \sigma+\bold{v\cdot \nabla}\sigma=0
$$
where $v$ is the fluide velocity. Idefix support an arbitrary number of tracers, on every fluid (i.e. gas and dust).

To enable a single tracer to our gas, just add a `tracer` entry in the `[hydro]` block with `1` as a parameter (meaning 1 tracer).

This work out of the box, but since we have not defined the initial distribution of our tracer, it will be useless. Therefore, we also have to provide an initial condition for this tracer. In the `InitFlow` function of your setup, you will see that you have commented the initial condition for the tracer. Your task is to uncomment this and define a tracer that is initially 0 for the material inside the planet orbit, and 1 for the material outside of the planet orbit. This will allow us to trace how much material from the outer disc manage to cross the planet orbit.

Once done, you will have to recompile, run the code, and check what's happening at your tracer.

## Add dust grains

The current public version of Idefix can only treat dust grains as a zero pressure fluid. Idefix can treat an arbitrary number dust fluid, each one representing a dust size.

In order to set up a configuration, you will need 3 modifications:
- enable dust grains in `idefix.ini`
- define the initial condition for each dust fluid (=size)
- implement the radial boundary condition for the dust fluid, as we did for the gas.

### Enable dust grains in idefix.ini

For this, we need to create a new [Dust] block. A block like this is already there and commented in idefix.ini. The only mandatory in this block is the number of dust species, that we set to 1. We additionnally enable a drag force between the dust and the gas. This drag force assumes a constant stopping time, which is kind of incorrect, but is a good first approximation. We set the stopping to 1 code unit. More information on the dust module and its drag force [in the documentation](https://idefix.readthedocs.io/latest/modules/dust.html).

### Define the initial conditions

As for the tracer, we need to define the initial conditions for the dust. As usual, this is in the initFlow method, where you will see that we have commented the fields linked to the first dust specie. Each dust specie is identified by an index (here `0`), and has a density and a velocity, exactly like the gas (computationnaly speaking, both are described by the same `Fluid` template class). Fill in the initial conditions for the dust so that it has the same velocity as the gas, with a density equal to 1/100 the gas density.

### implement the radial boundary conditions

Since the dust is a new kind of fluid, it requires its own boundary conditions. We have already prepared a function for this. You just need to uncomment it and fill in the boundary conditions. Let's use the same conditions as the gas for now. Do not forget to enroll your new function in the constructor!

From this point, you can let the code execute. You will probably observe that some regions becomes dust-free while some others tend to be pressure traps. Congratulations, you have finished this tutorial!



