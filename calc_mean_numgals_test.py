import numpy as np
import subprocess


#new testing
mock_dir = "/mount/sirocco2/zz681/emulator/CMASSLOWZ/test_galaxy_mocks_wp_RSD/test_galaxy_mocks_new_f_env/mocks/"

tag = '_testing_hod100_test0'

cosmos = range(7)
hods = range(100)
boxes = range(5)

ngals_arr = []
for cosmo in cosmos:
  print cosmo
  for hod in hods:
    for box in boxes:
      fn = mock_dir + 'mock_cosmo_{}_Box_{}_HOD_{}_test_0.mock'.format(cosmo, box, hod)
      p = subprocess.Popen(['wc', '-l', fn], stdout=subprocess.PIPE, 
                                                      stderr=subprocess.PIPE)
      result, err = p.communicate()
      if p.returncode != 0:
        raise IOError(err)
      ngals = int(result.strip().split()[0])
      ngals_arr.append(ngals)

print np.mean(ngals_arr)
print np.std(ngals_arr)
print np.max(ngals_arr)
print np.min(ngals_arr)

np.savetxt("numgals/mean_numgals{}.dat".format(tag), [np.mean(ngals_arr)])
