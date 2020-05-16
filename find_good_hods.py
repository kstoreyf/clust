import numpy as np

hodfn = '/mount/sirocco2/zz681/emulator/CMASSLOWZ/test_galaxy_mocks_wp_RSD/test_galaxy_mocks/HOD_test_np11_n1000.dat'

hoddata = np.loadtxt(hodfn)
print hoddata
print hoddata.shape

count = 0
good = []
#for i in range(len(hoddata)):
for i in range(100):
  f_env = hoddata[i][8]
  if -0.3<=f_env<=0.3:
    good.append(i)
    count += 1

print good
print count
