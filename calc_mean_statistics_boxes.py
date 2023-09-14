import numpy as np
import os


statistics = ['wp', 'xi', 'xi2', 'mcf', 'upf']
#statistics = ['mcf', 'upf']

#mock_tag = '_aemulus_fmaxmocks_test'
mock_tag = '_aemulus_fmaxmocks_test_plus'
testtag = ''

nbins = 9
nhods = 100
testids = range(1)
boxids = range(5)

if mock_tag=='_aemulus_fmaxmocks_test':
    CC_test = range(0, 7)
    HH_test = range(0, nhods)
elif mock_tag=='_aemulus_fmaxmocks_test_minus' or mock_tag=='_aemulus_fmaxmocks_test_plus':
    CC_test = [1]
    HH_test = [19]

for statistic in statistics:
    print("stat:", statistic)
    testing_dir = f'/mount/sirocco1/ksf293/clust/results{mock_tag}/results_{statistic}'
    testmean_dir = f'/mount/sirocco1/ksf293/clust/results{mock_tag}_mean/results_{statistic}'
    os.makedirs(testmean_dir, exist_ok=True)

    for CID_test in CC_test:
        for HID_test in HH_test:

            print('CID, HID:', CID_test, HID_test)
            vals_avg = np.zeros(nbins)

            for boxid in boxids:
                for testid in testids:

                    rad, vals_test = np.loadtxt(f'{testing_dir}/{statistic}_cosmo_{CID_test}_Box_{boxid}_HOD_{HID_test}_test_{testid}.dat',
                                        delimiter=',', unpack=True)

                    vals_avg += vals_test

            vals_avg /= len(boxids)*len(testids)

            save_fn = f'{testmean_dir}/{statistic}_cosmo_{CID_test}_HOD_{HID_test}_mean.dat'
            results = np.array([rad, vals_avg])
            np.savetxt(save_fn, results.T, delimiter=',', fmt=['%f', '%e'])
