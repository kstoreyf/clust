import numpy as np
import subprocess


mock_tag_test = '_aemulus_fmaxmocks_test'
if mock_tag_test=='_aemulus_test':
    mock_dir = '/mount/sirocco2/zz681/emulator/CMASSLOWZ/test_galaxy_mocks_wp_RSD/test_galaxy_mocks_new_f_env/mocks'
elif mock_tag_test=='_aemulus_Msatmocks_test':
    # updated Msat 
    mock_dir = '/mount/sirocco2/zz681/emulator/CMASSLOWZ_Msat/test_mocks/mocks'
elif mock_tag_test=='_aemulus_fmaxmocks_test':
    mock_dir = '/mount/sirocco1/zz681/emulator/CMASSLOWZ_Msat_fmax_new/test_mocks/mocks'


cosmos = range(7)
hods = range(100)
boxes = range(5)

ngals_arr = []
for cosmo in cosmos:
  print(cosmo)
  for hod in hods:
    for box in boxes:
      fn = f'{mock_dir}/mock_cosmo_{cosmo}_Box_{box}_HOD_{hod}_test_0.mock'
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

np.savetxt(f"numgals/mean_numgals{mock_tag_test}.dat", [np.mean(ngals_arr)])
