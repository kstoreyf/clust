import numpy as np
import os


statistics = ['wp', 'xi', 'upf', 'mcf', 'xi2']
cov_tag = 'emuperf'
tag_str = '_nonolap_hod3_test0_mean_test0'

cov_dir = '/home/users/ksf293/clust/covariances'
stat_str = '_'.join(statistics)
cov_fn = f"{cov_dir}/cov_{cov_tag}_{stat_str}{tag_str}.dat"
cov_diag_fn = f"{cov_dir}/cov_diag_{cov_tag}_{stat_str}{tag_str}.dat"

if os.path.exists(cov_fn):
    cov = np.loadtxt(cov_fn)
else:
    raise ValueError(f"Path to covmat {cov_fn} doesn't exist!")
print(cov_diag_fn)
# Takes the diagonal elements of cov, then puts them back
# into a diagonal covariance matrix with all other elements zero
cov_diag = np.diag(np.diag(cov))

np.savetxt(cov_diag_fn, cov_diag)


