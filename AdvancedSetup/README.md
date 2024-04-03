# A more advanced setup

In this tutorial, we are going to set up a simple planet-disk interraction problem. This will be the opportunity to introduce several important aspects hidden in the [Getting Started](../GettingStarted/README.md) tutorial: Host and Device memory space, the `idefix_loop` construct and the tricks associated with it.

## Before we start

This tutorial is not intended to duplicate Idefix documentation. It is strongly recommended to read the introduction in the programming guide regarding [Host and device](https://idefix.readthedocs.io/latest/programmingguide.html#host-and-device), [Arrays](https://idefix.readthedocs.io/latest/programmingguide.html#arrays) and [Loops](https://idefix.readthedocs.io/latest/programmingguide.html#execution-space-and-loops)

## The problem

The setup provided in problem 1 is a simple 2D Keplerian disk in polar coordinates ($x_1=R,x_2=\phi$). The initial conditions are already writte defines a surface density profile $\Sigma=100 R^{-1}$ (the code does not include units). We have also defined the disk aspect ratio $h_0\equiv H/R=c_s/V_k$, which is read by the Setup constructor from idefix.ini. Here, $V_k$ is the Keplerian velocity, which reads with our units (central Mass=1), $V_K=1/\sqrt{R}$.

### Define the sound speed

We assume the flow is locally isothermal, meaning that we assume the temperature (and therefore the sound speed, since $T\propto c_s^2$) at each radius $R$ is fixed. As you can see in `idefix.ini`, in the [Hydro] block, we have said that the sound speed is user-defined. This is because we want to tell idefix explicitely which function it should use for the sound speed. To define this sound speed, we are going to assume that the disk aspect ratio is constant, so that $c_s=h_0 V_K=h_0/\sqrt{R}$. 

For this we have already started to write a function `MySoundSpeed` in `setup.cpp`, in which we have already gathered the radial coordinate array ($x_1$) and the aspect ratio ($h_0$). The goal of this function is to fill an idefixArray `cs` (that appears as a parameter of this function) that idefix will use to get the sound speed at each point.

#### Your first idefix_for
Your first task is to invoke an `idefix_for` (read the [doc](https://idefix.readthedocs.io/latest/programmingguide.html#execution-space-and-loops)!). This idefix for should cover the entire domain (or sub-domain if using MPI) of the simulation.
The domain extends from $0$ to `data.np_tot[IDIR]` in the x1 direction, $0$ to `data.np_tot[JDIR]` in the x2 direction, and $0$ to `data.np_tot[KDIR]` in the x3 direction (see the [dataBlock documentation](https://idefix.readthedocs.io/latest/programmingguide.html#execution-space-and-loops)). Note that in idefix, it is cusommary to use first the `k` index running on `x3`, then `j` running on `x2` etc. This is to ensure that the fastest running index is always `i` and is also the last  index of every array for optimal performance.

#### Fill the cs(k,j,i) array and compile
Next, you should fill the array `cs(k,j,i)` with the expression we want for the sound speed.

At this point, you can try to configure and compile the code, and it should build properly. However, if you run it, you will get an error message. The reason is simple: we have defined a function to compute the sound speed array, we have told idefix that we were going to use a user-defined function, but we have not said *where* was this function! This is the role played by enrollment.

#### Function enrollment

The Enrollment is a very common thing in idefix: each time you define a "user-defined" function, you need to Enroll it: i.e. tell idefix where it is. For computer people, this can be considered as linking your functions at runtime to the main idefix code.

Enrollment is usually done in the Setup constructor. Since we have created a function to compute the sound speed, you should enroll it using the `EnrollIsoSoundSpeed` function. You can find the right line commented in the Setup constructor. Uncomment it and recompile the code. It should now run well.

The code will integrate the equations of motion for 3 orbital period at R=1. You can visualize the flow with paraview, or with the provided python script `read_problem.py` to see the flow after 1 orbit, simply type
```shell
python3 read_problem.py 1
```
You should see a weak axisymmetric wave propagating outwards from the inner boundary condition. We will come back to this inner boundary condition later. For now, let's add a planet.

### Add a planet

To add a planet, we must modify the gravitational potential. This can be done by writing your own gravitational potential. But you're in luck, because planet interaction problems are so common, this has already been done for you. You will be able to add your planet without recompiling the code!

Everything happens in idefix.ini: first, you must add the planet to the gravitationnal potential. In the `[Gravity]` block, in the entry `potential` add `planet` next to `central`. This way, idefix will understand that you want planets in addition to the central potential.

Next, we must describe our planets. This is done in the `[Planet]`block. Have a look at the dedicated [documentation of the planet module](https://idefix.readthedocs.io/latest/modules/planet.html). For now, you can simply uncomment the proposed configuration for our (single!) planet in `idefix.ini`.

Now you can re-run the code (no need to recompile) and tada! here comes our planet.

### Fix the radial boundaries

As pointed out above, our radial boundary conditions are "outflow". This is partly incorrect because we are in a Keplerian disc, so expect $v_\phi$ to be close to Keplerian, while the outflow copies the last active zone value into the ghost zones. Hence, a better boundary would be to copy the last active zone except for $v_\phi$, where we would like the flow to be Keplerian in the ghost zones. This means that we need to code our own boundary conditions.

The first step is to modify the `[Boundary]` block in `idefix.ini`, to say that you want to use `userdef` boundaries in the X1 directions (at both ends). From this point, idefix will expect you to enroll a function to define the boundaries.

We then move to `setup.cpp`. You will see that we have already defined a function `UserdefBoundary`, that needs to be completed. So we first enroll this function as a boundary function in the `Setup` constructor (by now, you should know how to do this). Next, we need to define what's happening in the ghost zones of our domain in the `Userdefboundary` function.

In order to simplify your life, idefix comes with pre-defined loops on the boundaries, called `BoundaryFor`. These loops are identical to `idefix_for`, but they automatically define their bounds according to the direction and side of the boundary condition to be defined. Our plan is therefore to apply our new boundary conditions by copying the first active zone (with index `iref`) to the ghost zones, except for $v_\phi$ (`VX2`), for which we want to impose a Keplerian profile.

Once done, you have to recompile and re-run the code. You should see a slight improvement of the solution at the radial boundaries.

## Restarting

As any code, idefix can restart a simulation that have been saved to disk. This is done using the .dmp files (called a dump file), a format specific to Idefix, that contains all of the variables at the precision required during the code configuration. These dump files do not depend on the architecture, you can restart with your favourite GPU a run that started on a GPU. Similarly, you can change the domain decomposition and/or enable/disable MPI altogether. You can also remove/add some physics at restart. The only thing fixed is the total domain resolution and extent: idefix doesn't interpolate from a dump.







