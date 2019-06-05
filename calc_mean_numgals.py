import numpy as np
import subprocess

#training
#mock_dir = "/mount/sirocco1/zz681/emulator/CMASS_BIAS/COS_2000HOD/galaxy_mock/"
#new training
mock_dir = "/mount/sirocco2/zz681/emulator/CMASSLOWZ/galaxy_mocks/mocks/"

#testing
#mock_dir = "/mount/sirocco1/zz681/emulator/CMASS_BIAS/GP_Test_BOX/test_galaxy_mock/"
#new testing
#mock_dir = "/mount/sirocco2/zz681/emulator/CMASSLOWZ/test_galaxy_mocks_wp_RSD/test_galaxy_mocks/mocks/"

tag = '_hod100_newcheck'

cosmos = range(40)
hods = range(100)
#cosmos = range(7)
#hods = [0, 6, 10, 11]
#hods = [0, 6, 10, 11, 14, 16, 19, 20, 23, 24, 25, 26, 27, 28, 29, 32, 33, 38, 41, 43, 44, 47, 50, 52, 57, 58, 59, 60, 64, 68, 71, 72, 74, 75, 77, 81, 82, 83, 84, 85, 87, 91, 97, 99] #good hods
#boxes = range(5)
boxes = range(1)

ngals_arr = []
for cosmo in cosmos:
  print cosmo
  for hod in hods:

    for box in boxes:
      #boxtag = "Box_{}_".format(box)
      boxtag = ''
      mocktag = 'mock_'
      fn = mock_dir + '{}cosmo_{}_{}HOD_{}_test_0.mock'.format(mocktag, cosmo, boxtag, hod)
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
