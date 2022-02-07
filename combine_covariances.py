import numpy as np
import os

cov_dir = '/home/users/ksf293/clust/covariances'

statistics = ['wp', 'xi', 'upf', 'mcf']
stat_str = '_'.join(statistics)

cov_glam_fn = f'{cov_dir}/cov_glam_{stat_str}.dat'
cov_aemulus_fn = f'{cov_dir}/cov_aemulus_{stat_str}_hod3_test0.dat'
cov_emuperf_fn = f'{cov_dir}/cov_emuperf_{stat_str}_nonolap_hod3_test0_mean_test0.dat'
cov_combined_fn = f"{cov_dir}/cov_combined_glam_{stat_str}.dat"

# load covs
cov_glam = np.loadtxt(cov_glam_fn)
cov_aemulus = np.loadtxt(cov_aemulus_fn)
cov_emuperf = np.loadtxt(cov_emuperf_fn)

# rescale covariances 
L_glam = 1000.
L_aemulus = 1050.
cov_glam_scaled = cov_glam*(1/5)*(L_glam/L_aemulus)**3
cov_aemulus_5box = cov_aemulus*(1/5)

# combine
cov_emu = cov_emuperf - cov_glam_scaled
cov_combined = cov_emu + cov_aemulus_5box

# save
np.savetxt(cov_combined_fn, cov_combined)


