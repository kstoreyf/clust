import numpy as np
import subprocess

#new training
mock_dir = "/mount/sirocco2/zz681/emulator/CMASSLOWZ/galaxy_mocks/mocks/"
tag = '_training_nonolap'

cosmos = range(40)
boxes = range(1)
nhodsnonolap = 100
nhodspercosmo = 100

ngals_arr = []
for cosmo in cosmos:
  print(cosmo)
  hods = cosmo*nhodsnonolap + np.arange(nhodspercosmo)
  for hod in hods:
      fn = mock_dir + 'mock_cosmo_{}_HOD_{}_test_0.mock'.format(cosmo, hod)
      p = subprocess.Popen(['wc', '-l', fn], stdout=subprocess.PIPE, 
                                                      stderr=subprocess.PIPE)
      result, err = p.communicate()
      if p.returncode != 0:
        raise IOError(err)
      ngals = int(result.strip().split()[0])
      ngals_arr.append(ngals)

print( np.mean(ngals_arr) )
print( np.std(ngals_arr) )
print( np.max(ngals_arr) )
print( np.min(ngals_arr) )

np.savetxt("numgals/mean_numgals{}.dat".format(tag), [np.mean(ngals_arr)])
