import numpy as np
import subprocess


mockdir="/mount/sirocco2/zz681/emulator/CMASSLOWZ_SHAM_Joe/UNIT/1Gpc"
mocknamebase="Clustering_catalog_hlist_0.64210.list.sham"
mocknametags=["_fixedAmp_001_", "_fixedAmp_002_", "_fixedAmp_InvPhase_001_", "_fixedAmp_InvPhase_002_"]
mock_tag="_unit"

ngals_arr = []
for mocknametag in mocknametags:
    mockname=f"{mocknamebase}{mocknametag}.dat"
    fn = f'{mockdir}/{mockname}'
    p = subprocess.Popen(['wc', '-l', fn], stdout=subprocess.PIPE, 
                                                stderr=subprocess.PIPE)
    result, err = p.communicate()
    if p.returncode != 0:
        raise IOError(err)
    ngals = int(result.strip().split()[0])
    ngals_arr.append(ngals)

ngals_mean = np.mean(ngals_arr)
print(ngals_arr)
print(ngals_mean)

np.savetxt(f"numgals/mean_numgals{mock_tag}.dat", [ngals_mean])
