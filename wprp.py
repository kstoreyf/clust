import os
import argparse
import numpy as np

from Corrfunc.theory.wp import wp



def run_aemulus(filename, rmin, rmax, nbins, savename, cosmofn, cosmoid):

    # Parameters
    # should these be passed in?
    L = 1050.0 # Mpc/h
    redshift = 0.55
    nthreads = 1
    pimax = 40.0

    print("Loading in mock data")
    x, y, z, vx, vy, vz = np.loadtxt(filename, usecols=range(6), unpack=True)

    # Change to redshift space
    print("Computing redshift space positions")
    Omega_ms, ws = np.loadtxt(cosmofn, usecols=[0, 6], unpack=True)
    Omega_m = Omega_ms[cosmoid]
    w = ws[cosmoid]
    print(f"Omega_m: {Omega_m}, w: {w}")
    E = np.sqrt(Omega_m*(1+redshift)**3 + 
                (1-Omega_m)*(1+redshift)**(3*(1+w)))
    z = [real_to_zspace(z[i], vz[i], redshift, E, L) for i in range(len(z))]

    compute_wprp(x, y, z, L, pimax, rmin, rmax, nbins, savename)


def run_minerva(filename, rmin, rmax, nbins, savename, nthreads=24):
    L = 1500.0 # Mpc/h
    redshift = 0.57
    pimax = 40.0

    print("Loading data")
    x, y, z, vx, vy, vz = np.loadtxt(filename, usecols=range(6), unpack=True)

    Omega_m = 0.285
    w = -1 #??

    E = np.sqrt(Omega_m*(1+redshift)**3 + 
                (1-Omega_m)*(1+redshift)**(3*(1+w)))
    z = [real_to_zspace(z[i], vz[i], redshift, E, L) for i in range(len(z))]

    compute_wprp(x, y, z, L, pimax, rmin, rmax, nbins, savename, nthreads=nthreads)



def compute_wprp(x, y, z, L, pimax, rmin, rmax, nbins, savename, nthreads=1):
    # Compute wp(rp)
    print("Computing wp(rp)")
    rbins = np.logspace(np.log10(rmin), np.log10(rmax), nbins + 1) # Note the + 1 to nbins
    r_logavg = 10 ** (0.5 * (np.log10(rbins)[1:] + np.log10(rbins)[:-1]))

    res = wp(L, pimax, nthreads, rbins, x, y, z)

    print("Saving")
    os.makedirs(os.path.dirname(savename), exist_ok=True)
    wp_vals = res['wp']
    results = np.array([r_logavg, wp_vals])
    np.savetxt(savename, results.T, delimiter=',', fmt=['%f', '%e'])



def real_to_zspace(pos, vel, redshift, E, L):
    pos_zspace = pos+vel*(1+redshift)/(E*100)
    if (pos_zspace < 0):
        pos_zspace += L
    if (pos_zspace >= L):
        pos_zspace -= L
    return pos_zspace

if __name__=='__main__':
    
    parser = argparse.ArgumentParser(description='Compute w_p(r_p)')
    parser.add_argument('filename', type=str, 
        help='name of mock catalog file')
    parser.add_argument('rmin', type=float, help='minimum r bin')
    parser.add_argument('rmax', type=float, help='maximum r bin')
    parser.add_argument('nbins', type=int, help='number of r bins')
    parser.add_argument('savename', type=str, help='save filename')
    parser.add_argument('cosmofn', type=str, 
        help='name of file with cosmology info')
    parser.add_argument('cosmoid', type=int, 
        help='id of cosmology')
    args = parser.parse_args()

    run_aemulus(args.filename, args.rmin, args.rmax, args.nbins,
            args.savename, args.cosmofn, args.cosmoid)
