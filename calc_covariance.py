import numpy as np
import glob 


def main():
    calc_cov(['wp', 'xi', 'upf', 'mcf'], 'glam', 986)

def calc_cov(statistics, mock_tag, n_mocks):

    n_mock_min, n_mock_max = 0, n_mocks
    if mock_tag=='minerva':
        n_mock_min += 1
        n_mock_max += 1

    arrs = []
    for n in range(n_mock_min, n_mock_max):
        arr_comb = []
        for i, stat in enumerate(statistics):
            res_dir = f'results_{mock_tag}/results_{mock_tag}_{stat}'
            fn = f'{res_dir}/{stat}_{mock_tag}_n{n}.dat'
            r, arr = np.loadtxt(fn, unpack=True, delimiter=',')
            arr_comb.extend(arr)
        arrs.append(arr_comb)

    cov = covariance(arrs, fractional=True)
    
    stat_str = '_'.join(statistics)
    np.savetxt(f'covariances/cov_{mock_tag}_{stat_str}.dat', cov)

    #corrmat = reduced_covariance(covmat)
    #np.savetxt(f'results_minerva/covmat_minerva_{stat}.dat', covmat)
    #np.savetxt(f'results_minerva/corrmat_minerva_{stat}.dat', corrmat)
    
    #covmat_np = np.cov(np.array(arrs).T)
    #np.savetxt(f'results_minerva/covmatnp_minerva_{stat}.dat', covmat_np)
    #var = np.std(arrs, axis=0)**2
    #np.savetxt(f'results_minerva/var_minerva_{stat}.dat', var)


def covariance(arrs, fractional=False, zeromean=False):
    arrs = np.array(arrs)
    N = arrs.shape[0]

    assert not (fractional and zeromean), "Can't have both fractional and zero mean!"
    mean = arrs.mean(0)

    if zeromean:
        w = arrs
    if fractional:
        w = (arrs - mean)/mean
    else:
        w = arrs - mean

    outers = np.array([np.outer(w[n], w[n]) for n in range(N)])
    covsum = np.sum(outers, axis=0)
    cov = 1.0/float(N-1.0) * covsum
    return cov


def covariance_elements(arrs):
    arrs = np.array(arrs)
    N = arrs.shape[0]
    Nb = arrs.shape[1]
    means = np.mean(arrs, axis=0)
    fac = 1.0/(float(N)-1.0)
    covmat = np.zeros((Nb, Nb))
    for i in range(Nb):
        for j in range(Nb):
            covsum = 0
            for n in range(N):
                covsum += (arrs[n][i]-means[i])*(arrs[n][j]-means[j])
            covmat[i][j] = fac * covsum
    return covmat


# aka Correlation Matrix
def reduced_covariance(covmat):
    covmat = np.array(covmat)
    Nb = covmat.shape[0]
    reduced = np.zeros_like(covmat)
    for i in range(Nb):
        ci = covmat[i][i]
        for j in range(Nb):
            cj = covmat[j][j]
            reduced[i][j] = covmat[i][j]/np.sqrt(ci*cj)
    return reduced


# The prefactor unbiases the inverse; see e.g. Pearson 2016
def inverse_covariance(covmat, N):
    inv = np.linalg.inv(covmat)
    Nb = covmat.shape[0]
    prefac = float(N - Nb - 2)/float(N - 1)
    return prefac * inv


if __name__=='__main__':
    main()
