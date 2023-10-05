import numpy as np
import subprocess


#mock_tag_test = '_aemulus_fmaxmocks_test_minus1'
mock_tag_test = '_aemulus_fmaxmocks_test_plus1'
if mock_tag_test=='_aemulus_test':
    mock_dir = '/mount/sirocco2/zz681/emulator/CMASSLOWZ/test_galaxy_mocks_wp_RSD/test_galaxy_mocks_new_f_env/mocks'
elif mock_tag_test=='_aemulus_Msatmocks_test':
    # updated Msat 
    mock_dir = '/mount/sirocco2/zz681/emulator/CMASSLOWZ_Msat/test_mocks/mocks'
elif mock_tag_test=='_aemulus_fmaxmocks_test':
    mock_dir = '/mount/sirocco1/zz681/emulator/CMASSLOWZ_Msat_fmax_new/test_mocks/mocks'
elif mock_tag_test=='_aemulus_fmaxmocks_test_minus':
    mock_dir = '/mount/sirocco1/zz681/emulator/CMASSLOWZ_Msat_fmax_new/test_mocks_Aemulus6_NumberDensity/mocks_minus'
elif mock_tag_test=='_aemulus_fmaxmocks_test_plus':
    mock_dir = '/mount/sirocco1/zz681/emulator/CMASSLOWZ_Msat_fmax_new/test_mocks_Aemulus6_NumberDensity/mocks_plus'
elif mock_tag_test=='_aemulus_fmaxmocks_test_minus1':
    mock_dir = '/mount/sirocco1/zz681/emulator/CMASSLOWZ_Msat_fmax_new/test_mocks_Aemulus6_NumberDensity_v2/mocks_minus'
elif mock_tag_test=='_aemulus_fmaxmocks_test_plus1':
    mock_dir = '/mount/sirocco1/zz681/emulator/CMASSLOWZ_Msat_fmax_new/test_mocks_Aemulus6_NumberDensity_v2/mocks_plus'

cosmo = 6
hod = 69
boxes = range(5)

ngals_arr = []
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

np.savetxt(f"numgals/mean_numgals{mock_tag_test}_c{cosmo}h{hod}.dat", [np.mean(ngals_arr)])
