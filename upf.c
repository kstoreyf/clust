#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <time.h>

#include "include/kdtree_periodic.h"
#include <utils.h>


/* Compute upf at given radius values, assuming periodic box. Units are Mpc/h */
int compute_upf(double *x, double *y, double *z, 
                int n_galaxies, double *radii, int n_radii, 
                int n_spheres, double density_threshold, 
                double L, double *results_arr){

    double px, py, pz;
    int i, j;
    unsigned int msec,start;
    void *kd, *pset;
    
    /* Set up results array (has length n_radii) */
    for(i=0;i<n_radii;i++){
        results_arr[i] = 0;
    }

    /* Build kdtree */
    kd = kd_create(3); /* 3 is number of dimensions */
	  start = get_msec();
	  for(i=0; i<n_galaxies; i++) {
		  assert(kd_insert3(kd, x[i], y[i], z[i], 0) == 0);
	  }
	  msec = get_msec() - start;
	  printf("Built tree in %.3f sec\n", (float)msec / 1000.0);

    /* Query kdtree at radii */
    /* Set random seed */
    srand(time(NULL));
    for (i=0; i<n_radii; i++){
        start = get_msec();
        double vol = 4.0/3.0*PI*pow(radii[i], 3);
        int n_underdense = 0;
        for (j=0; j<n_spheres; j++){
            /* Make random center of sphere */
            px = ((float)rand() / RAND_MAX) * L;
            py = ((float)rand() / RAND_MAX) * L;
            pz = ((float)rand() / RAND_MAX) * L;
            pset = kd_nearest_range3_periodic(
                      kd, px, py, pz, radii[i], L);
            /* If sphere is undersdense compared to threshold, count it */
            if (kd_res_size(pset)/vol <= density_threshold) {
                n_underdense += 1;
            }
        }
        results_arr[i] = (double)n_underdense / (double)n_spheres;
        msec = get_msec() - start;
        printf("Found %d/%d=%.10f underdense n_spheres at radius %f in %.3f sec\n", n_underdense, n_spheres, results_arr[i], radii[i], (float)msec / 1000.0);
    }

    /* Clean up */
    kd_res_free(pset);
	  kd_free(kd);

	  return 0;
}