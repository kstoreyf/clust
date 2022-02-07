import numpy as np
import scipy
import scipy.spatial
from scipy import interpolate
from scipy.stats import poisson, erlang
import gc
import os


# Code snippets from Arka Banerjee

def create_bins(knn_orders, bins_per_k, nbar=2.0e-4, 
                epsilon = 5e-3, upper_stretch=1.25, 
                lower_stretch=1.25):
    bins = np.zeros((len(knn_orders),bins_per_k))
    for i in range(bins.shape[0]):
        lv = erlang.ppf(epsilon, knn_orders[i]) / nbar
        ls = (lv*3/(4*np.pi))**(1./3.)/lower_stretch
        
        uv = erlang.ppf(1.0 - epsilon, knn_orders[i])/nbar
        us = (uv*3/(4*np.pi))**(1./3.)*upper_stretch
        
        bins[i, :] = np.logspace(np.log10(ls), np.log10(us), 
                                 bins_per_k)
        
    return bins


def compute_cdf(distances):
    '''
    Computes an interpolating function to evaluate CDF 
    at a given radius.
    
    Parameters
    ----------
    
    distances: float[:,:]
        List of nearest neighbor distances for each kNN.
        distances.shape[1] should be # of kNN
    
    Returns
    -------
    
    cdf: scipy interpolating function for each kNN
    '''
    
    cdf = []
    n = distances.shape[0]
    l = distances.shape[1]
    gof = ((np.arange(0, n) + 1) / (n*1.0))
    for c in range(l):
        ind = np.argsort(distances[:, c])
        sorted_distances = distances[ind, c]
        cdf.append(interpolate.interp1d(sorted_distances, gof, kind='linear', 
                                        bounds_error=False))
    return cdf


def generate_knn_cdfs(pos, knn_orders, n_query, bins, boxsize=1050):
    '''
    Computes the CDF of nn distances of 
    data points from a set of space-filling
    randoms.
    
    Currently set for periodic boundary 
    conditions
    
    Parameters
    ----------
    
    pos: float[:,:]
        Positions of particles (data)
    knn_orders: int list
        List of k nearest neighbor distances
        that need to be computed
    n_query: int
        Number of query to be used 
        for the calculation
    boxsize: float
        Size of the simulation box
    bins: float[:, :]
        Bin centers for each kNN
        
    Returns
    -------
    
    data: float[:,:]
        kNN CDFs at the  bin centers
    '''
    print('generate_knn_cdfs')
    print(pos.shape)
    print(boxsize)
    xtree = scipy.spatial.cKDTree(pos, boxsize=boxsize)

    #Generate query points in the same volume
    #Here we use randomly distributed query points. Can also use a grid
    query_pos = np.random.rand(n_query, 3)*boxsize

    distances, indices = xtree.query(query_pos, k=knn_orders,
                            n_jobs=-1)
    del(indices)
    gc.collect()
    
    #Now get the CDF
    data = (np.zeros((len(knn_orders), bins.shape[1])))
    cdfs = compute_cdf(distances)
    for i in range(len(knn_orders)):
        data[i,: ]= cdfs[i](bins[i, :])
        
    return data

# for now, knn_orders is a single number
# TODO: rearrange to take array, compute multiple at once
def compute_knn_cdf(x, y, z, L, n_bins_per_k, knn_order_max, fn_save,
                    n_query=400**3, target_nbar=2.0e-4):
    knn_orders = np.arange(1, knn_order_max+1)
    print('orders:',knn_orders)

    pos = np.array([x,y,z]).T
    #Now downsample randomly to target density
    print('Number:', len(pos), ' Target number:', int(target_nbar * L**3))
    idxs_pos = np.arange(len(pos))
    n_target = int(target_nbar * L**3)
    idxs_pos_target = np.random.choice(idxs_pos, size=n_target, replace=False)
    pos_target = pos[idxs_pos_target]
    bins = create_bins(knn_orders, n_bins_per_k)
    print('Bins:',bins)
    knn_cdfs = generate_knn_cdfs(pos_target, knn_orders, n_query, bins, boxsize=L)
    
    print("Saving")
    # Make results directory if doesn't exist
    os.makedirs(os.path.dirname(fn_save), exist_ok=True)
    for i in range(len(knn_orders)):
        fn_save_order = add_knn_order_to_name(fn_save, knn_orders[i])
        results = np.array([bins[i,:], knn_cdfs[i,:]])
        print(results.shape)
        print(fn_save_order)
        np.savetxt(fn_save_order, results.T, delimiter=',', fmt=['%f', '%e'])


def add_knn_order_to_name(fn_save, knn_order):
    dir, name = os.path.split(fn_save) # get file name, without path
    name_split = name.split('knn')
    assert len(name_split)==2, "knn should only be in name once!"
    name_order = name_split[0] + 'knn' + str(knn_order) + name_split[1]
    return os.path.join(dir, name_order)

### Usage:

# knn_orders = [1,2,3]
# boxsize=1050 #Mpc/h
# nhalos = 300000 #This is a dummy variable, just to illustrate the downsampling step below
# bins_per_k = 9
# n_query= 400**3
# target_nbar = 2.1e-4

# #Input set of galaxy positions. Using randoms as an example
# pos = np.random.rand(nhalos,3)*boxsize
# #Now downsample randomly to target density
# idx = np.random.choice(len(pos), int(target_nbar*boxsize**3),
#                        replace=False)
# bins = create_bins(knn_orders, bins_per_k)
# knn_cdfs = generate_knn_cdfs(pos, knn_orders, n_query, bins, boxsize=1050)
