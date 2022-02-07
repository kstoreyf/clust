import numpy as np
import os


#statistic = 'wp'
#statistic = 'upf'
#statistic = 'mcf'
statistic = 'xi2'

#testtag = '_fstar8.0_p1.0'
testtag = ''
meantag = '_test0'

nbins = 9
nhods = 100
testids = range(1)
boxids = range(5)

res_dir = '../../clust/results_{}/'.format(statistic)
testing_dir = '{}testing_{}{}/'.format(res_dir, statistic, testtag)
testmean_dir = '{}testing_{}{}_mean{}/'.format(res_dir, statistic, testtag, meantag)
os.makedirs(testmean_dir, exist_ok=True)

CC_test = range(0, 7)
#CC_test = range(0, 1)
#HH_test = range(1, 2)
HH_test = range(0, nhods)

for CID_test in CC_test:
    for HID_test in HH_test:

        print('CID, HID:', CID_test, HID_test)
        vals_avg = np.zeros(nbins)

        for boxid in boxids:
            for testid in testids:

                idtag = "cosmo_{}_Box_{}_HOD_{}_test_{}".format(CID_test, boxid, HID_test, testid)
                rad, vals_test = np.loadtxt(testing_dir + "{}_{}.dat".format(statistic, idtag),
                                       delimiter=',', unpack=True)

                vals_avg += vals_test

        vals_avg /= len(boxids)*len(testids)

        np.savetxt(testmean_dir + "{}_cosmo_{}_HOD_{}_mean.dat".format(statistic, CID_test, HID_test),
                   [rad, vals_avg])
