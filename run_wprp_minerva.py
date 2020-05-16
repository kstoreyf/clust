import os

import wprp


nmocks = 100
rmin = 0.1
rmax = 50
nbins = 9

save_dir = 'results_minerva/results_minerva_wp'
mock_dir = '/mount/sirocco1/zz681/emulator/Minerva'

for n in range(1, nmocks+1):
    print("Minerva", n)
    mockstr = str(n).zfill(3)
    filename = '{}/Galaxies_HOD_{}_z0.57.dat'.format(mock_dir, mockstr)
    savename = '{}/wp_minerva_n{}.dat'.format(save_dir, n)
    if os.path.isfile(savename):
        print(f"wp for minerva mock {n} already exists!")
        continue
    wprp.run_minerva(filename, rmin, rmax, nbins, savename)
    