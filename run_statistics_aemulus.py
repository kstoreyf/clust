import os
import argparse
import numpy as np

import clustering_statistics as cs
import utils


def run_statistics(fn_mock, L, cosmofn, cosmoid, redshift, statistic, fn_save, r_min, r_max, n_bins, fn_marks=None, nthreads=1):
    
    print("Loading data")
    x, y, z, _, _, vz = np.loadtxt(fn_mock, usecols=range(6), unpack=True)

    print("Get cosmology values")
    Omega_ms, ws = np.loadtxt(cosmofn, usecols=[0, 6], unpack=True)
    Omega_m = Omega_ms[cosmoid]
    w = ws[cosmoid]

    print("Converting to redshift space along z-axis")
    z = utils.real_to_zspace(z, vz, L, redshift, Omega_m, w)

    if statistic=='wp':
        cs.compute_wprp(x, y, z, L, r_min, r_max, n_bins, fn_save)
    elif statistic=='xi':
        cs.compute_xi0(x, y, z, L, r_min, r_max, n_bins, fn_save)
    elif statistic=='xi2':
        cs.compute_xi2(x, y, z, L, r_min, r_max, n_bins, fn_save)
    elif statistic=='mcf':
        assert fn_marks is not None, "Must pass marks file for mcf!"
        assert os.path.isfile(fn_marks), f"File {fn_marks} not found!"
        # Load in marks; _ column is indices in galaxy file
        marks, _ = np.loadtxt(fn_marks, delimiter=',', unpack=True) 
        cs.compute_mcf(x, y, z, marks, L, r_min, r_max, n_bins, fn_save)
    else:
        print(f"Statistic {statistic} not recognized! Use one of ['wp', 'xi', 'xi2', 'mcf']")


if __name__=='__main__':
    
    parser = argparse.ArgumentParser(description='Compute w_p(r_p)')
    parser.add_argument('fn_mock', type=str, 
        help='name of mock catalog file')
    parser.add_argument('L', type=float, help='Box length (Mpc/h)')
    parser.add_argument('cosmofn', type=str,
        help='Name of file with cosmology info for Aemulus') #optional
    parser.add_argument('cosmoid', type=int,
        help='ID of cosmology for Aemulus') #optional
    parser.add_argument('redshift', type=float, help='Value of catalog redshift')
    parser.add_argument('statistic', type=str, help='Name of statistic')
    parser.add_argument('fn_save', type=str, help='Filename for saving results')
    parser.add_argument('r_min', type=float, help='Minimum r bin')
    parser.add_argument('r_max', type=float, help='Maximum r bin')
    parser.add_argument('n_bins', type=int, help='Number of r bins')
    parser.add_argument('-fn_marks', type=str, dest='fn_marks',
        help='name of file containing marks for mcf') #optional
    args = parser.parse_args()

    run_statistics(args.fn_mock, args.L, 
                   args.cosmofn, args.cosmoid, args.redshift, 
                   args.statistic, args.fn_save, 
                   args.r_min, args.r_max, args.n_bins, fn_marks=args.fn_marks,
                   )
