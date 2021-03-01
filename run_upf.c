#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

#include <utils.h>
#include <upf.h>

/* Header */
int main(int argc, char **argv);

/* Run with: ./run_upf [fn_mock] [L] [Omega_m] [w] [redshift] [r_min] [r_max] [n_radii] [fn_ngalmean] [fn_save] */
int main(int argc, char **argv)
{
	char *fn_mock, *fn_save, *fn_ngalmean;
	double L, Omega_m, w, redshift, r_min, r_max, density_threshold_frac, n_galaxies_mean;
	double *x,*y,*z,*vx,*vy,*vz;
	int i, n_radii;
    FILE *fp_save, *fp_ngalmean;

	if(argc==12) {
		fn_mock = argv[1];
        L = atof(argv[2]);
        Omega_m = atof(argv[3]);
        w = atof(argv[4]);
        redshift = atof(argv[5]);
        r_min = atof(argv[6]);
        r_max = atof(argv[7]);
        n_radii = atoi(argv[8]);
        density_threshold_frac = atof(argv[9]);
        fn_ngalmean = argv[10];
		fn_save = argv[11];
	}
	else {
	    printf("Enter exactly 10 arguments\n");
	    printf("./run_upf [fn_mock] [L] [Omega_m] [w] [redshift] [r_min] [r_max] [n_radii] [density_threshold_frac] [fn_ngalmean] [fn_save]\n");
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

    /* Set up radii for UPF */
    double radii[n_radii];
    linspace(r_min, r_max, n_radii, radii);

    /* Set density threshold */
    fp_ngalmean = fopen(fn_ngalmean,"r");
    if( !(fp_ngalmean=fopen(fn_ngalmean,"r")) ){
      printf("ERROR opening [%s]\n",fn_ngalmean);
      exit(0);
    }
    fscanf(fp_ngalmean,"%lf",&n_galaxies_mean);
    double density_mean = n_galaxies_mean/pow(L, 3); /* 3D */
    double density_threshold = density_threshold_frac*density_mean;

    /* Run UPF */
    int n_spheres = 1e6;
    double results_arr[n_radii];
    compute_upf(x, y, z, 
                n_galaxies, radii, n_radii, 
                n_spheres, density_threshold, 
                L, results_arr);

    /* Save results to file */
    fp_save = fopen(fn_save,"w");
    if(fp_save == NULL){
        printf("Error! Could not open %s for writing.\n", fn_save);
        exit(1);
    }
    for (i=0; i<n_radii; i++){
        fprintf(fp_save,"%f,%f\n",radii[i],results_arr[i]);
    }
    fclose(fp_save);
    printf("Wrote results to %s\n", fn_save);

	return 0;
}
