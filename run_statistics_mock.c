#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include <utils.h>
#include <upf.h>
#include <marks.h>

/* Header */
int main(int argc, char **argv);

/* Run with: ./run_upf [fn_mock] [L] [Omega_m] [w] [redshift] [r_min] [r_max] [n_radii] [fn_ngalmean] [fn_save] */
int main(int argc, char **argv)
{
	char *fn_mock, *statistic, *fn_save, *fn_ngalmean;
	double L, Omega_m, w, redshift, r_min, r_max, density_threshold_frac, n_galaxies_mean, radius, factor_star, p;
	double *x,*y,*z,*vx,*vy,*vz,*densities_arr;
	int i, n_radii, n_spheres;
    FILE *fp_save, *fp_ngalmean;

	if(argc==17) {
        /*TODO: add checking of inputs*/
        /* all */
		fn_mock = argv[1];
        L = atof(argv[2]);
        Omega_m = atof(argv[3]);
        w = atof(argv[4]);
        redshift = atof(argv[5]);
        statistic = argv[6];
        fn_save = argv[7];
        fn_ngalmean = argv[8];
		
        /* upf */
        r_min = atof(argv[9]);
        r_max = atof(argv[10]);
        n_radii = atoi(argv[11]);
        n_spheres = atoi(argv[12]);
        density_threshold_frac = atof(argv[13]);

        /* marks */
        radius = atof(argv[14]);
        factor_star = atof(argv[15]);
        p = atof(argv[16]);
        
	}
	else {
	    printf("Enter exactly 16 arguments. If you don't need some of these, pass -1.\n");
	    printf("./run_clustering_statistic [fn_mock] [L] [Omega_m] [w] [redshift] [statistic] [fn_save] [fn_ngalmean] [r_min] [r_max] [n_radii] [n_spheres] [density_threshold_frac] [radius] [factor_star] [p]\n");
	    exit(0);
	}

    /* Read in data */
    int n_galaxies = count_lines_in_file(fn_mock);
    x = malloc(sizeof(double)*n_galaxies);
    y = malloc(sizeof(double)*n_galaxies);
    z = malloc(sizeof(double)*n_galaxies);
    vx = malloc(sizeof(double)*n_galaxies);
    vy = malloc(sizeof(double)*n_galaxies);
    vz = malloc(sizeof(double)*n_galaxies);
    load_mock(fn_mock, x, y, z, vx, vy, vz, n_galaxies);

    /* Convert to redshift-space (LOS=z-axis) */
    real_to_redshift_space(z, vz, n_galaxies, L, 
                           redshift, Omega_m, w);

    /* Get mean density threshold */
    fp_ngalmean = fopen(fn_ngalmean,"r");
    if( !(fp_ngalmean=fopen(fn_ngalmean,"r")) ){
      printf("ERROR opening [%s]\n",fn_ngalmean);
      exit(0);
    }
    fscanf(fp_ngalmean,"%lf",&n_galaxies_mean);
    double density_mean = n_galaxies_mean/pow(L, 3); /* 3D */

    /* Open file for saving */
    fp_save = fopen(fn_save,"w");
    if(fp_save == NULL){
        printf("Error! Could not open %s for writing.\n", fn_save);
        exit(1);
    }

    /* Run UPF */
    if (strcmp("upf",statistic) == 0) {
        printf("Running upf\n");
        /* Set up radii for UPF */
        double radii[n_radii];
        linspace(r_min, r_max, n_radii, radii);
        /* Set up other UPF parameters */
        double density_threshold = density_threshold_frac*density_mean;
        double results_arr[n_radii];
        compute_upf(x, y, z, 
                    n_galaxies, radii, n_radii, 
                    n_spheres, density_threshold, 
                    L, results_arr);
        /* Save results */
        for (i=0; i<n_radii; i++){
            fprintf(fp_save,"%f,%f\n",radii[i],results_arr[i]);
        }
    }
    /* Run MCF */
    else if (strcmp("marks",statistic) == 0) {
        printf("Running marks\n");
        densities_arr = malloc(sizeof(double)*n_galaxies);
        compute_marks(x, y, z, 
                    n_galaxies, radius, 
                    density_mean, 
                    factor_star, p,
                    L, densities_arr);
        /* Save results */
        for (i=0; i<n_galaxies; i++){
            fprintf(fp_save,"%f,%d\n",densities_arr[i], i);
        }
    }
    else {
        printf("Statistic %s not recognized! Exiting\n", statistic);
        return -1;
    }

    /* Close save file */
    fclose(fp_save);
    printf("Wrote results to %s\n", fn_save);

	return 0;
}
