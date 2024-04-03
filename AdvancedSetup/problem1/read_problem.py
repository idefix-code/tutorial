# import standard tools and Idefix VTK read routines
import os
import sys
sys.path.append(os.getenv("IDEFIX_DIR"))
from pytools.vtk_io import readVTK
import matplotlib.pyplot as plt
import numpy as np

n = int(sys.argv[1])

V=readVTK("data.%04d.vtk"%n)

x=V.xl[:,None]*np.cos(V.yl[None,:])
y=V.xl[:,None]*np.sin(V.yl[None,:])

for field in V.data.keys():
  plt.figure()
  plt.pcolormesh(x,y,V.data[field][:,:,0])
  plt.title(field+ " @ t=%f"%V.t)
  plt.colorbar()
  plt.gca().set_aspect('equal')
  


plt.show()


