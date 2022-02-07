import numpy as np
import subprocess


mock_dir = '/mount/sirocco2/zz681/emulator/GLAM_mock/mocks4'
tag = '_glam4'

nmocks = 986
ngals_arr = []
for nmock in range(nmocks):
  fn = f'{mock_dir}/mock_n{nmock}_test_0.mock'
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
