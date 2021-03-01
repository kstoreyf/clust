import os
import argparse
import numpy as np

from Corrfunc.theory.DD import DD
from Corrfunc.theory.xi import xi
from halotools.mock_observables import s_mu_tpcf, tpcf_multipole


def run_aemulus(filename, rmin, rmax, nbins, savename, cosmofn, cosmoid, statistic):

    # Parameters
    # should these be passed in?
    L = 1050.0 # Mpc/h
    redshift = 0.55
    nthreads = 1

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

    if statistic=='xi':
        xi0(x, y, z, L, rmin, rmax, nbins, savename, nthreads=nthreads)
    elif statistic=='xi2':
        xi2(x, y, z, L, rmin, rmax, nbins, savename, nthreads=nthreads)
    else:
        raise ValueError(f"Statistic {statistic} not recognized! Must be 'xi' or 'xi2'")
    

def run_minerva(filename, rmin, rmax, nbins, savename, statistic, nthreads=24):
    L = 1500.0 # Mpc/h
    redshift = 0.57

    print("Loading data")
    x, y, z, vx, vy, vz = np.loadtxt(filename, usecols=range(6), unpack=True)

    Omega_m = 0.285
    w = -1 #??

    E = np.sqrt(Omega_m*(1+redshift)**3 + 
                (1-Omega_m)*(1+redshift)**(3*(1+w)))
    z = [real_to_zspace(z[i], vz[i], redshift, E, L) for i in range(len(z))]

    if statistic=='xi':
        xi0(x, y, z, L, rmin, rmax, nbins, savename, nthreads=nthreads)
    elif statistic=='xi2':
        xi2(x, y, z, L, rmin, rmax, nbins, savename, nthreads=nthreads)
    else:
        raise ValueError(f"Statistic {statistic} not recognized! Must be 'xi' or 'xi2'")
    


def xi0(x, y, z, L, rmin, rmax, nbins, savename, nthreads=1):
    print("Computing xi_0")
    #LOG
    rbins = np.logspace(np.log10(rmin), np.log10(rmax), nbins + 1) # note the + 1 to nbins
    r_avg = 10 ** (0.5 * (np.log10(rbins)[1:] + np.log10(rbins)[:-1]))
    #LINEAR
    #rbins = np.linspace(rmin, rmax, nbins + 1) # note the + 1 to nbins
    #r_avg = 0.5*(rbins[1:] + rbins[:-1])

    res = xi(L, nthreads, rbins, x, y, z)

    print("Saving")
    os.makedirs(os.path.dirname(savename), exist_ok=True)
    xi_vals = res['xi']
    
    print(xi_vals)
    results = np.array([r_avg, xi_vals])
    np.savetxt(savename, results.T, delimiter=',', fmt=['%f', '%e'])

# uses halotools, as in this example: 
# https://halotools.readthedocs.io/en/latest/api/halotools.mock_observables.tpcf_multipole.html
def xi_rsd(x, y, z, L, smin, smax, nsbins, savename, nthreads=1, order=1):
    print("Computing xi_2")
    #LOG
    sbins = np.logspace(np.log10(smin), np.log10(smax), nsbins + 1) # note the + 1 to nbins
    s_avg = 10 ** (0.5 * (np.log10(sbins)[1:] + np.log10(sbins)[:-1]))
    #LINEAR
    #sbins = np.linspace(smin, smax, sbins + 1) # note the + 1 to nbins
    #s_avg = 0.5*(sbins[1:] + sbins[:-1])

    sample = np.vstack((x,y,z)).T
    nmubins = 15
    mu_bins = np.linspace(0, 1, nmubins)
    xi_s_mu = s_mu_tpcf(sample, sbins, mu_bins, period=L)
    xi_2 = tpcf_multipole(xi_s_mu, mu_bins, order=order)

    print("Saving")
    os.makedirs(os.path.dirname(savename), exist_ok=True)
    
    print(xi_2)
    results = np.array([s_avg, xi_2])
    np.savetxt(savename, results.T, delimiter=',', fmt=['%f', '%e'])


def real_to_zspace(pos, vel, redshift, E, L):
    pos_zspace = pos+vel*(1+redshift)/(E*100)
    if (pos_zspace < 0):
        pos_zspace += L
    if (pos_zspace >= L):
        pos_zspace -= L
    return pos_zspace


if __name__=='__main__':
    
    parser = argparse.ArgumentParser(description='Compute xi or xi2')
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
    parser.add_argument('statistic', type=str, help='mcf or wxi')
    args = parser.parse_args()

    run_aemulus(args.filename, args.rmin, args.rmax, args.nbins,
            args.savename, args.cosmofn, args.cosmoid, args.statistic)
