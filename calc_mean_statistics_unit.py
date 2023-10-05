import numpy as np
import subprocess


mocknametags=["_fixedAmp_001_", "_fixedAmp_002_", "_fixedAmp_InvPhase_001_", "_fixedAmp_InvPhase_002_"]
mock_tag="_unit"

nbins = 9

statistics = ['wp', 'xi', 'xi2', 'upf', 'mcf']
for statistic in statistics:
    result_dir = f'/home/users/ksf293/clust/results{mock_tag}/results_{statistic}'

    vals_avg = np.zeros(nbins)
    for mocknametag in mocknametags:
        fn_stat = f"{result_dir}/{statistic}{mock_tag}{mocknametag}.dat"
        rad, vals = np.loadtxt(fn_stat, delimiter=',', unpack=True)
        vals_avg += vals

    vals_avg /= len(mocknametags)

    save_fn = f'{result_dir}/{statistic}{mock_tag}.dat'
    results = np.array([rad, vals_avg])
    print(f"Saving mean statistic to {save_fn}")
    np.savetxt(save_fn, results.T, delimiter=',', fmt=['%f', '%e'])

