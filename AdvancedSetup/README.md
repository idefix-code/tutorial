# A more advanced setup

In this tutorial, we are going to set up a simple planet-disk interraction problem. This will be the opportunity to introduce several important aspects hidden in the [Getting Started](../GettingStarted/README.md) tutorial: Host and Device memory space, the `idefix_loop` construct and the tricks associated with it.

## Before we start

This tutorial is not intended to duplicate Idefix documentation. It is strongly recommended to read the introduction in the programming guide regarding [Host and device](https://idefix.readthedocs.io/latest/programmingguide.html#host-and-device), [Arrays](https://idefix.readthedocs.io/latest/programmingguide.html#arrays) and [Loops](https://idefix.readthedocs.io/latest/programmingguide.html#execution-space-and-loops)

## The problem

The setup provided in problem 1 is a simple 2D Keplerian disk in polar coordinates ($x_1=R,x_2=\phi$). The initial conditions are already writte defines a surface density profile $\Sigma=100 R^{-1}$ (the code does not include units). We have also defined the disk aspect ratio $h_0\equiv H/R=c_s/V_k$, which is read by the Setup constructor from idefix.ini. Here, $V_k$ is the Keplerian velocity, which reads with our units (central Mass=1), $V_K=1/\sqrt{R}$.

### Define the sound speed

We assume the flow is locally isothermal, meaning that we assume the temperature (and therefore the sound speed, since $T\propto c_s^2$) at each radius $R$ is fixed. As you can see in `idefix.ini`, in the [Hydro] block, we have said that the sound speed is user-defined. This is because we want to tell idefix explicitely which function it should use for the sound speed. To define this sound speed, we are going to assume that the disk aspect ratio is constant, so that $c_s=h_0 V_K=h_0/\sqrt{R}$. 

For this we have already started to write a function `MySoundSpeed` in `setup.cpp`, in which we have already gathered the radial coordinate array ($x_1$) and the aspect ratio ($h_0$). The goal of this function is to fill an idefixArray `cs` (that appears as a parameter of this function) that idefix will use to get the sound speed at each point.

Your first task is to invoke an `idefix_for` (read the [doc](https://idefix.readthedocs.io/latest/programmingguide.html#execution-space-and-loops)!). This idefix for should cover the entire domain (or sub-domain if using MPI) of simulation.
The domain extends from $0$ to `data->np_tot[IDIR]` in the x1 direction, $0$ to `data->np_tot[JDIR]` in the x2 direction, and `data->np_tot[KDIR]` in the x3 direction (see the [dataBlock documentation](https://idefix.readthedocs.io/latest/programmingguide.html#execution-space-and-loops)). Note that in idefix, it is cusommary to use first the `k` index running on `x3`, then `j` running on `x2` etc. This is to ensure that the fastest running index is always `i` and is also the last  index of every array for optimal performance.