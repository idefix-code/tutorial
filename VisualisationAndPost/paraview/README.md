# Idefix paraview tutorial

This tutorial will be done in presentation mode. To reproduce what is being done
in this presentation, you will need paraview>5.8 and the idefix dataset available following [this link](https://filesender.renater.fr/?s=download&token=fead5091-7308-4252-ba3c-8d740b40695a). The pdf of the presentation is [available here](slides.pdf).

In addition, you will need the following formula for Paraview calculator that you can directly copy/paste from this document:

## Magnetic pressure

We set `Pm` to

```shell
0.5*(BX1^2+BX2^2+BX3^2)
```

## Streamlines

The formula needed to transform vectors in spherical coordinates into cartesian ones is

```shell
BX1*(coordsX*iHat+coordsY*jHat+coordsZ*kHat)/sqrt(coordsX^2+coordsY^2+coordsZ^2) + BX2*(coordsX*coordsZ*iHat+coordsY*coordsZ*jHat-(coordsX^2+coordsY^2)*kHat)/sqrt((coordsX^2+coordsY^2+coordsZ^2)*(coordsX^2+coordsY^2)) + BX3*(coordsX*jHat-coordsY*iHat)/sqrt(coordsX^2+coordsY^2)
```

