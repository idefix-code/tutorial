# import standard tools and Idefix VTK read routines
import os
import sys
sys.path.append(os.getenv("IDEFIX_DIR"))
from pytools.vtk_io import readVTK
import matplotlib.pyplot as plt
import numpy as np

n = int(sys.argv[1])

V=readVTK("data.%04d.vtk"%n)

for field in V.data.keys():
  plt.figure(figsize=(10,4))
  plt.pcolormesh(V.x,V.y,V.data[field][:,:,0].T)
  plt.title(field+ " @ t=%f"%V.t)
  plt.colorbar()
  plt.gca().set_aspect('equal')
  
# compute vorticity
wz = np.gradient(V.data['VX2'],V.x,axis=0)-np.gradient(V.data['VX1'],V.y,axis=1)

plt.figure(figsize=(10,4))
plt.pcolormesh(V.x,V.y,wz[:,:,0].T)
plt.title(r"$\omega_z$ @ t=%f"%V.t)
plt.colorbar()
plt.gca().set_aspect('equal')

plt.show()


