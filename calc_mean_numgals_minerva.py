import numpy as np
import subprocess


#new testing
mock_dir = "/mount/sirocco1/zz681/emulator/Minerva"

tag = '_minerva'

nmocks = 100
ngals_arr = []
for n in range(1, nmocks+1):
  mockstr = str(n).zfill(3)
  fn = '{}/Galaxies_HOD_{}_z0.57.dat'.format(mock_dir, mockstr)
  p = subprocess.Popen(['wc', '-l', fn], stdout=subprocess.PIPE, 
                                                  stderr=subprocess.PIPE)
  result, err = p.communicate()
  if p.returncode != 0:
    raise IOError(err)
  ngals = int(result.strip().split()[0])
  ngals_arr.append(ngals)

print(np.mean(ngals_arr))
print(np.std(ngals_arr))
print(np.max(ngals_arr))
print(np.min(ngals_arr))

np.savetxt("numgals/mean_numgals{}.dat".format(tag), [np.mean(ngals_arr)])
