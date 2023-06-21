#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <time.h>

#include "kdtree/include/kdtree_periodic.h"
#include <utils.h>


int compute_marks(double *x, double *y, double *z, 
                  int n_galaxies, double radius,
                  double density_mean,
                  double factor_star, double p, 
                  double L, double *densities_arr){

    int i;
    unsigned int msec, start;
    void *kd, *pset;

    /* Build kdtree */
    kd = kd_create(3);
	start = get_msec();
	for(i=0; i<n_galaxies; i++) {
		  assert(kd_insert3(kd, x[i], y[i], z[i], 0) == 0);
	}
	msec = get_msec() - start;
	printf("Built tree in %.3f sec\n", (float)msec / 1000.0);

    /* Loop over all galaxies to get each one's local density */
    start = get_msec();
    double vol_sphere = 4.0/3.0 * PI * radius*radius*radius;
    for (i=0; i<n_galaxies; i++){
        pset = kd_nearest_range3_periodic(kd, x[i], y[i], z[i], radius, L);
        densities_arr[i] = (double) kd_res_size(pset) / vol_sphere; /*n neighbors*/
    }
    msec = get_msec() - start;
    printf("Computed marks in %.3f sec\n", (float)msec / 1000.0);

    /* compute marks using Satpathy 2019 (sdss) eqn 5 */
    /* Alternativeley can use Padmanabhan & White 2009 formula: densities[i] = densities[i]/(dens_star + densities[i]);*/ 
    double density_star = factor_star*density_mean;
    for (i=0; i<n_galaxies; i++){
        densities_arr[i] = pow( (density_star + density_mean)/(density_star + densities_arr[i]), p );
    }

    /* Clean up */
    kd_res_free(pset);
	kd_free(kd);

	return 0;
}
