import numpy as np
import subprocess


mock_tag_train = '_aemulus_fmaxmocks_train'
if mock_tag_train=='_aemulus_train':
    mock_dir = '/mount/sirocco2/zz681/emulator/CMASSLOWZ/galaxy_mocks/mocks/'
elif mock_tag_train=='_aemulus_Msatmocks_train':
    # updated Msat 
    mock_dir = '/mount/sirocco2/zz681/emulator/CMASSLOWZ_Msat/training_mocks/mocks'
elif mock_tag_train=='_aemulus_fmaxmocks_train':
    mock_dir = '/mount/sirocco1/zz681/emulator/CMASSLOWZ_Msat_fmax_new/training_mocks/mocks'

cosmos = range(40)
boxes = range(1)
nhodsnonolap = 100
nhodspercosmo = 100

ngals_arr = []
for cosmo in cosmos:
  print(cosmo)
  hods = cosmo*nhodsnonolap + np.arange(nhodspercosmo)
  for hod in hods:
      fn = f'{mock_dir}/mock_cosmo_{cosmo}_HOD_{hod}_test_0.mock'
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

np.savetxt("numgals/mean_numgals{}.dat".format(mock_tag_train), [np.mean(ngals_arr)])
