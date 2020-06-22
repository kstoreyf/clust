import os
import argparse
import numpy as np

from Corrfunc.theory.DD import DD



def run_aemulus(filename, rmin, rmax, nbins, savename, markfn, cosmofn, cosmoid):

    # Parameters
    # should these be passed in?
    L = 1050.0 # Mpc/h
    redshift = 0.55
    nthreads = 1

    print("Loading in mock data")
    x, y, z, vx, vy, vz = np.loadtxt(filename, usecols=range(6), unpack=True)
    marks, _ = np.loadtxt(markfn, delimiter=',', unpack=True) # LOAD IN MARKS, _ is indices

    # Change to redshift space
    print("Computing redshift space positions")
    Omega_ms, ws = np.loadtxt(cosmofn, usecols=[0, 6], unpack=True)
    Omega_m = Omega_ms[cosmoid]
    w = ws[cosmoid]
    print(f"Omega_m: {Omega_m}, w: {w}")
    E = np.sqrt(Omega_m*(1+redshift)**3 + 
                (1-Omega_m)*(1+redshift)**(3*(1+w)))
    z = [real_to_zspace(z[i], vz[i], redshift, E, L) for i in range(len(z))]

    compute_mcf(x, y, z, marks, L, rmin, rmax, nbins, savename, nthreads=nthreads)


def run_minerva(filename, rmin, rmax, nbins, savename, markfn, nthreads=24):
    L = 1500.0 # Mpc/h
    redshift = 0.57
    pimax = 40.0

    print("Loading data")
    x, y, z, vx, vy, vz = np.loadtxt(filename, usecols=range(6), unpack=True)
    marks, _ = np.loadtxt(markfn, unpack=True) # LOAD IN MARKS, _ is indices

    Omega_m = 0.285
    w = -1 #??

    E = np.sqrt(Omega_m*(1+redshift)**3 + 
                (1-Omega_m)*(1+redshift)**(3*(1+w)))
    z = [real_to_zspace(z[i], vz[i], redshift, E, L) for i in range(len(z))]

    compute_mcf(x, y, z, marks, L, rmin, rmax, nbins, savename, nthreads=nthreads)



def compute_mcf(x, y, z, marks, L, rmin, rmax, nbins, savename, nthreads=1):
    print("Computing M(r)")
    #LOG
    rbins = np.logspace(np.log10(rmin), np.log10(rmax), nbins + 1) # note the + 1 to nbins
    r_avg = 10 ** (0.5 * (np.log10(rbins)[1:] + np.log10(rbins)[:-1]))
    #LINEAR
    #rbins = np.linspace(rmin, rmax, nbins + 1) # note the + 1 to nbins
    #r_avg = 0.5*(rbins[1:] + rbins[:-1])

    autocorr=1
    res = DD(autocorr, nthreads, rbins, x, y, z, 
            weights1=marks, periodic=True, boxsize=L, weight_type="pair_product")

    print(res)
    print("Saving")
    os.makedirs(os.path.dirname(savename), exist_ok=True)
    mcf_vals = res['weightavg'] #mcf = npairs*weightavg, then divide out pairs by definition formula 
    mcf_vals /= np.mean(marks)**2 #eq 2.1, White 2016
    
    print(mcf_vals)
    results = np.array([r_avg, mcf_vals])
    np.savetxt(savename, results.T, delimiter=',', fmt=['%f', '%e'])



def real_to_zspace(pos, vel, redshift, E, L):
    pos_zspace = pos+vel*(1+redshift)/(E*100)
    if (pos_zspace < 0):
        pos_zspace += L
    if (pos_zspace >= L):
        pos_zspace -= L
    return pos_zspace


if __name__=='__main__':
    
    parser = argparse.ArgumentParser(description='Compute M(r), marked correlation function')
    parser.add_argument('filename', type=str, 
        help='name of mock catalog file')
    parser.add_argument('rmin', type=float, help='minimum r bin')
    parser.add_argument('rmax', type=float, help='maximum r bin')
    parser.add_argument('nbins', type=int, help='number of r bins')
    parser.add_argument('savename', type=str, help='save filename')
    parser.add_argument('markfn', type=str, help='name of mark file')
    parser.add_argument('cosmofn', type=str, 
        help='name of file with cosmology info')
    parser.add_argument('cosmoid', type=int, 
        help='id of cosmology')
    args = parser.parse_args()

    run_aemulus(args.filename, args.rmin, args.rmax, args.nbins,
            args.savename, args.markfn, args.cosmofn, args.cosmoid)
