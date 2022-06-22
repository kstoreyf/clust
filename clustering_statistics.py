import os
import numpy as np

from Corrfunc.theory.wp import wp
from Corrfunc.theory.xi import xi
from Corrfunc.theory.DD import DD
from halotools.mock_observables import s_mu_tpcf, tpcf_multipole


def compute_wprp(x, y, z, L, r_min, r_max, n_bins, fn_save, pi_max=40.0, nthreads=1):
    # Compute wp(rp)
    print("Computing wp(rp)")

    # Set up bins (log)
    r_bins = np.logspace(np.log10(r_min), np.log10(r_max), n_bins + 1) # Note the + 1 to nbins
    r_avg = 10 ** (0.5 * (np.log10(r_bins)[1:] + np.log10(r_bins)[:-1]))

    res = wp(L, pi_max, nthreads, r_bins, x, y, z)
    wp_vals = res['wp']

    print("Saving")
    # Make results directory if doesn't exist
    os.makedirs(os.path.dirname(fn_save), exist_ok=True)
    results = np.array([r_avg, wp_vals])
    np.savetxt(fn_save, results.T, delimiter=',', fmt=['%f', '%e'])
    return r_avg, wp_vals


def compute_xi0(x, y, z, L, r_min, r_max, n_bins, fn_save, nthreads=1):
    print("Computing xi_0")
    # Set up bins (log)
    r_bins = np.logspace(np.log10(r_min), np.log10(r_max), n_bins + 1) # Note the + 1 to nbins
    r_avg = 10 ** (0.5 * (np.log10(r_bins)[1:] + np.log10(r_bins)[:-1]))

    res = xi(L, nthreads, r_bins, x, y, z)
    xi_vals = res['xi']

    print("Saving")
    os.makedirs(os.path.dirname(fn_save), exist_ok=True)
    results = np.array([r_avg, xi_vals])
    np.savetxt(fn_save, results.T, delimiter=',', fmt=['%f', '%e'])


def compute_xi2(x, y, z, L, r_min, r_max, n_bins, fn_save, 
        nmubins=15, nthreads=1):
    print("Computing xi_2")
    # Set up bins (log)
    r_bins = np.logspace(np.log10(r_min), np.log10(r_max), n_bins + 1) # note the + 1 to nbins
    r_avg = 10 ** (0.5 * (np.log10(r_bins)[1:] + np.log10(r_bins)[:-1]))

    # Use halotools to compute the quadrupole, as in this example: 
    # https://halotools.readthedocs.io/en/latest/api/halotools.mock_observables.tpcf_multipole.html
    sample = np.vstack((x,y,z)).T
    print(sample.shape)
    mu_bins = np.linspace(0, 1, nmubins)
    xi_s_mu = s_mu_tpcf(sample, r_bins, mu_bins, period=L)
    xi_2 = tpcf_multipole(xi_s_mu, mu_bins, order=2) # Order 2 is quadrupole
    print(xi_s_mu)
    print(xi_2)
    print(mu_bins)
    print(L)

    print("Saving")
    os.makedirs(os.path.dirname(fn_save), exist_ok=True)
    results = np.array([r_avg, xi_2])
    np.savetxt(fn_save, results.T, delimiter=',', fmt=['%f', '%e'])


def compute_mcf(x, y, z, marks, L, r_min, r_max, n_bins, fn_save, nthreads=1):
    print("Computing M(r)")
    # Set up bins (log)
    r_bins = np.logspace(np.log10(r_min), np.log10(r_max), n_bins + 1) # Note the + 1 to nbins
    r_avg = 10 ** (0.5 * (np.log10(r_bins)[1:] + np.log10(r_bins)[:-1]))

    autocorr=1
    res = DD(autocorr, nthreads, r_bins, x, y, z, 
            weights1=marks, periodic=True, boxsize=L, weight_type="pair_product")
    mcf_vals = res['weightavg'] 
    # Compute mcf with: mcf = prefac * pairs_weighted = prefac * npairs*weightavg, where prefac divides out npairs, so left with just weightavg  
    mcf_vals /= np.mean(marks)**2 #eq 2.1, White 2016

    print("Saving")
    os.makedirs(os.path.dirname(fn_save), exist_ok=True)
    results = np.array([r_avg, mcf_vals])
    np.savetxt(fn_save, results.T, delimiter=',', fmt=['%f', '%e'])
