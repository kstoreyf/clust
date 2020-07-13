#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#include <sys/time.h>
#include "include/kdtree.h"
#include "include/kdtree_periodic.h"
#include "include/kdtree_periodic.h"


#define PI 3.1415926535897932

int main(int argc, char **argv);
void linspace(double xmin, double xmax, int xnum, double* xarr);
void logspace(double xmin, double xmax, int xnum, double* xarr);
unsigned int get_msec(void);



unsigned int get_msec(void)
{
	static struct timeval timeval, first_timeval;

	gettimeofday(&timeval, 0);

	if(first_timeval.tv_sec == 0) {
		first_timeval = timeval;
		return 0;
	}
	return (timeval.tv_sec - first_timeval.tv_sec) * 1000 + (timeval.tv_usec - first_timeval.tv_usec) / 1000;
}


int main(int argc, char **argv)
{
	char *fn, *fnsave, *fn_ng, *fn_cosmo;
	double L,meanngals,meandens,redshift;
    double Omega_m, Omega_b, sigma_8, h, n_s, N_eff, w, E;
	double *x,*y,*z,*vx,*vy,*vz,*mh,*densities;
    double factor_star, p;
	int *idx;
	int i, dim, ngal, cosmo;
	char string[1000];
    unsigned int msec,start;
    void *kd, *pset;
    FILE *fp, *fp_ng, *fp_cosmo;

    redshift = 0.55;

    srand(time(NULL));
	if(argc==8) {
		fn = argv[1];
		fnsave = argv[2];
    fn_ng = argv[3];
    fn_cosmo = argv[4];
        if (isdigit(argv[5][0]) && isdigit(argv[6][0]) && isdigit(argv[7][0])){
            cosmo = atoi(argv[5]);
            factor_star = atof(argv[6]);
            p = atof(argv[7]);
	    }
	    else{
	        printf("Cosmo must be an integer\n");
	        printf("./markedcf_zspace [filename] [savename] [meandensfile] [cosmoid] [factor_star] [p]\n");
	        exit(0);
	    }
	}
	else {
	    printf("Enter exactly 7 arguments\n");
	    printf("./markedcf_zspace [filename] [savename] [meandensfile] [cosmofile] [cosmoid] [factor_star] [p]\n");
	    exit(0);
	}

    printf("%s\n", fn);

    /* Boxsize - should this be a parameter? */
    L = 1050.0; /* Mpc/h */
    ngal = 0;
    dim = 3;

    fp_cosmo = fopen(fn_cosmo,"r");
    if( !(fp_cosmo=fopen(fn_cosmo,"r")) ){
      printf("ERROR opening [%s]\n",fn_cosmo);
      exit(0);
    }
    int nlines = 0;
    for (i=0; i<cosmo+1; i++){
      fscanf(fp_cosmo,"%lf %lf %lf %lf %lf %lf %lf", &Omega_m, &Omega_b, &sigma_8, &h, &n_s, &N_eff, &w);
      if (nlines==cosmo) {
        break;
      }
      nlines++;
    }
    /* E^2 = H^2/H0^2 */
    E = pow(Omega_m*pow(1+redshift, 3) + (1-Omega_m)*pow(1+redshift, 3*(1+w)), 0.5);
    fclose(fp_cosmo);

    /* get mean density */
    fp_ng = fopen(fn_ng,"r");
    if( !(fp_ng=fopen(fn_ng,"r")) ){
      printf("ERROR opening [%s]\n",fn_ng);
      exit(0);
    }

    fscanf(fp_ng,"%lf",&meanngals);
    meandens = meanngals/pow(L, dim); /* (h/Mpc)^3 */
    printf("mean number density of mocks: %f\n",meandens);
    fclose(fp_ng);

    /* Load data */
    fp = fopen(fn,"r");
	  if( !(fp=fopen(fn,"r")) ){
        printf("ERROR opening [%s]\n",fn);
        exit(0);
    }

    while(fgets(string,1000,fp)){
        ngal++;
    }
    rewind(fp);
    printf("ngals: %d\n", ngal);

    x = malloc(sizeof(double)*ngal);
    y = malloc(sizeof(double)*ngal);
    z = malloc(sizeof(double)*ngal);
    vx = malloc(sizeof(double)*ngal);
    vy = malloc(sizeof(double)*ngal);
    vz = malloc(sizeof(double)*ngal);
    mh = malloc(sizeof(double)*ngal);
    idx = malloc(sizeof(int)*ngal);

    printf("Reading file %s...\n", fn);

    for(i = 0; i < ngal; i++){
        fscanf(fp,"%lf %lf %lf %lf %lf %lf %*d %lf",&x[i],&y[i],&z[i],&vx[i],&vy[i],&vz[i],&mh[i]);
        idx[i] = i;
        fgets(string,1000,fp);
    }

    fclose(fp);

    /* convert to redshift space, with line of sight along z */
    double z_zspace; 
    for (i=0; i<ngal; i++){
      z_zspace = z[i]+vz[i]*(1+redshift)/(E*100);
      if (z_zspace < 0) { 
        z_zspace += L;
      }
      if (z_zspace >= L) {
        z_zspace -= L;
      }
      z[i] = z_zspace;
    }

    /* Build kdtree */
    kd = kd_create(dim);
	start = get_msec();
	for(i=0; i<ngal; i++) {
		  assert(kd_insert3(kd, x[i], y[i], z[i], 0) == 0);
	}
	msec = get_msec() - start;
	printf("Built tree in %.3f sec\n", (float)msec / 1000.0);

    /* loop through each halo to get its local density */
    /* i think i dont need to loop radii here - single local density metric per galaxy */
    densities = malloc(sizeof(double)*ngal);

    start = get_msec();
    double distance = 10.0; /*how to choose?? --> guesstimated the same as was used in Satpathy 2019 (sdss)!*/
    double vol_sphere = 4.0/3.0 * PI * distance*distance*distance;
    for (i=0; i<ngal; i++){
        pset = kd_nearest_range3_periodic(kd, x[i], y[i], z[i], distance, L);
        densities[i] = (double) kd_res_size(pset) / vol_sphere; /*n neighbors*/
    }
    msec = get_msec() - start;
    
    /* compute marks using Satpathy 2019 (sdss) eqn 5 */
    printf("factor_star=%f, p=%f\n", factor_star, p);
    double dens_star = factor_star*meandens;
    /*printf("WARNING, USING PADWHITE2009 MARK\n");*/
    for (i=0; i<ngal; i++){
        densities[i] = pow( (dens_star + meandens)/(dens_star + densities[i]), p );
        /*densities[i] = densities[i]/(dens_star + densities[i]);*/ /* PADWHITE2009 */ 
    }

    FILE *fptr;
    
    fptr = fopen(fnsave,"w");
    if(fptr == NULL){
        printf("Error! Could not open %s for writing.\n", fnsave);
        exit(1);
    }

    /* save densities and indices */
    for (i=0; i<ngal; i++){
        fprintf(fptr,"%f,%d\n",densities[i], idx[i]);
    }

    fclose(fptr);
    printf("Wrote results to %s\n", fnsave);
    kd_res_free(pset);

	kd_free(kd);
	return 0;
}


void logspace(double xmin, double xmax, int xnum, double* xarr){
    double logspace = (log10(xmax) - log10(xmin))/(xnum-1);
    int i;
    for (i=0; i<xnum; i++){
        double logx = log10(xmin) + i*logspace;
        xarr[i] = pow(10, logx);
    }
}

void linspace(double xmin, double xmax, int xnum, double* xarr){
    double space = (xmax - xmin)/(xnum-1);
    int i;
    for (i=0; i<xnum; i++){
        xarr[i] = xmin + i*space;
    }
}
